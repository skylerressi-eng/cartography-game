// Copyright (c) 2026 The Last Cartographer.

#include "Map/MapBookmark.h"

int32 UMapBookmarkComponent::Add(FName Id, FText Name, FVector2D MapUV, float Zoom)
{
	for (FMapBookmark& B : Bookmarks)
	{
		if (B.Id == Id) { B.Name = Name; B.MapUV = MapUV; B.Zoom = Zoom; OnBookmarksChanged.Broadcast(); return INDEX_NONE; }
	}
	FMapBookmark B; B.Id = Id; B.Name = Name; B.MapUV = MapUV; B.Zoom = Zoom;
	const int32 Idx = Bookmarks.Add(B);
	OnBookmarksChanged.Broadcast();
	return Idx;
}

void UMapBookmarkComponent::Remove(FName Id)
{
	const int32 Removed = Bookmarks.RemoveAll([&](const FMapBookmark& B){ return B.Id == Id; });
	if (Removed > 0) { OnBookmarksChanged.Broadcast(); }
}

FMapBookmark UMapBookmarkComponent::Get(FName Id) const
{
	for (const FMapBookmark& B : Bookmarks) { if (B.Id == Id) { return B; } }
	return FMapBookmark();
}
