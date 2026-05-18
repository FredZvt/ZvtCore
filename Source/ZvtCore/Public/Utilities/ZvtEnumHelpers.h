#pragma once

/**
 * Template utility functions for working with UENUM types.
 *
 * All methods that accept an EnumType parameter require StaticEnum<EnumType>() to be non-null,
 * meaning the enum must be decorated with the UENUM() macro. Methods that fail this requirement
 * log an error and return a safe default value.
 *
 * Naming conventions used throughout this struct:
 * - "short name"  - enumerator name with the type prefix stripped (e.g. "ValueA" from "EMyEnum::ValueA").
 * - "full name"   - fully-qualified enumerator name including the type prefix (e.g. "EMyEnum::ValueA").
 */
struct FZvtEnumHelpers
{
	/**
	 * Returns the localized display text for an enum value by looking up its short name in a string table.
	 *
	 * @param EnumValue     The enum value whose display text is requested.
	 * @param StringTableId FName ID of the target string table asset (e.g. "ST_MyTable").
	 * @return              The FText entry from the string table, or an empty FText if the key is not found.
	 */
	template<typename EnumType>
	static FText GetDisplayTextFor(EnumType EnumValue, const FName& StringTableId)
	{
		const FString EnumValueString = GetNameByValue(EnumValue).ToString();
		return FText::FromStringTable(StringTableId, EnumValueString);
	}

	/**
	 * Fills OutNames with the fully-qualified name of every enumerator (e.g. "EMyEnum::ValueA"),
	 * excluding the compiler-generated _MAX sentinel.
	 *
	 * @param OutNames  Array to receive the names. Not cleared before adding.
	 * @return          true on success; false if StaticEnum<EnumType>() is null.
	 */
	template<typename EnumType>
	static bool GetEnumFullNames(TArray<FName>& EnumNames)
	{
		const UEnum* EnumPtr = StaticEnum<EnumType>();
		if (!EnumPtr)
		{
			UE_LOG(LogTemp, Error, TEXT("EnumPtr is null."));
			return false;
		}

		for (int32 i = 0; i < EnumPtr->NumEnums() - 1; ++i)
		{
			EnumNames.Add(EnumPtr->GetNameByIndex(i));
		}

		return true;
	}

	/**
	 * Fills OutNames with the short name of every enumerator (prefix stripped, e.g. "ValueA"),
	 * excluding the _MAX sentinel.
	 *
	 * @param OutNames  Array to receive the names. Not cleared before adding.
	 * @return          true on success; false if StaticEnum<EnumType>() is null.
	 */
	template<typename EnumType>
	static bool GetEnumNames(TArray<FName>& EnumNames)
	{
		const UEnum* EnumPtr = StaticEnum<EnumType>();
		if (!EnumPtr)
		{
			UE_LOG(LogTemp, Error, TEXT("EnumPtr is null."));
			return false;
		}

		for (int32 i = 0; i < EnumPtr->NumEnums() - 1; ++i)
		{
			EnumNames.Add(
				RemoveEnumPrefix(
					EnumPtr->GetNameByIndex(i)
					));
		}

		return true;
	}

	/**
	 * Fills OutStrings with the short name string of every enumerator, skipping entries
	 * whose name ends with "_MAX".
	 *
	 * @param OutStrings  Array to receive the strings. Not cleared before adding.
	 * @return            true on success; false if StaticEnum<EnumType>() is null.
	 */
	template<typename EnumType>
	static bool GetEnumStrings(TArray<FString>& EnumStrings)
	{
		static_assert(TIsEnum<EnumType>::Value, "TEnum must be an enum type");

		const UEnum* EnumPtr = StaticEnum<EnumType>();
		if (!EnumPtr)
		{
			UE_LOG(LogTemp, Error, TEXT("EnumPtr is null."));
			return false;
		}

		for (int32 i = 0; i < EnumPtr->NumEnums(); ++i)
		{
			const FString Name = EnumPtr->GetNameStringByIndex(i);

			// Skip *_MAX entries
			if (Name.EndsWith(TEXT("_MAX")))
			{
				continue;
			}

			EnumStrings.Add(Name);
		}

		return true;
	}

	/**
	 * Returns the fully-qualified name of a single enum value (e.g. "EMyEnum::ValueA"),
	 * or NAME_None if StaticEnum<EnumType>() is null.
	 *
	 * @param EnumValue  The value to query.
	 */
	template<typename EnumType>
	static FName GetEnumFullName(const EnumType EnumValue)
	{
		const UEnum* EnumPtr = StaticEnum<EnumType>();
		if (!EnumPtr)
		{
			UE_LOG(LogTemp, Error, TEXT("EnumPtr is null."));
			return NAME_None;
		}

		return EnumPtr->GetNameByValue(ToUnderlyingValue(EnumValue));
	}

