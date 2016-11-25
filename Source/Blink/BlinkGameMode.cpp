// Fill out your copyright notice in the Description page of Project Settings.

#include "Blink.h"
#include "BlinkGameMode.h"

ABlinkGameMode::ABlinkGameMode(const FObjectInitializer& ObjectIntializer) : Super(ObjectIntializer)
{
	// Set the default pawn class.
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnObject(TEXT("Class'/Script/Blink.BlinkCharacter'"));
	if (PlayerPawnObject.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnObject.Class;
	}
}

void ABlinkGameMode::StartPlay()
{
	Super::StartPlay();
	StartMatch();
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("SUP WORLD"));
	}
}


