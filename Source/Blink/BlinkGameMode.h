// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "BlinkGameMode.generated.h"

/**
 * 
 */
UCLASS()
class BLINK_API ABlinkGameMode : public AGameMode
{
	GENERATED_BODY()
	
	ABlinkGameMode(const FObjectInitializer& ObjectInitializer);
	
	virtual void StartPlay();
};
