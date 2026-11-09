// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "AbilitySystemInterface.h"
#include "SampleGameState.generated.h"

class USampleAbilitySystemComponent;

/**
 *  ASampleGameState
 *  Provides Gameplay Ability functionality to the GameState.
 *  Player-agnostic GameState abilities can serve as game phases, rules or event listeners.
 */
UCLASS(abstract)
class SAMPLEFRAMEWORK_API ASampleGameState : public AGameStateBase,
	public IAbilitySystemInterface
{
	GENERATED_BODY()
	
protected:

	/** Constructor */
	ASampleGameState(const FObjectInitializer& ObjectInitializer);

	/** Initialization */
	virtual void BeginPlay() override;

private:

	/** Ability System Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
	TObjectPtr <USampleAbilitySystemComponent> AbilitySystem;

	/////////////////////////////////////
	// Ability System Interface

public:

	/* Implement IAbilitySystemInterface */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
};
