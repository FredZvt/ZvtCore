#pragma once

#include "ZvtUtils.h"

/**
 * @file ZvtMacros.h
 *
 * Provides two categories of macros:
 *
 * 1. Log injection macros - declared in the class body to inject LogInfo / LogWarning / LogError
 *    helper methods, which are then called by ZVT_LOG, ZVT_LOG_WARNING, and ZVT_LOG_ERROR.
 *    Must be present in any class that uses those logging macros or the defensive return macros.
 *
 * 2. Defensive return macros - check a condition, log an error via ZVT_LOG_ERROR, and return
 *    (or continue) early. They replace the repeated boilerplate of a null check, a log call,
 *    and a return statement with a single readable line.
 */

#if WITH_EDITOR
#include "Widgets/Notifications/SNotificationList.h"
#include "Framework/Notifications/NotificationManager.h"
#endif

// ---------------------------------------------------------------------------
// Null pointer checks
// ---------------------------------------------------------------------------

/** @brief Logs an error and returns void if @p Ptr is null. @param Ptr The pointer to check. */
#define ZVT_RETURN_IF_NULL(Ptr) if (!Ptr) { ZVT_LOG_ERROR(TEXT("%s is null."), TEXT(#Ptr)) return; }

/** @brief Logs an error and returns @p Value if @p Ptr is null. @param Ptr The pointer to check. @param Value The value to return. */
#define ZVT_RETURN_VALUE_IF_NULL(Ptr, Value) if (!Ptr) { ZVT_LOG_ERROR(TEXT("%s is null."), TEXT(#Ptr)) return Value; }

/** @brief Logs an error and returns nullptr if @p Ptr is null. @param Ptr The pointer to check. */
#define ZVT_RETURN_NULL_IF_NULL(Ptr) if (!Ptr) { ZVT_LOG_ERROR(TEXT("%s is null."), TEXT(#Ptr)) return nullptr; }

/** @brief Logs an error and returns false if @p Ptr is null. @param Ptr The pointer to check. */
#define ZVT_RETURN_FALSE_IF_NULL(Ptr) if (!Ptr) { ZVT_LOG_ERROR(TEXT("%s is null."), TEXT(#Ptr)) return false; }

/** @brief Logs an error and returns true if @p Ptr is null. @param Ptr The pointer to check. */
#define ZVT_RETURN_TRUE_IF_NULL(Ptr) if (!Ptr) { ZVT_LOG_ERROR(TEXT("%s is null."), TEXT(#Ptr)) return true; }

/** @brief Logs an error and returns 0 if @p Ptr is null. @param Ptr The pointer to check. */
#define ZVT_RETURN_ZERO_IF_NULL(Ptr) if (!Ptr) { ZVT_LOG_ERROR(TEXT("%s is null."), TEXT(#Ptr)) return 0; }

/** @brief Logs an error and returns FVector::ZeroVector if @p Ptr is null. @param Ptr The pointer to check. */
#define ZVT_RETURN_ZEROVECTOR_IF_NULL(Ptr) if (!Ptr) { ZVT_LOG_ERROR(TEXT("%s is null."), TEXT(#Ptr)) return FVector::ZeroVector; }

/** @brief Logs an error and returns NAME_None if @p Ptr is null. @param Ptr The pointer to check. */
#define ZVT_RETURN_NAME_NONE_IF_NULL(Ptr) if (!Ptr) { ZVT_LOG_ERROR(TEXT("%s is null."), TEXT(#Ptr)) return NAME_None; }

/**
 * @brief Logs an error and returns void if @p Ptr is a null TSoftObjectPtr / TSoftClassPtr.
 * Uses Ptr.IsNull() rather than a raw pointer check.
 * @param Ptr The soft pointer to check.
 */
#define ZVT_RETURN_IF_NULL_SOFTPTR(Ptr) if (Ptr.IsNull()) { ZVT_LOG_ERROR(TEXT("%s is null."), TEXT(#Ptr)) return; }

/** @brief Logs an error and executes continue if @p Ptr is null. For use inside loops. @param Ptr The pointer to check. */
#define ZVT_CONTINUE_IF_NULL(Ptr) if (!Ptr) { ZVT_LOG_ERROR(TEXT("%s is null."), TEXT(#Ptr)) continue; }

