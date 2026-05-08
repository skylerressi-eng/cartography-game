// Copyright (c) 2026 The Last Cartographer.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CartographerCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UInputAction;
class UInputMappingContext;
class UFieldMapComponent;
class UFieldNotebookComponent;
struct FInputActionValue;

UCLASS(Blueprintable)
class CARTOGRAPHYGAME_API ACartographerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ACartographerCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Map")
	TObjectPtr<UFieldMapComponent> FieldMap;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Map")
	TObjectPtr<UFieldNotebookComponent> Notebook;

	// Enhanced Input — assign in BP child (BP_Cartographer).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	TObjectPtr<UInputAction> ToggleFieldMapAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	TObjectPtr<UInputAction> ToggleNotebookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	TObjectPtr<UInputAction> DrawAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Movement", meta=(ClampMin="50", ClampMax="600"))
	float WalkSpeed = 200.f;

	// Hooks. Default C++ implementations talk to the player controller's
	// widget toggles and to nearby InkingDesks; override in BP if needed.
	UFUNCTION(BlueprintNativeEvent, Category="Map")
	void OnToggleFieldMap();

	UFUNCTION(BlueprintNativeEvent, Category="Notebook")
	void OnToggleNotebook();

	UFUNCTION(BlueprintNativeEvent, Category="Interaction")
	void OnInteractPressed();

	UFUNCTION(BlueprintNativeEvent, Category="Map")
	void OnDrawStarted();

	UFUNCTION(BlueprintNativeEvent, Category="Map")
	void OnDrawStopped();

	UFUNCTION(BlueprintCallable, Category="Map")
	bool IsFieldMapOpen() const { return bFieldMapOpen; }

	UFUNCTION(BlueprintCallable, Category="Map")
	void SetFieldMapOpen(bool bOpen) { bFieldMapOpen = bOpen; }

	UPROPERTY(BlueprintReadOnly, Category="Map")
	bool bDrawing = false;

	UPROPERTY(BlueprintReadOnly, Category="Map")
	FVector2D LastDrawUV = FVector2D::ZeroVector;

	virtual void OnToggleFieldMap_Implementation();
	virtual void OnToggleNotebook_Implementation();
	virtual void OnInteractPressed_Implementation();
	virtual void OnDrawStarted_Implementation();
	virtual void OnDrawStopped_Implementation();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void HandleToggleFieldMap(const FInputActionValue& Value);
	void HandleToggleNotebook(const FInputActionValue& Value);
	void HandleInteract(const FInputActionValue& Value);
	void HandleDrawStarted(const FInputActionValue& Value);
	void HandleDrawStopped(const FInputActionValue& Value);

private:
	bool bFieldMapOpen = false;
};
