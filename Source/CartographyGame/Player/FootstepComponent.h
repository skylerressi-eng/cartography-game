// Copyright (c) 2026 The Last Cartographer.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FootstepComponent.generated.h"

class USoundBase;

USTRUCT(BlueprintType)
struct FFootstepSet
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName SurfaceTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<TObjectPtr<USoundBase>> Sounds;
};

UCLASS(ClassGroup=(Cartography), meta=(BlueprintSpawnableComponent))
class CARTOGRAPHYGAME_API UFootstepComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFootstepComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Footsteps") TArray<FFootstepSet> Sets;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Footsteps") float StridePerCm = 1.f / 90.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Footsteps") float Volume = 0.5f;

	virtual void TickComponent(float Dt, ELevelTick, FActorComponentTickFunction*) override;

private:
	float Distance = 0.f;
	FVector LastLocation = FVector::ZeroVector;
	bool bHasLast = false;

	FName ResolveSurfaceTag() const;
	void Step();
};
