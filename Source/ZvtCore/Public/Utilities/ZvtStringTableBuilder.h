#pragma once

#include "CoreMinimal.h"
#include "ZvtCoreLogCategories.h"
#include "ZvtMacros.h"

/**
 * Builds a plain-text table with aligned, pipe-separated columns, suitable for
 * printing structured data to the output log or any string sink.
 *
 * Columns and rows are independent: declare all columns first, then add rows
 * and fill their cells. Column widths are recalculated lazily on GetString().
 * Rows are emitted in the order they were added via AddRow().
 *
 * Cells that are never set are rendered as empty strings. SetRowCellValue()
 * returns false (and logs an error) if the row or column does not exist, so
 * always call AddRow() and AddColumns() before setting values.
 *
 * Example output:
 * @code
 *   Action     | Key   | State
 *   Jump       | Space | Pressed
 *   Fire       | LMB   | Released
 * @endcode
 *
 * Typical usage:
 * @code
 *   FZvtStringTableBuilder Table;
 *   Table.AddColumns({ TEXT("Action"), TEXT("Key"), TEXT("State") });
 *   Table.AddRow(0);
 *   Table.SetRowCellValue(0, TEXT("Action"), TEXT("Jump"));
 *   Table.SetRowCellValue(0, TEXT("Key"),    TEXT("Space"));
 *   Table.SetRowCellValue(0, TEXT("State"),  TEXT("Pressed"));
 *   UE_LOG(LogTemp, Log, TEXT("%s"), *Table.GetString());
 * @endcode
 */
struct ZVTCORE_API FZvtStringTableBuilder
{
	ZVT_LOG_FUNCTIONS_NOT_UOBJECT(LogZvtCore)

	/**
	 * Declares the column layout. Call once before adding rows or setting cell values.
	 * Calling a second time appends more columns rather than replacing the existing ones.
	 *
	 * @param InColumns  Ordered list of column names. These same names are used as keys in SetRowCellValue.
	 */
	void AddColumns(const TArray<FName>& InColumns);

	/**
	 * Registers a new row. Rows are emitted in the order they are added.
	 * Adding a duplicate RowId is a no-op; the original row is preserved.
	 *
	 * @param RowId  Arbitrary integer that uniquely identifies this row. Used only as a key
	 *               for subsequent SetRowCellValue calls and to define insertion order.
	 */
	void AddRow(int32 RowId);

	/**
	 * Sets the content of a single cell.
	 *
	 * @param RowId   ID of an existing row (must have been registered with AddRow).
	 * @param Column  Name of the target column (must have been declared with AddColumns).
	 * @param Value   String to display in the cell.
	 * @return        true on success. false if RowId or Column does not exist; an error is also logged.
	 */
	bool SetRowCellValue(int32 RowId, FName Column, const FString& Value);

	/**
	 * Builds and returns the formatted table as a multi-line string.
	 *
	 * The header row is emitted first, followed by data rows in insertion order. Each column
	 * is padded to the width of its widest cell (including the header). Columns are separated
	 * by " | ". Cells that were never set are rendered as empty strings.
	 *
	 * @return  The formatted table string, or an empty string if no rows have been added.
	 */
	FString GetString() const;

	/**
	 * Removes all rows and columns and resets column-width state.
	 * After calling Clear() the builder is ready to be configured from scratch.
	 */
	void Clear();

	/**
	 * Returns true if no rows have been added since construction or the last Clear() call.
	 */
	bool IsEmpty() const;

private:

	void RecalculateColumnWidths() const;

	static constexpr const TCHAR* Separator = TEXT(" | ");
	TArray<FName> Columns;
	TArray<int32> RowOrder;
	TMap<int32, TMap<FName, FString>> Rows;
	mutable TMap<FName, int32> ColumnWidths;
	mutable bool bColumnWidthsDirty = true;
};
