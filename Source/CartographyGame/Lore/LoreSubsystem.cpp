// Copyright (c) 2026 The Last Cartographer.

#include "Lore/LoreSubsystem.h"

#include "Engine/DataTable.h"

void ULoreSubsystem::MarkFound(FName LoreId)
{
	if (LoreId.IsNone() || Found.Contains(LoreId)) { return; }
	Found.Add(LoreId);
	OnLoreFound.Broadcast(LoreId);
}

FLoreRow ULoreSubsystem::GetEntry(FName LoreId) const
{
	if (!LoreTable) { return FLoreRow(); }
	for (const auto& Pair : LoreTable->GetRowMap())
	{
		const FLoreRow* R = (FLoreRow*)Pair.Value;
		if (R && R->LoreId == LoreId) { return *R; }
	}
	return FLoreRow();
}

TArray<FLoreRow> ULoreSubsystem::GetAllFoundOrdered() const
{
	TArray<FLoreRow> Out;
	if (!LoreTable) { return Out; }
	for (const auto& Pair : LoreTable->GetRowMap())
	{
		const FLoreRow* R = (FLoreRow*)Pair.Value;
		if (R && Found.Contains(R->LoreId)) { Out.Add(*R); }
	}
	Out.Sort([](const FLoreRow& A, const FLoreRow& B){ return A.OrderHint < B.OrderHint; });
	return Out;
}

void ULoreSubsystem::LoadFound(const TArray<FName>& Ids)
{
	Found.Reset();
	for (const FName& Id : Ids) { Found.Add(Id); }
}