// ---------------------------------------------------------------------------
// TWeakObjectPtr / TWeakPtr validity checks
// ---------------------------------------------------------------------------

/** @brief Logs an error and returns void if @p Ptr.IsValid() is false. @param Ptr The weak pointer to check. */
#define ZVT_RETURN_IF_NOT_VALID(Ptr) if (!Ptr.IsValid()) { ZVT_LOG_ERROR(TEXT("%s is not valid."), TEXT(#Ptr)); return; }

/** @brief Logs an error and returns false if @p Ptr.IsValid() is false. @param Ptr The weak pointer to check. */
#define ZVT_RETURN_FALSE_IF_NOT_VALID(Ptr) if (!Ptr.IsValid()) { ZVT_LOG_ERROR(TEXT("%s is not valid."), TEXT(#Ptr)); return false; }

/** @brief Logs an error and returns @p Value if @p Ptr.IsValid() is false. @param Ptr The weak pointer to check. @param Value The value to return. */
#define ZVT_RETURN_VALUE_IF_NOT_VALID(Ptr, Value) if (!Ptr.IsValid()) { ZVT_LOG_ERROR(TEXT("%s is not valid."), TEXT(#Ptr)); return Value; }

/** @brief Logs an error and executes continue if @p Ptr.IsValid() is false. For use inside loops. @param Ptr The weak/shared pointer to check. */
#define ZVT_CONTINUE_IF_NOT_VALID(Ptr) if (!Ptr.IsValid()) { ZVT_LOG_ERROR(TEXT("%s is not valid."), TEXT(#Ptr)); continue; }

// ---------------------------------------------------------------------------
// Boolean checks
// ---------------------------------------------------------------------------

/** @brief Logs an error and returns void if @p BoolValue is false. @param BoolValue The condition to test. */
#define ZVT_RETURN_IF_FALSE(BoolValue) if (!BoolValue) { ZVT_LOG_ERROR(TEXT("%s is false."), TEXT(#BoolValue)) return; }

/** @brief Logs an error and returns false if @p BoolValue is false. @param BoolValue The condition to test. */
#define ZVT_RETURN_FALSE_IF_FALSE(BoolValue) if (!BoolValue) { ZVT_LOG_ERROR(TEXT("%s is false."), TEXT(#BoolValue)) return false; }

/** @brief Logs an error and returns nullptr if @p BoolValue is false. @param BoolValue The condition to test. */
#define ZVT_RETURN_NULL_IF_FALSE(BoolValue) if (!BoolValue) { ZVT_LOG_ERROR(TEXT("%s is false."), TEXT(#BoolValue)) return nullptr; }

/** @brief Logs an error and returns @p Value if @p BoolValue is false. @param BoolValue The condition to test. @param Value The value to return. */
#define ZVT_RETURN_VALUE_IF_FALSE(BoolValue, Value) if (!BoolValue) { ZVT_LOG_ERROR(TEXT("%s is false."), TEXT(#BoolValue)) return Value; }

/** @brief Logs an error and returns void if @p BoolValue is true. @param BoolValue The condition to test. */
#define ZVT_RETURN_IF_TRUE(BoolValue) if (BoolValue) { ZVT_LOG_ERROR(TEXT("%s is true."), TEXT(#BoolValue)) return; }

/** @brief Logs an error and executes continue if @p BoolValue is false. For use inside loops. @param BoolValue The condition to test. */
#define ZVT_CONTINUE_IF_FALSE(BoolValue) if (!BoolValue) { ZVT_LOG_ERROR(TEXT("%s is false."), TEXT(#BoolValue)) continue; }

/** @brief Logs an error and executes continue if @p BoolValue is true. For use inside loops. @param BoolValue The condition to test. */
#define ZVT_CONTINUE_IF_TRUE(BoolValue) if (BoolValue) { ZVT_LOG_ERROR(TEXT("%s is true."), TEXT(#BoolValue)) continue; }

// ---------------------------------------------------------------------------
// Other
// ---------------------------------------------------------------------------

