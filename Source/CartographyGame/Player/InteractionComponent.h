// Copyright (c) 2026 The Last Cartographer.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

UINTERFACE(BlueprintType)
class CARTOGRAPHYGAME_API UInteractable : public UInterface { GENERATED_BODY() };

class CARTOGRAPHYGAME_API IInteractable
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, Category="Interact") FText GetPrompt() const;
	UFUNCTION(BlueprintNativeEvent, Category="Interact") void Interact(AActor* Instigator);
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFocusChanged, AActor*, Focus, FText, Prompt);

/**
 * Hover/focus + interact dispatcher. Sphere-traces in front of the camera
 * each tick, broadcasts the hovered actor + prompt text, and forwards
 * interact presses to whatever IInteractable it found.
 */
UCLASS(ClassGroup=(Cartography), meta=(BlueprintSpawnableComponent))
class CARTOGRAPHYGAME_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractionComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interact") float Reach = 280.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interact") float Radius = 24.f;

	UPROPERTY(BlueprintReadOnly, Category="Interact") TWeakObjectPtr<AActor> HoveredActor;
	UPROPERTY(BlueprintReadOnly, Category="Interact") FText CurrentPrompt;

	UPROPERTY(BlueprintAssignable, Category="Interact") FOnFocusChanged OnFocusChanged;

	UFUNCTION(BlueprintCallable, Category="Interact") bool TryInteract();

	virtual void TickComponent(float Dt, ELevelTick, FActorComponentTickFunction*) override;
};