	/**
	 * Returns the short name of a single enum value as a string (prefix stripped, e.g. "ValueA"),
	 * or "None" if StaticEnum<EnumType>() is null.
	 *
	 * @param EnumValue  The value to query.
	 */
	template<typename EnumType>
	static FString GetEnumString(const EnumType EnumValue)
	{
		static_assert(TIsEnum<EnumType>::Value, "TEnum must be an enum type");

		const UEnum* EnumPtr = StaticEnum<EnumType>();
		if (!EnumPtr)
		{
			UE_LOG(LogTemp, Error, TEXT("EnumPtr is null."));
			return FString(TEXT("None"));
		}

		return EnumPtr->GetNameStringByValue(ToUnderlyingValue(EnumValue));
	}

	/**
	 * Returns the short name of a single enum value as an FName (prefix stripped, e.g. "ValueA"),
	 * or NAME_None on failure.
	 *
	 * @param EnumValue  The value to query.
	 */
	template<typename EnumType>
	static FName GetEnumName(const EnumType EnumValue)
	{
		return RemoveEnumPrefix(GetEnumFullName<EnumType>(EnumValue));
	}

	/**
	 * Returns an array containing all enumerator values in declaration order, excluding the _MAX sentinel.
	 * Returns an empty array if StaticEnum<EnumType>() is null.
	 */
	template <typename EnumType>
	static TArray<EnumType> GetAllEnumValues()
	{
		const UEnum* EnumPtr = StaticEnum<EnumType>();
		if (!EnumPtr)
		{
			UE_LOG(LogTemp, Error, TEXT("EnumPtr is null."));
			return TArray<EnumType>();
		}

		TArray<EnumType> EnumValues;
		for (int32 i = 0; i < EnumPtr->NumEnums() - 1; ++i)
		{
			EnumValues.Add(static_cast<EnumType>(i));
		}

		return EnumValues;
	}

	/**
	 * Casts an enum value to its underlying integer type at compile time.
	 *
	 * @param EnumValue  The value to convert.
	 * @return           The underlying integer representation of EnumValue.
	 */
	template <typename EnumType>
	static constexpr std::underlying_type_t<EnumType> ToUnderlyingValue(EnumType EnumValue)
	{
		return static_cast<std::underlying_type_t<EnumType>>(EnumValue);
	}

	/**
	 * Returns the fully-qualified name for an enum value (e.g. "EMyEnum::ValueA"),
	 * or NAME_None on failure. Equivalent to GetEnumFullName(); prefer that name.
	 *
	 * @param EnumValue  The value to query.
	 */
	template <typename EnumType>
	static FName GetFullNameByValue(EnumType EnumValue)
	{
		const UEnum* EnumPtr = StaticEnum<EnumType>();
		if (!EnumPtr)
		{
			UE_LOG(LogTemp, Error, TEXT("EnumPtr is null."));
			return NAME_None;
		}

		return EnumPtr->GetNameByValue(ToUnderlyingValue(EnumValue));
	}

	/**
	 * Returns the short name for an enum value (prefix stripped, e.g. "ValueA"),
	 * or NAME_None on failure. Equivalent to GetEnumName(); prefer that name.
	 *
	 * @param EnumValue  The value to query.
	 */
	template <typename EnumType>
	static FName GetNameByValue(EnumType EnumValue)
	{
		return RemoveEnumPrefix(GetFullNameByValue(EnumValue));
	}

	/**
	 * Looks up an enum value by its short name (without the "EnumType::" prefix).
	 *
	 * @param Name   Short name to search for (e.g. "ValueA").
	 * @param Value  Receives the found value on success; set to EnumType() on failure.
	 * @return       true if found; false if StaticEnum<EnumType>() is null or the name is unknown.
	 */
	template <typename EnumType>
	static bool GetValueByName(const FName Name, EnumType& Value)
	{
		const UEnum* EnumPtr = StaticEnum<EnumType>();
		if (!EnumPtr)
		{
			UE_LOG(LogTemp, Error, TEXT("EnumPtr is null."));
			Value = EnumType();
			return false;
		}

		const int32 Index = EnumPtr->GetIndexByName(Name);
		if (Index == INDEX_NONE)
		{
			UE_LOG(LogTemp, Error, TEXT("Enum name '%s' not found."), *Name.ToString());
			Value = EnumType();
			return false;
		}

		Value = static_cast<EnumType>(Index);
		return true;
	}

