// Copyright (c) 2026 The Last Cartographer.

#include "Achievements/AchievementSubsystem.h"

#include "Engine/DataTable.h"

bool UAchievementSubsystem::Unlock(FName AchievementId)
{
	if (AchievementId.IsNone() || Unlocked.Contains(AchievementId)) { return false; }
	Unlocked.Add(AchievementId);
	OnUnlocked.Broadcast(GetRow(AchievementId));
	return true;
}

FAchievementRow UAchievementSubsystem::GetRow(FName Id) const
{
	if (!AchievementsTable) { return FAchievementRow(); }
	for (const auto& Pair : AchievementsTable->GetRowMap())
	{
		const FAchievementRow* R = (FAchievementRow*)Pair.Value;
		if (R && R->AchievementId == Id) { return *R; }
	}
	return FAchievementRow();
}

void UAchievementSubsystem::Load(const TArray<FName>& Ids)
{
	Unlocked.Reset();
	for (const FName& Id : Ids) { Unlocked.Add(Id); }
}
