// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "SampleNPC.generated.h"

class USampleAbilitySystemComponent;
class ASampleNPCController;
class UStateTree;
struct FInstancedStruct;

/**
 *  ASampleNPC
 *  Base class for placeable or spawnable game-controlled Characters
 *  - StateTree support when possessed by an ASampleNPCController
 *  - GAS support
 */
UCLASS(abstract)
class SAMPLEFRAMEWORK_API ASampleNPC : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	/** Constructor */
	ASampleNPC(const FObjectInitializer& ObjectInitializer);

protected:

	/** Pointer to the ability system component for this pawn */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USampleAbilitySystemComponent> AbilitySystem;

	/** Pointer to the NPC Controller possessing this pawn */
	UPROPERTY(Transient)
	TObjectPtr<ASampleNPCController> NPCController;

	/** StateTree to run when the pawn is possessed and initialized */
	UPROPERTY(EditAnywhere, Category = "NPC")
	TObjectPtr<UStateTree> StartingStateTree;

protected:

	/** BeginPlay ability initialization */
	virtual void BeginPlay() override;

	/** PossessedBy ability initialization */
	virtual void PossessedBy(AController* NewController) override;

	/** UnPossessed cleanup */
	virtual void UnPossessed() override;

public:

	/** Attempts to initialize all gameplay abilities on this pawn. Note that not all abilities may be initialized on one call due to requirements, e.g. missing a Controller */
	void TryInitializeAbilities();

	/* Implement IAbilitySystemInterface */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	/** Returns the NPC Controller possessing this pawn */
	UFUNCTION(BlueprintPure, Category = "NPC")
	ASampleNPCController* GetNPCController() const;

	/** Returns the starting StateTree for this NPC */
	UFUNCTION(BlueprintPure, Category = "NPC")
	UStateTree* GetStartingStateTree() const;

	/** Sends a StateTree event to the possessing controller */
	UFUNCTION(BlueprintCallable, Category = "NPC")
	void SendStateTreeEvent(const FGameplayTag& Tag, const FInstancedStruct& Payload);
};
