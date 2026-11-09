// Copyright Epic Games, Inc. All Rights Reserved.


#include "Framework/SamplePlayerPawn.h"
#include "Framework/SamplePlayerController.h"
#include "Framework/SamplePlayerState.h"
#include "GAS/AbilitySet.h"
#include "GAS/SampleAbilitySystemComponent.h"

ASamplePlayerPawn::ASamplePlayerPawn(const FObjectInitializer& ObjectInitializer)
	: APawn(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASamplePlayerPawn::PossessedBy(AController* NewController)
{
	// cast the player controller
	SamplePlayerController = CastChecked<ASamplePlayerController>(NewController);

	// cast the player state
	SamplePlayerState = SamplePlayerController->GetPlayerState<ASamplePlayerState>();
	check(SamplePlayerState);

	// cast the ability system
	PlayerStateAbilitySystem = CastChecked<USampleAbilitySystemComponent>(SamplePlayerState->GetAbilitySystemComponent());

	// continue with the PossessedBy flow AFTER we set the pointers, so abilities can be correctly initialized
	Super::PossessedBy(NewController);
}

void ASamplePlayerPawn::UnPossessed()
{
	if (IsValid(PlayerStateAbilitySystem))
	{
		// remove the pawn's abilities from the PlayerState
		AbilityHandles.TakeFromAbilitySystem(PlayerStateAbilitySystem);
	}

	// clear the player controller and player state pointers
	SamplePlayerController = nullptr;
	SamplePlayerState = nullptr;
	PlayerStateAbilitySystem = nullptr;

	Super::UnPossessed();
}

void ASamplePlayerPawn::InitializeAbilities()
{	
	if (IsValid(PlayerStateAbilitySystem))
	{
		// give the pawn's ability sets to the PlayerState
		for (TObjectPtr<UAbilitySet> AbilitySet : AbilitySets)
		{
			if (AbilitySet)
			{
				AbilitySet->GiveToAbilitySystem(PlayerStateAbilitySystem, &AbilityHandles, this);
			}
		}

		// attempt to activate the granted abilities
		PlayerStateAbilitySystem->TryActivateAbilitiesOnSpawn();
	}
}

UAbilitySystemComponent* ASamplePlayerPawn::GetAbilitySystemComponent() const
{
	// if we have a player state, resolve its ASC
	if (SamplePlayerState)
	{
		return SamplePlayerState->GetAbilitySystemComponent();
	}

	return nullptr;
}
