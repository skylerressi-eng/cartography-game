// Copyright (c) 2026 The Last Cartographer.

#include "UI/CompassWidget.h"

#include "Components/Image.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"

void UCompassWidget::NativeTick(const FGeometry& Geo, float Dt)
{
	Super::NativeTick(Geo, Dt);
	APlayerCameraManager* Cam = UGameplayStatics::GetPlayerCameraManager(this, 0);
	if (!Cam) { return; }
	const float Yaw = Cam->GetCameraRotation().Yaw;
	if (Arrow_Image)
	{
		Arrow_Image->SetRenderTransformAngle(-Yaw);
	}
	OnHeadingChanged(Yaw);
}
