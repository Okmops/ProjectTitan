// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/SampleNPC.h"
#include "InteractionInterface.h"
#include "DialogueNode.h"
#include "TitanNPC.generated.h"

class ATitanGameplayCharacter;

/**
 *  FTitanNPCInteractionPayload
 *  StateTree Event Payload struct used for NPC interactions
 */
USTRUCT(BlueprintType)
struct FTitanNPCInteractionPayload
{
	GENERATED_BODY()

	/** Pointer to the player character that started this interaction */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<ATitanGameplayCharacter> InteractingCharacter = nullptr;

	/** Built-in dialogue node to simplify one-page dialogue interactions */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FDialogueNode SimpleDialogue;
};

/**
 *  ATitanNPC
 *  Base class for all AI-controlled NPCs
 *  Implements the player interaction interface.
 *  On player interaction, triggers a StateTree event to transition to interacting state.
 *  The actual interaction logic is implemented on StateTree.
 */
UCLASS(abstract)
class TITAN_API ATitanNPC : public ASampleNPC, public IPlayerInteractionInterface
{
	GENERATED_BODY()
	
	/** Constructor */
	ATitanNPC(const FObjectInitializer& ObjectInitializer);

protected:

	/** Base targeting score for this actor. Use a higher number to increase target priority. Use 0 to disable targeting. */
	UPROPERTY(EditAnywhere, Category = "NPC|Targeting", meta = (ClampMin = 0) )
	float TargetingScore = 10.0f;

	/** If true, the scoring will be multiplied by the distance score, otherwise it will not be modified. */
	UPROPERTY(EditAnywhere, Category = "NPC|Targeting")
	bool bUseDistanceScoring = false;

	/** If the interactor is beyond this distance, this actor can't be interacted with */
	UPROPERTY(EditAnywhere, Category = "NPC|Targeting", meta = (ClampMin = 1) )
	float MaxInteractionDistance = 150.0f;

	/** Curve used to score this actor based on its distance to the interactor normalized to MaxDistance */
	UPROPERTY(EditAnywhere, Category = "NPC|Targeting")
	TObjectPtr<UCurveFloat> DistanceScoreCurve;

	/** Interaction verb text to display when this actor is targeted */
	UPROPERTY(EditAnywhere, Category = "NPC|Interaction")
	FText InteractionVerbText;

	/** StateTree event tag to send when this actor is interacted with */
	UPROPERTY(EditAnywhere, Category = "NPC|Interaction")
	FGameplayTag InteractionEventTag; 

	/** Optional dialogue node to pass to the StateTree on interaction for simple dialogue */
	UPROPERTY(EditAnywhere, Category = "NPC|Interaction")
	FDialogueNode InteractionSimpleDialogue;

public:

	/** Notifies the actor that it's being targeted for interaction */
	virtual void OnBeginTargeted() override;

	/** Notifies the actor that it's stopped being targeted for interaction */
	virtual void OnEndTargeted() override;

	/** Allows the actor to handle an interaction request. Should return true if the interaction was accepted. */
	virtual bool RequestInteraction(AActor* RequestingActor) override;

	/** Starts the interaction behavior. Assumes RequestInteraction was called beforehand and returned true. */
	virtual void BeginInteraction(AActor* Interactor) override;

	/** Scores this actor for target selection. Return a higher value to increase target priority. Return 0 or lower to prevent targeting. */
	virtual float ScoreTarget(const AActor* Interactor) const override;

	/** Returns the interaction verb text for this actor */
	virtual const FText GetInteractionVerbText() const override;

protected:

	/** Allows BP to respond to this actor being targeted for interaction */
	UFUNCTION(BlueprintImplementableEvent, Category = "Interactive Actor", meta = (DisplayName = "On Begin Targeted"))
	void BP_OnBeginTargeted();

	/** Allows BP to respond to this actor no longer being targeted for interaction */
	UFUNCTION(BlueprintImplementableEvent, Category = "Interactive Actor", meta = (DisplayName = "On End Targeted"))
	void BP_OnEndTargeted();
};
