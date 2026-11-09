// Copyright Epic Games, Inc. All Rights Reserved.


#include "Framework/SamplePlayerState.h"
#include "GAS/SampleAbilitySystemComponent.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "Framework/SamplePlayerPawn.h"


ASamplePlayerState::ASamplePlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// create the ASC
	AbilitySystem = CreateDefaultSubobject<USampleAbilitySystemComponent>(TEXT("Ability System Component"));
	check(AbilitySystem);
}

void ASamplePlayerState::BeginPlay()
{
	Super::BeginPlay();

	// get our player controller and ensure it's valid
	if (APlayerController* PC = GetPlayerController())
	{
		// initialize the ability input events
		if (UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(PC->InputComponent))
		{
			AbilitySystem->InitializeInputEvents(EnhancedInputComp);
		}

		// if we have a valid pawn and an attached player, initialize the ActorInfo
		if (GetPawn() && PC->Player)
		{
			OnPawnChanged(this, GetPawn(), nullptr);
		}
	}

	// listen to controlled pawn changes so we can update the AbilitySystem's ActorInfo
	OnPawnSet.AddDynamic(this, &ASamplePlayerState::OnPawnChanged);
}

void ASamplePlayerState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// get our player controller
	if (APlayerController* PC = GetPlayerController())
	{
		// clear the ability input events
		if (UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(PC->InputComponent))
		{
			AbilitySystem->ClearInputEvents(EnhancedInputComp);
		}
	}

	// clear the pawn changed delegate
	OnPawnSet.RemoveAll(this);

	Super::EndPlay(EndPlayReason);
}

void ASamplePlayerState::OnPawnChanged(APlayerState* PlayerState, APawn* NewPawn, APawn* OldPawn)
{
	// init the actor info
	AbilitySystem->InitAbilityActorInfo(this, NewPawn);

	// cast the Sample Pawn
	SamplePawn = Cast<ASamplePlayerPawn>(NewPawn);

	// initialize the pawn's abilities
	if (SamplePawn)
	{
		SamplePawn->InitializeAbilities();
	}
}

ASamplePlayerPawn* ASamplePlayerState::GetSamplePawn() const
{
	return SamplePawn.Get();
}

UAbilitySystemComponent* ASamplePlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystem;
}