// Copyright (c) 2026 The Last Cartographer.

#include "Audio/AudioMoodComponent.h"

#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"

UAudioMoodComponent::UAudioMoodComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAudioMoodComponent::BeginPlay()
{
	Super::BeginPlay();
	for (FMoodLayer& Layer : Layers)
	{
		if (!Layer.Sound) { continue; }
		Layer.Component = NewObject<UAudioComponent>(GetOwner());
		Layer.Component->RegisterComponent();
		Layer.Component->SetSound(Layer.Sound);
		Layer.Component->SetVolumeMultiplier(Layer.CurrentVolume);
		Layer.Component->SetUISound(true);
		Layer.Component->Play();
	}
}

void UAudioMoodComponent::SetLayerTarget(FName Id, float Volume)
{
	for (FMoodLayer& L : Layers)
	{
		if (L.Id == Id) { L.TargetVolume = FMath::Clamp(Volume, 0.f, 1.f); return; }
	}
}

void UAudioMoodComponent::StopAll()
{
	for (FMoodLayer& L : Layers) { L.TargetVolume = 0.f; }
}

void UAudioMoodComponent::TickComponent(float DeltaTime, ELevelTick, FActorComponentTickFunction*)
{
	for (FMoodLayer& L : Layers)
	{
		if (!L.Component) { continue; }
		L.CurrentVolume = FMath::FInterpTo(L.CurrentVolume, L.TargetVolume, DeltaTime, FadeRate);
		L.Component->SetVolumeMultiplier(L.CurrentVolume);
	}
}
