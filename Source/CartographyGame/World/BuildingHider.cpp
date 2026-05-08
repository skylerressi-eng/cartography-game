// Copyright (c) 2026 The Last Cartographer.

#include "World/BuildingHider.h"

#include "Components/PrimitiveComponent.h"
#include "EngineUtils.h"
#include "GameFramework/Actor.h"

UBuildingHiderComponent::UBuildingHiderComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBuildingHiderComponent::BeginPlay()
{
	Super::BeginPlay();
	if (bApplyOnBeginPlay) { Apply(); }
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

int32 UBuildingHiderComponent::ApplyToActor(AActor* InTarget)
{
	if (!InTarget) { return 0; }
	int32 Count = 0;
	TArray<UActorComponent*> Components;
	InTarget->GetComponents(Components);
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
	InTarget->GetAttachedActors(Children, /*bResetArray*/ true, /*bRecursivelyIncludeAttachedActors*/ true);
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

int32 UBuildingHiderComponent::ApplyToWorld()
{
	UWorld* World = GetWorld();
	if (!World) { return 0; }
	int32 Count = 0;
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* A = *It;
		if (!A) { continue; }
		Count += ApplyToActor(A);
	}
	return Count;
}

int32 UBuildingHiderComponent::Apply()
{
	if (Target) { return ApplyToActor(Target); }
	if (AActor* Owner = GetOwner())
	{
		TArray<AActor*> Attached;
		Owner->GetAttachedActors(Attached, true, true);
		if (Attached.Num() > 0) { return ApplyToActor(Owner); }
	}
	return ApplyToWorld();
}

void UBuildingHiderComponent::RevealAll()
{
	const bool Was = bReveal;
	bReveal = true;
	ApplyToOwner();
	bReveal = Was;
}
