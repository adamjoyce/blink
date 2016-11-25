// Fill out your copyright notice in the Description page of Project Settings.

#include "Blink.h"
#include "BlinkCharacter.h"
#include "EngineUtils.h"


// Sets default values
ABlinkCharacter::ABlinkCharacter(const FObjectInitializer& ObjectInitiliazer)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Create the first person camera component.
	FirstPersonCameraComponent = ObjectInitiliazer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("First person camera"));

	// Position the camera just above the character's eyeline.
	FirstPersonCameraComponent->RelativeLocation = FVector(0, 0, 50.0f + BaseEyeHeight);

	// Allow the character pawn to control rotation.
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Pair the camera to the character.
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());

	// Create a mesh component that will be used when being viewed from first person.
	FirstPersonMesh = ObjectInitiliazer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("First person mesh"));
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->SetupAttachment(FirstPersonCameraComponent);
	FirstPersonMesh->CastShadow = false;

	// Hide character mesh from the player.
	GetMesh()->SetOwnerNoSee(true);

	// Setup movement speed.
	RunSpeed = 600.0f;
	SprintSpeed = 1200.0f;
	CharacterMovementComponent = GetCharacterMovement();
	CharacterMovementComponent->MaxWalkSpeed = RunSpeed;

	MaxRaycastDistance = 20000.0f;
	HitscanImpulse = 100000.0f;
}

// Called when the game starts or when spawned
void ABlinkCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Using a BlinkCharacter"));
	}

	// Find the starting platform.
	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AActor* Actor = *ActorItr;
		if (Actor->ActorHasTag("StartingPlatform"))
		{
			CurrentPlatform = Actor;
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, Actor->GetName());
			break;
		}
	}
}

// Called every frame
void ABlinkCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABlinkCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	// Player movement.
	InputComponent->BindAxis("MoveForward", this, &ABlinkCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ABlinkCharacter::MoveRight);
	InputComponent->BindAxis("Turn", this, &ABlinkCharacter::AddControllerYawInput);
	InputComponent->BindAxis("LookUp", this, &ABlinkCharacter::AddControllerPitchInput);
	InputComponent->BindAction("Jump", IE_Pressed, this, &ABlinkCharacter::OnStartJump);
	InputComponent->BindAction("Jump", IE_Released, this, &ABlinkCharacter::OnStopJump);
	InputComponent->BindAction("Sprint", IE_Pressed, this, &ABlinkCharacter::OnStartSprint);
	InputComponent->BindAction("Sprint", IE_Released, this, &ABlinkCharacter::OnStopSprint);
	InputComponent->BindAction("Blink", IE_Pressed, this, &ABlinkCharacter::OnStartBlink);
	//InputComponent->BindAction("Blink", IE_Released, this, &ABlinkCharacter::OnStopBlink);

	// Player abilities.
	InputComponent->BindAction("ForcePush", IE_Pressed, this, &ABlinkCharacter::OnHitscanFire);
}

