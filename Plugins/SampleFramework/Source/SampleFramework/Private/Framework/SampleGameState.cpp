// Copyright Epic Games, Inc. All Rights Reserved.


#include "Framework/SampleGameState.h"
#include "GAS/SampleAbilitySystemComponent.h"

ASampleGameState::ASampleGameState(const FObjectInitializer& ObjectInitializer)
{
	// create and configure the ASC
	AbilitySystem = CreateDefaultSubobject<USampleAbilitySystemComponent>(TEXT("Ability System Component"));
	check(AbilitySystem);
}

void ASampleGameState::BeginPlay()
{
	Super::BeginPlay();

	// GameState is both the owner and avatar for its abilities
	AbilitySystem->InitAbilityActorInfo(this, this);
}

UAbilitySystemComponent* ASampleGameState::GetAbilitySystemComponent() const
{
	return AbilitySystem;
}
