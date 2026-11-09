// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "AbilitySystemInterface.h"
#include "GAS/AbilitySet.h"
#include "SamplePlayerPawn.generated.h"

class ASamplePlayerController;
class ASamplePlayerState;
class USampleAbilitySystemComponent;

/**
 *  ASamplePlayerPawn
 *	Base class for player-controlled Pawns
 *  Serve as Avatars for the PlayerState Ability System
 */
UCLASS(abstract, notplaceable)
class SAMPLEFRAMEWORK_API ASamplePlayerPawn : public APawn,
	public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	/** Constructor */
	ASamplePlayerPawn(const FObjectInitializer& ObjectInitializer);

protected:

	/** Ability Sets to grant to the PlayerState's ability system on possession */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TArray<TObjectPtr<UAbilitySet>> AbilitySets;

	/** Handles to the granted abilities */
	UPROPERTY(Transient)
	FAbilitySet_GrantedHandles AbilityHandles;

	/** Pointer to the possessing PlayerController */
	UPROPERTY(Transient)
	TObjectPtr<ASamplePlayerController> SamplePlayerController;

	/** Pointer to the possessing PlayerState */
	UPROPERTY(Transient)
	TObjectPtr<ASamplePlayerState> SamplePlayerState;

	/** Pointer to the PlayerState's ability system */
	UPROPERTY(Transient)
	TObjectPtr<USampleAbilitySystemComponent> PlayerStateAbilitySystem;

protected:

	/** PossessedBy initialization */
	virtual void PossessedBy(AController* NewController) override;

	/** UnPossessed cleanup */
	virtual void UnPossessed() override;

public:

	/** Grants and initializes gameplay abilities when possessed */
	void InitializeAbilities();

	/* Implement IAbilitySystemInterface */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

};
