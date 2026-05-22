# ZvtCore

Shared support plugin for Zvt projects. Provides logging macros, defensive return macros, a debug overlay subsystem, enum utilities, and a string table builder.

---

## Logging

Include `ZvtMacros.h`. Place one of the injection macros in the class body (no semicolon), then use the `ZVT_LOG*` call macros anywhere in that class.

**Injection macros**

| Macro                                         | Use for                          |
|-----------------------------------------------|----------------------------------|
| `ZVT_LOG_FUNCTIONS(CategoryName)`             | `UObject`-derived classes        |
| `ZVT_LOG_FUNCTIONS_NOT_UOBJECT(CategoryName)` | plain structs / non-UObject types |

**Call macros** (require one of the above to be injected)

| Macro                          | Level   | Notes                                        |
|--------------------------------|---------|----------------------------------------------|
| `ZVT_LOG(Fmt, ...)`            | Log     |                                              |
| `ZVT_LOG_WARNING(Fmt, ...)`    | Warning |                                              |
| `ZVT_LOG_ERROR(Fmt, ...)`      | Error   | Also shows a Slate toast in `WITH_EDITOR`    |
| `ZVT_LOG_IF(CVar, Fmt, ...)`   | Log     | Only logs when `CVar.GetValueOnAnyThread()` is true |

Log format: `[FileName.cpp:Line][ObjectName] Message`

All macros expand to nothing in `NO_LOGGING` builds.

**Example**

```cpp
// MyActor.h
UCLASS()
class AMyActor : public AActor
{
    GENERATED_BODY()
    ZVT_LOG_FUNCTIONS(LogMyGame)
};

// MyActor.cpp
void AMyActor::Foo()
{
    ZVT_LOG(TEXT("Foo called"));
    ZVT_LOG_ERROR(TEXT("Value is %d"), SomeValue);
}
```

---

## Defensive Return Macros

All macros log an error via `ZVT_LOG_ERROR` and exit early. Requires `ZVT_LOG_FUNCTIONS` or `ZVT_LOG_FUNCTIONS_NOT_UOBJECT` to be injected in the calling class.

**Null pointer checks**

| Macro                               | Returns          |
|-------------------------------------|------------------|
| `ZVT_RETURN_IF_NULL(Ptr)`           | `void`           |
| `ZVT_RETURN_VALUE_IF_NULL(Ptr, V)`  | `V`              |
| `ZVT_RETURN_NULL_IF_NULL(Ptr)`      | `nullptr`        |
| `ZVT_RETURN_FALSE_IF_NULL(Ptr)`     | `false`          |
| `ZVT_RETURN_TRUE_IF_NULL(Ptr)`      | `true`           |
| `ZVT_RETURN_ZERO_IF_NULL(Ptr)`      | `0`              |
| `ZVT_RETURN_ZEROVECTOR_IF_NULL(Ptr)`| `FVector::ZeroVector` |
| `ZVT_RETURN_NAME_NONE_IF_NULL(Ptr)` | `NAME_None`      |
| `ZVT_RETURN_IF_NULL_SOFTPTR(Ptr)`   | `void` (uses `IsNull()`) |
| `ZVT_CONTINUE_IF_NULL(Ptr)`         | `continue` (loop) |

**Weak pointer validity checks** (`TWeakObjectPtr` / `TWeakPtr`)

| Macro                                    | Returns           |
|------------------------------------------|-------------------|
| `ZVT_RETURN_IF_NOT_VALID(Ptr)`           | `void`            |
| `ZVT_RETURN_FALSE_IF_NOT_VALID(Ptr)`     | `false`           |
| `ZVT_RETURN_VALUE_IF_NOT_VALID(Ptr, V)`  | `V`               |
| `ZVT_CONTINUE_IF_NOT_VALID(Ptr)`         | `continue` (loop) |

**Boolean checks**

| Macro                                  | Returns           |
|----------------------------------------|-------------------|
| `ZVT_RETURN_IF_FALSE(B)`               | `void`            |
| `ZVT_RETURN_FALSE_IF_FALSE(B)`         | `false`           |
| `ZVT_RETURN_NULL_IF_FALSE(B)`          | `nullptr`         |
| `ZVT_RETURN_VALUE_IF_FALSE(B, V)`      | `V`               |
| `ZVT_RETURN_IF_TRUE(B)`                | `void`            |
| `ZVT_CONTINUE_IF_FALSE(B)`             | `continue` (loop) |
| `ZVT_CONTINUE_IF_TRUE(B)`              | `continue` (loop) |