/** @brief Logs an error and returns void if @p Value equals zero. @param Value The integer value to check. */
#define ZVT_RETURN_IF_ZERO(Value) if (Value == 0) { ZVT_LOG_ERROR(TEXT("%s is zero."), TEXT(#Value)) return; }

/**
 * @def ZVT_LOG_FUNCTIONS(CategoryName)
 * @brief Injects LogInfo, LogWarning, and LogError instance methods into a UObject-derived class.
 *
 * Place this macro in the class body (no semicolon needed). The injected methods format the
 * message with the source file, line number, and the object's GetName(), then forward to UE_LOG
 * using @p CategoryName. After injection, use ZVT_LOG, ZVT_LOG_WARNING, and ZVT_LOG_ERROR to log.
 *
 * In WITH_EDITOR builds, LogError additionally pops a Slate toast notification.
 * In NO_LOGGING builds, all injected methods and the ZVT_LOG macros expand to nothing.
 *
 * @param CategoryName  An existing UE log category (e.g. LogMyGame). Must be declared before
 *                      the class that uses this macro.
 *
 * Example:
 * @code
 *   UCLASS()
 *   class UMyComponent : public UActorComponent
 *   {
 *       GENERATED_BODY()
 *       ZVT_LOG_FUNCTIONS(LogMyGame)
 *   };
 * @endcode
 */

/**
 * @def ZVT_LOG_FUNCTIONS_NOT_UOBJECT(CategoryName)
 * @brief Injects static LogInfo, LogWarning, and LogError methods into a plain struct or class.
 *
 * Identical to ZVT_LOG_FUNCTIONS but injects static methods (no GetName() call), making it
 * suitable for types that do not inherit from UObject.
 *
 * @param CategoryName  An existing UE log category.
 *
 * Example:
 * @code
 *   struct FMyHelper
 *   {
 *       ZVT_LOG_FUNCTIONS_NOT_UOBJECT(LogMyGame)
 *   };
 * @endcode
 */

/**
 * @def ZVT_LOG(Format, ...)
 * @brief Logs an informational message at Verbose/Log level.
 *
 * Requires ZVT_LOG_FUNCTIONS or ZVT_LOG_FUNCTIONS_NOT_UOBJECT to be declared in the calling class.
 * Expands to nothing in NO_LOGGING builds.
 *
 * @param Format  UE printf-style format string (TEXT("...")).
 * @param ...     Optional format arguments.
 */

/**
 * @def ZVT_LOG_WARNING(Format, ...)
 * @brief Logs a warning message.
 *
 * Requires ZVT_LOG_FUNCTIONS or ZVT_LOG_FUNCTIONS_NOT_UOBJECT to be declared in the calling class.
 * Expands to nothing in NO_LOGGING builds.
 *
 * @param Format  UE printf-style format string.
 * @param ...     Optional format arguments.
 */

/**
 * @def ZVT_LOG_ERROR(Format, ...)
 * @brief Logs an error message. In WITH_EDITOR builds, also shows a Slate toast notification.
 *
 * Requires ZVT_LOG_FUNCTIONS or ZVT_LOG_FUNCTIONS_NOT_UOBJECT to be declared in the calling class.
 * Called automatically by all defensive return macros. Expands to nothing in NO_LOGGING builds.
 *
 * @param Format  UE printf-style format string.
 * @param ...     Optional format arguments.
 */

/**
 * @def ZVT_LOG_IF(CVar, Format, ...)
 * @brief Logs an informational message only when the given console variable is true.
 *
 * Requires ZVT_LOG_FUNCTIONS or ZVT_LOG_FUNCTIONS_NOT_UOBJECT to be declared in the calling class.
 * Safe to call from any thread. Expands to nothing in NO_LOGGING builds.
 *
 * @param CVar    A TAutoConsoleVariable<bool> whose value gates the log call.
 * @param Format  UE printf-style format string (TEXT("...")).
 * @param ...     Optional format arguments.
 */

#if NO_LOGGING

#define ZVT_LOG(Format, ...)
#define ZVT_LOG_WARNING(Format, ...)
#define ZVT_LOG_ERROR(Format, ...)
#define ZVT_LOG_IF(CVar, Format, ...)

#define ZVT_LOG_FUNCTIONS(CategoryName)
#define ZVT_LOG_FUNCTIONS_NOT_UOBJECT(CategoryName)

