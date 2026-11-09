// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "SamplePlayerState.generated.h"

class USampleAbilitySystemComponent;
class ASamplePlayerPawn;

/**
 *  ASamplePlayerState
 *  Provides Gameplay Ability functionality at the PlayerState level.
 *  Because of this, it can have abilities that are independent of the Pawn lifecycle.
 */
UCLASS()
class SAMPLEFRAMEWORK_API ASamplePlayerState : public APlayerState,
	public IAbilitySystemInterface
{
	GENERATED_BODY()
	
protected:

	/** Constructor */
	ASamplePlayerState(const FObjectInitializer& ObjectInitializer);

	/** Initialization */
	virtual void BeginPlay() override;

	/** De-Initialization */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:

	/** Ability System Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USampleAbilitySystemComponent> AbilitySystem = nullptr;

protected:

	/** Cast pointer to the Sample player Pawn */
	UPROPERTY()
	TObjectPtr<ASamplePlayerPawn> SamplePawn;

	/** Handles ability reinitialization on pawn changes */
	UFUNCTION()
	virtual void OnPawnChanged(APlayerState* PlayerState, APawn* NewPawn, APawn* OldPawn);

	UFUNCTION(BlueprintPure, Category="Player State")
	ASamplePlayerPawn* GetSamplePawn() const;

	/////////////////////////////////////
	// Ability System Interface

public:

	/* Implement IAbilitySystemInterface */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

};
