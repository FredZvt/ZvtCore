#include "ZvtStringTableBuilder.h"

void FZvtStringTableBuilder::AddColumns(const TArray<FName>& InColumns)
{
	for (const FName& Col : InColumns)
	{
		if (Columns.Contains(Col)) continue;
		Columns.Add(Col);
	}
	bColumnWidthsDirty = true;
}

void FZvtStringTableBuilder::AddRow(int32 RowId)
{
	if (Rows.Contains(RowId)) return;
	RowOrder.Add(RowId);
	Rows.Add(RowId, TMap<FName, FString>());
}

bool FZvtStringTableBuilder::SetRowCellValue(int32 RowId, FName Column, const FString& Value)
{
	ZVT_RETURN_FALSE_IF_FALSE(Rows.Contains(RowId))
	ZVT_RETURN_FALSE_IF_FALSE(Columns.Contains(Column))
	Rows[RowId].Add(Column, Value);
	bColumnWidthsDirty = true;
	return true;
}

void FZvtStringTableBuilder::RecalculateColumnWidths() const
{
	ColumnWidths.Empty();
	for (const FName& Col : Columns)
	{
		int32 Width = Col.ToString().Len();
		for (const TPair<int32, TMap<FName, FString>>& RowPair : Rows)
		{
			if (const FString* Cell = RowPair.Value.Find(Col))
			{
				Width = FMath::Max(Width, Cell->Len());
			}
		}
		ColumnWidths.Add(Col, Width);
	}
	bColumnWidthsDirty = false;
}

FString FZvtStringTableBuilder::GetString() const
{
	if (bColumnWidthsDirty) RecalculateColumnWidths();

	FString Result;

	for (int32 i = 0; i < Columns.Num(); i++)
	{
		if (i > 0) Result += Separator;
		const FName& Col = Columns[i];
		const FString ColStr = Col.ToString();
		const int32* WidthPtr = ColumnWidths.Find(Col);
		const int32 Width = WidthPtr ? *WidthPtr : ColStr.Len();
		Result += ColStr + FString::ChrN(FMath::Max(0, Width - ColStr.Len()), ' ');
	}

	for (const int32 RowId : RowOrder)
	{
		Result += LINE_TERMINATOR;
		const TMap<FName, FString>* RowDataPtr = Rows.Find(RowId);
		for (int32 i = 0; i < Columns.Num(); i++)
		{
			if (i > 0) Result += Separator;
			const FName& Col = Columns[i];
			const int32* WidthPtr = ColumnWidths.Find(Col);
			const int32 Width = WidthPtr ? *WidthPtr : 0;
			const FString CellValue = (RowDataPtr && RowDataPtr->Contains(Col)) ? *RowDataPtr->Find(Col) : TEXT("");
			Result += CellValue + FString::ChrN(FMath::Max(0, Width - CellValue.Len()), ' ');
		}
	}

	return Result;
}

void FZvtStringTableBuilder::Clear()
{
	Columns.Empty();
	RowOrder.Empty();
	Rows.Empty();
	ColumnWidths.Empty();
	bColumnWidthsDirty = true;
}

bool FZvtStringTableBuilder::IsEmpty() const
{
	return Rows.Num() == 0;
}