	/**
	 * Looks up an enum value by its short name string (without the "EnumType::" prefix).
	 *
	 * @param String  Short name string to search for (e.g. "ValueA").
	 * @param Value   Receives the found value on success; set to EnumType() on failure.
	 * @return        true if found; false if StaticEnum<EnumType>() is null or the name is unknown.
	 */
	template <typename EnumType>
	static bool GetValueByString(const FString String, EnumType& Value)
	{
		const UEnum* EnumPtr = StaticEnum<EnumType>();
		if (!EnumPtr)
		{
			UE_LOG(LogTemp, Error, TEXT("EnumPtr is null."));
			Value = EnumType();
			return false;
		}

		const int32 Index = EnumPtr->GetIndexByNameString(String);
		if (Index == INDEX_NONE)
		{
			UE_LOG(LogTemp, Error, TEXT("Enum name '%s' not found."), *String);
			Value = EnumType();
			return false;
		}

		Value = static_cast<EnumType>(Index);
		return true;
	}

	/**
	 * Strips the "EnumType::" prefix from a fully-qualified enum name.
	 * If the name contains no '::' separator it is returned unchanged.
	 *
	 * @param FullName  Fully-qualified name (e.g. "EMyEnum::ValueA").
	 * @return          Short name (e.g. "ValueA").
	 */
	static FName RemoveEnumPrefix(const FName& FullName)
	{
		int32 Index;
		const FString FullNameString = FullName.ToString();
		if (FullNameString.FindLastChar(TEXT(':'), Index))
		{
			return FName(FullNameString.Mid(Index + 1));
		}
		return FullName;
	}

	/**
	 * Returns the next enum value in sequence, wrapping from LastEnumValue back to FirstEnumValue.
	 * Assumes contiguous underlying integer values.
	 *
	 * @param FirstEnumValue    The first value in the valid range (wrap target).
	 * @param LastEnumValue     The last value in the valid range.
	 * @param CurrentEnumValue  The current value.
	 * @return                  CurrentEnumValue + 1, or FirstEnumValue if CurrentEnumValue == LastEnumValue.
	 */
	template <typename EnumType>
	static EnumType CycleToNextEnumValue(
		const EnumType& FirstEnumValue,
		const EnumType& LastEnumValue,
		const EnumType& CurrentEnumValue)
	{
		std::underlying_type_t<EnumType> LastEnumValueUnderlying = ToUnderlyingValue(LastEnumValue);
		std::underlying_type_t<EnumType> CurrentEnumValueUnderlying = ToUnderlyingValue(CurrentEnumValue);

		if (CurrentEnumValueUnderlying == LastEnumValueUnderlying)
		{
			return FirstEnumValue;
		}

		return static_cast<EnumType>(CurrentEnumValueUnderlying + 1);
	}

	/**
	 * Returns the next enum value, clamping at LastEnumValue (no wrap).
	 * Assumes contiguous underlying integer values.
	 *
	 * @param LastEnumValue     The maximum value; the result will never exceed this.
	 * @param CurrentEnumValue  The current value.
	 * @return                  CurrentEnumValue + 1, or LastEnumValue if already at the end.
	 */
	template <typename EnumType>
	static EnumType GetNextEnumValue(
		const EnumType& LastEnumValue,
		const EnumType& CurrentEnumValue)
	{
		std::underlying_type_t<EnumType> LastEnumValueUnderlying = ToUnderlyingValue(LastEnumValue);
		std::underlying_type_t<EnumType> CurrentEnumValueUnderlying = ToUnderlyingValue(CurrentEnumValue);

		if (CurrentEnumValueUnderlying == LastEnumValueUnderlying)
		{
			return CurrentEnumValue;
		}

		return static_cast<EnumType>(CurrentEnumValueUnderlying + 1);
	}

	/**
	 * Returns the previous enum value, clamping at FirstEnumValue (no wrap).
	 * Assumes contiguous underlying integer values.
	 *
	 * @param FirstEnumValue    The minimum value; the result will never go below this.
	 * @param CurrentEnumValue  The current value.
	 * @return                  CurrentEnumValue - 1, or FirstEnumValue if already at the start.
	 */
	template <typename EnumType>
	static EnumType GetPreviousEnumValue(
		const EnumType& FirstEnumValue,
		const EnumType& CurrentEnumValue)
	{
		std::underlying_type_t<EnumType> FirstEnumValueUnderlying = ToUnderlyingValue(FirstEnumValue);
		std::underlying_type_t<EnumType> CurrentEnumValueUnderlying = ToUnderlyingValue(CurrentEnumValue);

		if (CurrentEnumValueUnderlying == FirstEnumValueUnderlying)
		{
			return CurrentEnumValue;
		}

		return static_cast<EnumType>(CurrentEnumValueUnderlying - 1);
	}
};
