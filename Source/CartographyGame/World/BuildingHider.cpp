// Copyright (c) 2026 The Last Cartographer.

#include "World/BuildingHider.h"

#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"

UBuildingHiderComponent::UBuildingHiderComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBuildingHiderComponent::BeginPlay()
{
	Super::BeginPlay();
	if (bApplyOnBeginPlay) { ApplyToOwner(); }
}

bool UBuildingHiderComponent::MatchesPrefix(const FString& Name) const
{
	for (const FString& Keep : KeepExceptions)
	{
		if (!Keep.IsEmpty() && Name.Equals(Keep, ESearchCase::IgnoreCase)) { return false; }
	}
	for (const FString& P : HidePrefixes)
	{
		if (!P.IsEmpty() && Name.StartsWith(P, ESearchCase::IgnoreCase)) { return true; }
	}
	return false;
}

int32 UBuildingHiderComponent::ApplyToOwner() { return ApplyToActor(GetOwner()); }

int32 UBuildingHiderComponent::ApplyToActor(AActor* Target)
{
	if (!Target) { return 0; }
	int32 Count = 0;
	TArray<UActorComponent*> Components;
	Target->GetComponents(Components);
	for (UActorComponent* C : Components)
	{
		UPrimitiveComponent* P = Cast<UPrimitiveComponent>(C);
		if (!P) { continue; }
		const FString Name = P->GetName();
		if (!MatchesPrefix(Name)) { continue; }
		const bool bShouldShow = bReveal;
		P->SetVisibility(bShouldShow, true);
		P->SetHiddenInGame(!bShouldShow, true);
		P->SetCollisionEnabled(bShouldShow ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
		++Count;
	}

	// Also walk attached child actors (glTF imports often spawn them).
	TArray<AActor*> Children;
	Target->GetAttachedActors(Children, /*bResetArray*/ true, /*bRecursivelyIncludeAttachedActors*/ true);
	for (AActor* Child : Children)
	{
		if (!Child) { continue; }
		const FString Name = Child->GetName();
		if (!MatchesPrefix(Name)) { continue; }
		const bool bShouldShow = bReveal;
		Child->SetActorHiddenInGame(!bShouldShow);
		Child->SetActorEnableCollision(bShouldShow);
		++Count;
	}

	return Count;
}

void UBuildingHiderComponent::RevealAll()
{
	const bool Was = bReveal;
	bReveal = true;
	ApplyToOwner();
	bReveal = Was;
}
