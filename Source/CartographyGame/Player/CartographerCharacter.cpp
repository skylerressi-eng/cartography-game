// Copyright (c) 2026 The Last Cartographer.

#include "Player/CartographerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Core/CartographyPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Inking/InkingDesk.h"
#include "InputActionValue.h"
#include "Map/FieldMapComponent.h"
#include "Notebook/FieldNotebookComponent.h"
#include "Player/InteractionComponent.h"

ACartographerCharacter::ACartographerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 360.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->BrakingDecelerationWalking = 1500.f;
	GetCharacterMovement()->JumpZVelocity = 320.f;
	GetCharacterMovement()->AirControl = 0.15f;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 380.f;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->SocketOffset = FVector(0.f, 40.f, 60.f);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;
	Camera->FieldOfView = 70.f;

	FieldMap = CreateDefaultSubobject<UFieldMapComponent>(TEXT("FieldMap"));
	Notebook = CreateDefaultSubobject<UFieldNotebookComponent>(TEXT("Notebook"));
}

void ACartographerCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
				ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}
}

void ACartographerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveAction)            EIC->BindAction(MoveAction,            ETriggerEvent::Triggered, this, &ACartographerCharacter::Move);
		if (LookAction)            EIC->BindAction(LookAction,            ETriggerEvent::Triggered, this, &ACartographerCharacter::Look);
		if (JumpAction)            EIC->BindAction(JumpAction,            ETriggerEvent::Started,   this, &ACharacter::Jump);
		if (JumpAction)            EIC->BindAction(JumpAction,            ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		if (ToggleFieldMapAction)  EIC->BindAction(ToggleFieldMapAction,  ETriggerEvent::Started,   this, &ACartographerCharacter::HandleToggleFieldMap);
		if (ToggleNotebookAction)  EIC->BindAction(ToggleNotebookAction,  ETriggerEvent::Started,   this, &ACartographerCharacter::HandleToggleNotebook);
		if (InteractAction)        EIC->BindAction(InteractAction,        ETriggerEvent::Started,   this, &ACartographerCharacter::HandleInteract);
		if (DrawAction)            EIC->BindAction(DrawAction,            ETriggerEvent::Started,   this, &ACartographerCharacter::HandleDrawStarted);
		if (DrawAction)            EIC->BindAction(DrawAction,            ETriggerEvent::Completed, this, &ACartographerCharacter::HandleDrawStopped);
	}
}

void ACartographerCharacter::Move(const FInputActionValue& Value)
{
	if (bFieldMapOpen) { return; }

	const FVector2D Axis = Value.Get<FVector2D>();
	if (Controller && !Axis.IsNearlyZero())
	{
		const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
		const FVector Forward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector Right   = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Forward, Axis.Y);
		AddMovementInput(Right,   Axis.X);
	}
}

void ACartographerCharacter::Look(const FInputActionValue& Value)
{
	if (bFieldMapOpen) { return; }

	const FVector2D Axis = Value.Get<FVector2D>();
	if (Controller)
	{
		AddControllerYawInput(Axis.X);
		AddControllerPitchInput(-Axis.Y);
	}
}

void ACartographerCharacter::HandleToggleFieldMap(const FInputActionValue& /*Value*/)
{
	bFieldMapOpen = !bFieldMapOpen;
	OnToggleFieldMap();
}

void ACartographerCharacter::HandleToggleNotebook(const FInputActionValue& /*Value*/)
{
	OnToggleNotebook();
}

void ACartographerCharacter::HandleInteract(const FInputActionValue& /*Value*/)
{
	OnInteractPressed();
}

void ACartographerCharacter::HandleDrawStarted(const FInputActionValue& /*Value*/)
{
	OnDrawStarted();
}

void ACartographerCharacter::HandleDrawStopped(const FInputActionValue& /*Value*/)
{
	OnDrawStopped();
}

// --- Native default behaviour ---

void ACartographerCharacter::OnToggleFieldMap_Implementation()
{
	if (ACartographyPlayerController* PC = Cast<ACartographyPlayerController>(GetController()))
	{
		PC->ToggleFieldMap();
	}
}

void ACartographerCharacter::OnToggleNotebook_Implementation()
{
	if (ACartographyPlayerController* PC = Cast<ACartographyPlayerController>(GetController()))
	{
		PC->ToggleNotebook();
	}
}

void ACartographerCharacter::OnInteractPressed_Implementation()
{
	// Try interaction-component focus first, then nearby InkingDesk.
	if (UInteractionComponent* I = FindComponentByClass<UInteractionComponent>())
	{
		if (I->TryInteract()) { return; }
	}
	TArray<AActor*> Overlapping;
	GetOverlappingActors(Overlapping, AInkingDesk::StaticClass());
	for (AActor* A : Overlapping)
	{
		if (AInkingDesk* Desk = Cast<AInkingDesk>(A))
		{
			if (Desk->TryBeginInking(this))
			{
				if (ACartographyPlayerController* PC = Cast<ACartographyPlayerController>(GetController()))
				{
					PC->OpenInking();
				}
				return;
			}
		}
	}
}

void ACartographerCharacter::OnDrawStarted_Implementation()
{
	if (!bFieldMapOpen) { return; }
	bDrawing = true;
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) { return; }
	float MX, MY; int32 SX, SY;
	if (PC->GetMousePosition(MX, MY))
	{
		PC->GetViewportSize(SX, SY);
		LastDrawUV = (SX > 0 && SY > 0) ? FVector2D(MX / SX, MY / SY) : FVector2D::ZeroVector;
	}
}

void ACartographerCharacter::OnDrawStopped_Implementation()
{
	bDrawing = false;
}

void ACartographerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bDrawing || !bFieldMapOpen || !FieldMap) { return; }
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) { return; }
	float MX, MY; int32 SX, SY;
	if (!PC->GetMousePosition(MX, MY)) { return; }
	PC->GetViewportSize(SX, SY);
	if (SX <= 0 || SY <= 0) { return; }
	const FVector2D NowUV(FMath::Clamp(MX / SX, 0.f, 1.f),
	                      FMath::Clamp(MY / SY, 0.f, 1.f));
	FieldMap->DrawStroke(LastDrawUV, NowUV, ECartoMapTarget::FieldMap, ECartoBrushMode::Pencil);
	LastDrawUV = NowUV;
}