#elif WITH_EDITOR

#define ZVT_LOG(Format, ...) {LogInfo(Format, __FILE__, __LINE__, ##__VA_ARGS__);}
#define ZVT_LOG_WARNING(Format, ...) {LogWarning(Format, __FILE__, __LINE__, ##__VA_ARGS__);}
#define ZVT_LOG_ERROR(Format, ...) {LogError(Format, __FILE__, __LINE__, ##__VA_ARGS__);}
#define ZVT_LOG_IF(CVar, Format, ...) if ((CVar).GetValueOnAnyThread()) { ZVT_LOG(Format, ##__VA_ARGS__) }

#define ZVT_LOG_FUNCTIONS(CategoryName) \
    template <typename... ArgTypes> \
    void LogWarning(UE::Core::TCheckedFormatString<FString::FmtCharType, ArgTypes...> Format, const FString& File, int Line, ArgTypes... Args) const \
	{ \
		const FString FormattedString = FString::Printf(Format, Args...); \
		UE_LOG( \
		CategoryName, Warning, TEXT("[%s:%i][%s] %s"), \
		*FPaths::GetCleanFilename(File), Line, \
		*GetName(), *FormattedString); \
	} \
	\
	template <typename... ArgTypes> \
    void LogError(UE::Core::TCheckedFormatString<FString::FmtCharType, ArgTypes...> Format, const FString& File, int Line, ArgTypes... Args) const \
	{ \
		const FString FormattedString = FString::Printf(Format, Args...); \
		\
		const FString FinalString = FString::Printf(TEXT("[%s:%i][%s] %s"), \
		*FPaths::GetCleanFilename(File), \
		Line, *GetName(), *FormattedString); \
		\
		UE_LOG(CategoryName, Error, TEXT("%s"), *FinalString); \
		\
		FNotificationInfo Ni(FText::FromString(FinalString)); \
		Ni.Image = FCoreStyle::Get().GetBrush("Icons.ErrorWithColor"); \
		Ni.ExpireDuration = 5.f; \
		Ni.WidthOverride = FOptionalSize(1000.f); \
		FSlateNotificationManager::Get().AddNotification(Ni); \
	} \
	\
	template <typename... ArgTypes> \
	void LogInfo(UE::Core::TCheckedFormatString<FString::FmtCharType, ArgTypes...> Format, const FString& File, int Line, ArgTypes... Args) const \
	{ \
		const FString FormattedString = FString::Printf(Format, Args...); \
		UE_LOG( \
		CategoryName, Log, TEXT("[%s:%i][%s] %s"), \
		*FPaths::GetCleanFilename(File), Line, \
		*GetName(), *FormattedString); \
	}

#define ZVT_LOG_FUNCTIONS_NOT_UOBJECT(CategoryName) \
	template <typename... ArgTypes> \
	static void LogWarning(UE::Core::TCheckedFormatString<FString::FmtCharType, ArgTypes...> Format, const FString& File, int Line, ArgTypes... Args) \
	{ \
		const FString FormattedString = FString::Printf(Format, Args...); \
		UE_LOG( \
		CategoryName, Warning, TEXT("[%s:%i] %s"), \
		*FPaths::GetCleanFilename(File), Line, \
		*FormattedString); \
	} \
	\
    template <typename... ArgTypes> \
    static void LogError(UE::Core::TCheckedFormatString<FString::FmtCharType, ArgTypes...> Format, const FString& File, int Line, ArgTypes... Args) \
	{ \
		const FString FormattedString = FString::Printf(Format, Args...); \
		\
		const FString FinalString = FString::Printf(TEXT("[%s:%i] %s"), \
		*FPaths::GetCleanFilename(File), \
		Line, *FormattedString); \
		\
		UE_LOG(CategoryName, Error, TEXT("%s"), *FinalString); \
		\
		FNotificationInfo Ni(FText::FromString(FinalString)); \
		Ni.Image = FCoreStyle::Get().GetBrush("Icons.ErrorWithColor"); \
		Ni.ExpireDuration = 5.f; \
		Ni.WidthOverride = FOptionalSize(1000.f); \
		FSlateNotificationManager::Get().AddNotification(Ni); \
	} \
	\
    template <typename... ArgTypes> \
    static void LogInfo(UE::Core::TCheckedFormatString<FString::FmtCharType, ArgTypes...> Format, const FString& File, int Line, ArgTypes... Args) \
	{ \
		const FString FormattedString = FString::Printf(Format, Args...); \
		UE_LOG( \
		CategoryName, Log, TEXT("[%s:%i] %s"), \
		*FPaths::GetCleanFilename(File), Line, \
		*FormattedString); \
	}

