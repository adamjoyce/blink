// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "BlinkCharacter.generated.h"

UCLASS()
class BLINK_API ABlinkCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Movement variables.
	float RunSpeed;
	float SprintSpeed;

	// Ability variables.
	float MaxRaycastDistance;
	float HitscanImpulse;

	// The platform the player is standing on.
	UPROPERTY()
	AActor* CurrentPlatform;

	// First person camera.
	UPROPERTY()
	UCameraComponent* FirstPersonCameraComponent;

	// Mesh viewed when controlling the character.
	UPROPERTY(VisibleDefaultsOnly, Category = "Mesh")
	USkeletalMeshComponent* FirstPersonMesh;

	// Change character movement variables here.
	UPROPERTY()
	UCharacterMovementComponent* CharacterMovementComponent;

	// Sets default values for this character's properties
	ABlinkCharacter(const FObjectInitializer& ObjectInitiliazer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

protected:
	// Called to bind functionality to input.
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	// Handles forward and backwards movement.
	UFUNCTION()
	void MoveForward(float Value);

	// Handles strafing movement.
	UFUNCTION()
	void MoveRight(float Value);

	// Sets the jump flag when key is pressed.
	UFUNCTION()
	void OnStartJump();

	// Clears the jump flag when key is released.
	UFUNCTION()
	void OnStopJump();

	// Increases run speed when key is pressed.
	UFUNCTION()
	void OnStartSprint();

	// Resets run speed when key is released.
	UFUNCTION()
	void OnStopSprint();

	// Handles blink movement.
	UFUNCTION()
	void OnStartBlink();

	// Handles blink movement.
	/*UFUNCTION()
	void OnStopBlink();*/

	/*UFUNCTION()
	FVector GetBlinkLocation(FVector CurrentLocation);*/

	// Handles blink movement.
	UFUNCTION()
	void TogglePlatformCollider();

	UFUNCTION()
	void OnHitscanFire();
};