void ABlinkCharacter::MoveForward(float Value)
{
	if (Controller != NULL && Value != 0.0f)
	{
		// Find which way is forward.
		FRotator Rotation = Controller->GetControlRotation();

		// Limit pitch when walking or falling.
		if (GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling())
		{
			Rotation.Pitch = 0.0f;
		}

		// Add movement along the forward vector.
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ABlinkCharacter::MoveRight(float Value)
{
	if (Controller != NULL && Value != 0.0f)
	{
		// Find which way is right.
		FRotator Rotation = Controller->GetControlRotation();

		// Add movement alond the right direction axis.
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void ABlinkCharacter::OnStartJump()
{
	bPressedJump = true;
}

void ABlinkCharacter::OnStopJump()
{
	bPressedJump = false;
}

void ABlinkCharacter::OnStartSprint()
{
	CharacterMovementComponent->MaxWalkSpeed = SprintSpeed;
}

void ABlinkCharacter::OnStopSprint()
{
	CharacterMovementComponent->MaxWalkSpeed = RunSpeed;
}

FVector NewLocation;
void ABlinkCharacter::OnStartBlink()
{
	if (Controller != NULL)
	{
		// Get the player point of view.
		FVector CameraLoc;
		FRotator CameraRot;
		Controller->GetPlayerViewPoint(CameraLoc, CameraRot);

		// Raycast to check for collision.
		const FVector TraceStart = CameraLoc;
		const FVector Direction = CameraRot.Vector();
		const FVector TraceEnd = TraceStart + (Direction * MaxRaycastDistance);
		FCollisionQueryParams TraceParams(FName(TEXT("TraceBlink")), true, this);
		TraceParams.bTraceAsyncScene = true;
		TraceParams.bReturnPhysicalMaterial = false;
		TraceParams.bTraceComplex = true;

		FHitResult Hit(ForceInit);
		UWorld* const World = GetWorld();
		if (World)
		{
			World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, TraceParams);
			DrawDebugLine(World, TraceStart, TraceEnd, FColor::Yellow, true, 1.0f);

			// Find the new player position dependent on collision.
			if (Hit.GetActor() != NULL && Hit.GetActor()->ActorHasTag("Platform"))
			{
				UE_LOG(LogTemp, Warning, TEXT("Platform"));
				TogglePlatformCollider();
				NewLocation = Hit.GetActor()->GetActorLocation();
				SetActorRelativeLocation(NewLocation + FVector(0, 0, 400));
				CurrentPlatform = Hit.GetActor();
				TogglePlatformCollider();
			}
		}
	}
}

//void ABlinkCharacter::OnStopBlink()
//{
//	if (Controller != NULL)
//	{
//		SetActorRelativeLocation(NewLocation);
//	}
//}

//FVector ABlinkCharacter::GetBlinkLocation(FVector CurrentLocation)
//{
//	FVector ForwardDirection = FirstPersonCameraComponent->GetForwardVector();
//	FVector NewLocation = CurrentLocation + (ForwardDirection * 2000.0f);
//	return NewLocation;
//}

void ABlinkCharacter::TogglePlatformCollider()
{
	UBoxComponent* BoxCollider = CurrentPlatform->FindComponentByClass<UBoxComponent>();
	if (BoxCollider != NULL && BoxCollider->GetCollisionProfileName() == "OverlapOnlyPawn")
	{
		BoxCollider->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	}
	else
	{
		BoxCollider->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));
	}
}

void ABlinkCharacter::OnHitscanFire()
{
	// Get the camera point of view.
	FVector CameraLoc;
	FRotator CameraRot;
	Controller->GetPlayerViewPoint(CameraLoc, CameraRot);

	// Trace variables and parameters.
	const FVector TraceStart = CameraLoc;
	const FVector Direction = CameraRot.Vector();
	const FVector TraceEnd = TraceStart + (Direction * MaxRaycastDistance);
	FCollisionQueryParams TraceParams(FName(TEXT("TraceHitScanFire")), true, this);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = false;
	TraceParams.bTraceComplex = true;

	FHitResult Hit(ForceInit);
	UWorld* const World = GetWorld();
	if (World)
	{
		World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, TraceParams);
		DrawDebugLine(World, TraceStart, TraceEnd, FColor::Red, false, 1.0f);

		// Ignore static actors such as the floor.
		if (Hit.GetActor() != NULL && Hit.GetActor()->ActorHasTag("NoImpulse") == false)
		{
			//Hit.GetComponent()->AddImpulseAtLocation(Direction * HitscanImpulse, Hit.ImpactPoint);
			
			APlayerController* PlayerController = Cast<APlayerController>(GetController());

			// Create a damage event  
			TSubclassOf<UDamageType> const ValidDamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());
			FDamageEvent DamageEvent(ValidDamageTypeClass);
			TArray<AActor*, FDefaultAllocator> IgnoredActors;

			UGameplayStatics::ApplyRadialDamage(World, 1, Hit.ImpactPoint, 1, ValidDamageTypeClass, IgnoredActors);
		}
	}
}