#else

#define ZVT_LOG(Format, ...) LogInfo(Format, __FILE__, __LINE__, ##__VA_ARGS__);
#define ZVT_LOG_WARNING(Format, ...) LogWarning(Format, __FILE__, __LINE__, ##__VA_ARGS__);
#define ZVT_LOG_ERROR(Format, ...) LogError(Format, __FILE__, __LINE__, ##__VA_ARGS__);
#define ZVT_LOG_IF(CVar, Format, ...) if ((CVar).GetValueOnAnyThread()) { ZVT_LOG(Format, ##__VA_ARGS__) }

#define ZVT_LOG_FUNCTIONS(CategoryName) \
    template <typename... ArgTypes> \
    void LogWarning(UE::Core::TCheckedFormatString<FString::FmtCharType, ArgTypes...> Format, const FString& File, int Line, ArgTypes... Args) const \
	{ \
		const FString FormattedString = FString::Printf(Format, Args...); \
		UE_LOG( \
		CategoryName, Warning, TEXT("[%s:%i][%s] %s"), \
		*FPaths::GetCleanFilename(File), Line, \
		*GetName(), *FormattedString); \
	} \
	\
    template <typename... ArgTypes> \
    void LogError(UE::Core::TCheckedFormatString<FString::FmtCharType, ArgTypes...> Format, const FString& File, int Line, ArgTypes... Args) const \
	{ \
		const FString FormattedString = FString::Printf(Format, Args...); \
		UE_LOG( \
		CategoryName, Error, TEXT("[%s:%i][%s] %s"), \
		*FPaths::GetCleanFilename(File), Line, \
		*GetName(), *FormattedString); \
	} \
	\
    template <typename... ArgTypes> \
    void LogInfo(UE::Core::TCheckedFormatString<FString::FmtCharType, ArgTypes...> Format, const FString& File, int Line, ArgTypes... Args) const \
	{ \
		const FString FormattedString = FString::Printf(Format, Args...); \
		UE_LOG( \
		CategoryName, Log, TEXT("[%s:%i][%s] %s"), \
		*FPaths::GetCleanFilename(File), Line, \
		*GetName(), *FormattedString); \
	}

#define ZVT_LOG_FUNCTIONS_NOT_UOBJECT(CategoryName) \
    template <typename... ArgTypes> \
    static void LogWarning(UE::Core::TCheckedFormatString<FString::FmtCharType, ArgTypes...> Format, const FString& File, int Line, ArgTypes... Args) \
	{ \
		const FString FormattedString = FString::Printf(Format, Args...); \
		UE_LOG( \
		CategoryName, Warning, TEXT("[%s:%i] %s"), \
		*FPaths::GetCleanFilename(File), Line, \
		*FormattedString); \
	} \
	\
    template <typename... ArgTypes> \
    static void LogError(UE::Core::TCheckedFormatString<FString::FmtCharType, ArgTypes...> Format, const FString& File, int Line, ArgTypes... Args) \
	{ \
		const FString FormattedString = FString::Printf(Format, Args...); \
		UE_LOG( \
		CategoryName, Error, TEXT("[%s:%i] %s"), \
		*FPaths::GetCleanFilename(File), Line, \
		*FormattedString); \
	} \
	\
    template <typename... ArgTypes> \
    static void LogInfo(UE::Core::TCheckedFormatString<FString::FmtCharType, ArgTypes...> Format, const FString& File, int Line, ArgTypes... Args) \
	{ \
		const FString FormattedString = FString::Printf(Format, Args...); \
		UE_LOG( \
		CategoryName, Log, TEXT("[%s:%i] %s"), \
		*FPaths::GetCleanFilename(File), Line, \
		*FormattedString); \
	}

#endif
