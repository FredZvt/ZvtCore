#pragma once

/** Miscellaneous file-path utility functions. */
struct FZvtUtils
{
	/**
	 * Converts an absolute file path to a path relative to the project's Content directory.
	 *
	 * The returned path begins with '/' and is suitable wherever UE expects a content-relative
	 * path (e.g. FStreamableManager or FObjectFinder). If AbsoluteFilePath is not located
	 * inside the Content directory the result is undefined.
	 *
	 * @param AbsoluteFilePath  Absolute path to a file inside the project's Content folder.
	 * @return                  Content-relative path starting with '/'.
	 */
	static FString ExtractRelativeFilePath(const FString& AbsoluteFilePath)
	{
		const FString ProjectContentDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir());
		FString RelativeFilePath = AbsoluteFilePath.Mid(ProjectContentDir.Len() - 1);
		return RelativeFilePath;
	}

#if WITH_EDITOR
	static FString GetPIEInstanceIdentifier()
	{
		if (!GWorld) { return {}; }
		const FWorldContext* WorldContext = GEngine->GetWorldContextFromWorld(GWorld);
		if (!WorldContext) { return {}; }
		switch (GWorld->GetNetMode())
		{
			case NM_Standalone:      return FString::Printf(TEXT("Standalone %d"), WorldContext->PIEInstance);
			case NM_DedicatedServer: return FString::Printf(TEXT("DedicatedServer %d"), WorldContext->PIEInstance);
			case NM_ListenServer:    return FString::Printf(TEXT("ListenServer %d"), WorldContext->PIEInstance);
			case NM_Client:          return FString::Printf(TEXT("Client %d"), WorldContext->PIEInstance);
			default:                 return {};
		}
	}
#endif
};
