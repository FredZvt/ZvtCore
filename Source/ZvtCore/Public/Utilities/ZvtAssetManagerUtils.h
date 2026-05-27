#pragma once

#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

struct FZvtAssetManagerUtils
{
private:

	/** Keeps discovered IDs and the typed callback alive across the async gap.
	 *  CreateSP stores only a weak ref, so TLoadContext keeps a SelfRef (strong ptr) to
	 *  prevent premature destruction. SelfRef is cleared inside OnStreamingComplete after
	 *  the callback fires, which is the point at which the context may be released. */
	template<typename T>
	struct TLoadContext
	{
		TArray<FPrimaryAssetId>                      FoundIds;
		TDelegate<void(TArray<T*>, TArray<FString>)> OnLoaded;
		TSharedPtr<TLoadContext<T>>                  SelfRef;

		/** Called by FStreamableDelegate when streaming finishes. Resolves each discovered
		 *  ID to a typed pointer. Any failure (null object or bad cast) produces an entry
		 *  in Errors so callers are never silently left without a diagnostic. */
		void OnStreamingComplete()
		{
			const UAssetManager& AssetManager = UAssetManager::Get();
			TArray<T*> Result;
			TArray<FString> Errors;

			for (const FPrimaryAssetId& Id : FoundIds)
			{
				UObject* Obj = AssetManager.GetPrimaryAssetObject(Id);
				if (!Obj)
				{
					Errors.Add(FString::Printf(TEXT("Asset '%s' failed to load (null object)."), *Id.ToString()));
					continue;
				}

				T* TypedObj = Cast<T>(Obj);
				if (!TypedObj)
				{
					Errors.Add(FString::Printf(TEXT("Asset '%s' loaded but cast to %s failed."), *Id.ToString(), *T::StaticClass()->GetName()));
					continue;
				}

				Result.Add(TypedObj);
			}

			OnLoaded.ExecuteIfBound(Result, Errors);
			SelfRef.Reset(); // release after firing; context may be destroyed after this line
		}
	};

public:

	/**
	 * Discovers all registered Primary Assets of type TPrimaryAssetType, loads them
	 * asynchronously via the Asset Manager, then calls Owner->Callback with the typed
	 * results and a (possibly empty) list of per-asset error messages.
	 *
	 * Typical call site:
	 *   FZvtAssetManagerUtils::LoadPrimaryAssets<UCharacterMetaData>(
	 *       this, &UMyClass::OnCharactersLoaded);
	 *
	 * @param Owner             UObject whose lifetime gates the callback. If the owner is
	 *                          garbage-collected before loading finishes the callback is
	 *                          safely skipped (CreateUObject weak-ref semantics).
	 * @param Callback          Member function called on completion. Receives the typed
	 *                          asset array and any per-asset diagnostic strings.
	 * @param PrimaryAssetType  The type name to query in the Asset Manager. Leave default
	 *                          to infer from TPrimaryAssetType's class name (works when the
	 *                          registered Primary Asset Type matches the class name).
	 * @param Bundles           Asset bundle names to load alongside the primary asset.
	 *                          Leave empty to load only the default asset state.
	 */
	template<typename TPrimaryAssetType, typename TOwner>
	static void LoadPrimaryAssets(
		TOwner* Owner,
		void (TOwner::*Callback)(TArray<TPrimaryAssetType*>, TArray<FString>),
		FPrimaryAssetType PrimaryAssetType = FPrimaryAssetType(),
		TArray<FName> Bundles = TArray<FName>())
	{
		// Infer the type name from the class if the caller did not supply one.
		if (!PrimaryAssetType.IsValid())
		{
			PrimaryAssetType = FPrimaryAssetType(TPrimaryAssetType::StaticClass()->GetFName());
		}

		UAssetManager& AssetManager = UAssetManager::Get();

		TArray<FPrimaryAssetId> FoundIds;
		AssetManager.GetPrimaryAssetIdList(PrimaryAssetType, FoundIds);

		// Nothing to load. Call back immediately with an empty result (not an error).
		if (FoundIds.Num() == 0)
		{
			TArray<TPrimaryAssetType*> EmptyResult;
			TArray<FString> NoErrors;
			(Owner->*Callback)(EmptyResult, NoErrors);
			return;
		}

		// Allocate a shared context so the ID list and callback survive the async gap.
		// SelfRef pins the strong ref count to >= 1 until OnStreamingComplete fires,
		// because CreateSP only holds a weak ref and the local TSharedRef goes out of scope
		// when this function returns.
		TSharedRef<TLoadContext<TPrimaryAssetType>> Context = MakeShared<TLoadContext<TPrimaryAssetType>>();
		Context->FoundIds = MoveTemp(FoundIds);
		Context->OnLoaded = TDelegate<void(TArray<TPrimaryAssetType*>, TArray<FString>)>::CreateUObject(Owner, Callback);
		Context->SelfRef = Context;

		const FStreamableDelegate LoadDelegate =
			FStreamableDelegate::CreateSP(Context, &TLoadContext<TPrimaryAssetType>::OnStreamingComplete);

		AssetManager.LoadPrimaryAssets(Context->FoundIds, Bundles, LoadDelegate);
	}
};