**Other**

| Macro                  | Returns |
|------------------------|---------|
| `ZVT_RETURN_IF_ZERO(V)`| `void`  |

---

## Debug Overlay - `UZvtDebugManager`

An `UEngineSubsystem` that renders text on the game viewport using the **RobotoMono** font. It is an alternative to `GEngine->AddOnScreenDebugMessage` with keyed deduplication and configurable scale.

```cpp
// Post a per-tick overlay (0.0f duration = one frame)
UZvtDebugManager::AddOnScreenMessage(0, 0.0f, FString::Printf(TEXT("Speed: %.1f"), Speed));

// Post a persistent message keyed to slot 1
UZvtDebugManager::AddOnScreenMessage(1, 5.0f, TEXT("Match started"), FColor::Yellow);

// Enable a semi-transparent background behind all messages
UZvtDebugManager::SetOnScreenMessagesBackground(true, FColor(0, 0, 0, 160));
```

- `Key == INDEX_NONE` always appends a new line.
- Any other key updates the existing entry with that key in place.
- Pass `Duration = 0.0f` for per-tick overlays that are re-posted every frame.

---

## Enum Helpers - `FZvtEnumHelpers`

Static template utilities for `UENUM`-decorated enums. Include `ZvtEnumHelpers.h`.

| Method                                          | Description                                           |
|-------------------------------------------------|-------------------------------------------------------|
| `GetEnumString<E>(Value)`                       | Short name as `FString` (prefix stripped)             |
| `GetEnumName<E>(Value)`                         | Short name as `FName`                                 |
| `GetEnumFullName<E>(Value)`                     | Fully-qualified name (e.g. `EMyEnum::ValueA`)         |
| `GetEnumStrings<E>(OutStrings)`                 | All short name strings, excluding `_MAX`              |
| `GetEnumNames<E>(OutNames)`                     | All short names as `FName`, excluding `_MAX`          |
| `GetEnumFullNames<E>(OutNames)`                 | All fully-qualified names, excluding `_MAX`           |
| `GetAllEnumValues<E>()`                         | `TArray<E>` of all values in declaration order        |
| `GetValueByName<E>(Name, OutValue)`             | Reverse lookup by short name (`FName`)                |
| `GetValueByString<E>(String, OutValue)`         | Reverse lookup by short name string                   |
| `ToUnderlyingValue<E>(Value)`                   | `constexpr` cast to underlying integer type           |
| `CycleToNextEnumValue<E>(First, Last, Current)` | Next value, wrapping from Last back to First          |
| `GetNextEnumValue<E>(Last, Current)`            | Next value, clamped at Last                           |
| `GetPreviousEnumValue<E>(First, Current)`       | Previous value, clamped at First                      |
| `GetDisplayTextFor<E>(Value, StringTableId)`    | Localized `FText` from a string table keyed by short name |

---

## String Table Builder - `FZvtStringTableBuilder`

Builds a pipe-separated, column-aligned plain-text table for log output. Include `ZvtStringTableBuilder.h`.

```cpp
FZvtStringTableBuilder Table;
Table.AddColumns({ TEXT("Action"), TEXT("Key"), TEXT("State") });
Table.AddRow(0);
Table.SetRowCellValue(0, TEXT("Action"), TEXT("Jump"));
Table.SetRowCellValue(0, TEXT("Key"),    TEXT("Space"));
Table.SetRowCellValue(0, TEXT("State"),  TEXT("Pressed"));
UE_LOG(LogTemp, Log, TEXT("%s"), *Table.GetString());
```

Output:
```
Action | Key   | State
Jump   | Space | Pressed
```

---

## File Path Utility - `FZvtUtils`

Include `ZvtUtils.h`.

| Method                              | Description                                                    |
|-------------------------------------|----------------------------------------------------------------|
| `ExtractRelativeFilePath(AbsPath)`  | Converts an absolute path inside `Content/` to a content-relative path (starts with `/`) |
