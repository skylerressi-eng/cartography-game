// Copyright (c) 2026 The Last Cartographer.

#include "Player/InteractionComponent.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/Actor.h"

FText IInteractable::GetPrompt_Implementation() const { return NSLOCTEXT("Cartography", "DefaultPrompt", "Interact"); }
void IInteractable::Interact_Implementation(AActor*) {}

UInteractionComponent::UInteractionComponent() { PrimaryComponentTick.bCanEverTick = true; }

void UInteractionComponent::TickComponent(float, ELevelTick, FActorComponentTickFunction*)
{
	AActor* Owner = GetOwner();
	if (!Owner) { return; }

	UCameraComponent* Cam = Owner->FindComponentByClass<UCameraComponent>();
	const FVector Start = Cam ? Cam->GetComponentLocation() : Owner->GetActorLocation();
	const FVector Fwd   = Cam ? Cam->GetForwardVector()    : Owner->GetActorForwardVector();
	const FVector End   = Start + Fwd * Reach;

	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(Interact), false, Owner);
	const bool bHit = GetWorld()->SweepSingleByChannel(Hit, Start, End, FQuat::Identity, ECC_Visibility,
	                                                   FCollisionShape::MakeSphere(Radius), Params);
	AActor* NewFocus = nullptr;
	FText NewPrompt;
	if (bHit && Hit.GetActor() && Hit.GetActor()->Implements<UInteractable>())
	{
		NewFocus = Hit.GetActor();
		NewPrompt = IInteractable::Execute_GetPrompt(NewFocus);
	}

	if (NewFocus != HoveredActor.Get() || !NewPrompt.EqualTo(CurrentPrompt))
	{
		HoveredActor = NewFocus;
		CurrentPrompt = NewPrompt;
		OnFocusChanged.Broadcast(NewFocus, NewPrompt);
	}
}

bool UInteractionComponent::TryInteract()
{
	AActor* Focus = HoveredActor.Get();
	if (!Focus || !Focus->Implements<UInteractable>()) { return false; }
	IInteractable::Execute_Interact(Focus, GetOwner());
	return true;
}
