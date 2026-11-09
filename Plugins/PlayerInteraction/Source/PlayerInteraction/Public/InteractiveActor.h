// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractionInterface.h"
#include "InteractiveActor.generated.h"

class UCurveFloat;

/**
 *  AInteractiveActor
 *  Simple Actor class that natively implements IPlayerInteractionInterface
 *  Use this as the base class for simple interactive objects: switches, treasure chests, etc.
 */
UCLASS(abstract)
class PLAYERINTERACTION_API AInteractiveActor : public AActor, public IPlayerInteractionInterface
{
	GENERATED_BODY()
	
public:	

	/** Constructor */
	AInteractiveActor();

protected:

	/** Base targeting score for this actor. Use a higher number to increase target priority. Use 0 to disable targeting. */
	UPROPERTY(EditAnywhere, Category = "Interactive Actor|Targeting", meta = (ClampMin = 0) )
	float TargetingScore = 1.0f;

	/** If true, the scoring will be multiplied by the distance score, otherwise it will not be modified. */
	UPROPERTY(EditAnywhere, Category = "Interactive Actor|Targeting")
	bool bUseDistanceScoring = false;

	/** If the interactor is beyond this distance, this actor can't be interacted with */
	UPROPERTY(EditAnywhere, Category = "Interactive Actor|Targeting", meta = (ClampMin = 1) )
	float MaxInteractionDistance = 50.0f;

	/** Curve used to score this actor based on its distance to the interactor normalized to MaxDistance */
	UPROPERTY(EditAnywhere, Category = "Interactive Actor|Targeting")
	TObjectPtr<UCurveFloat> DistanceScoreCurve;

	/** If true, the scoring will be multiplied by the positional facing score, otherwise it will not be modified. */
	UPROPERTY(EditAnywhere, Category = "Interactive Actor|Targeting")
	bool bUsePositionalFacingScoring = false;

	/** Curve used to score this actor based on its forward facing relative to the location of the interactor */
	UPROPERTY(EditAnywhere, Category = "Interactive Actor|Targeting")
	TObjectPtr<UCurveFloat> PositionalFacingScoreCurve;

	/** If true, the scoring will be multiplied by the interactor facing score, otherwise it will not be modified. */
	UPROPERTY(EditAnywhere, Category = "Interactive Actor|Targeting")
	bool bUseInteractorFacingScoring = false;

	/** Curve used to score this actor based on its forward facing relative to the forward facing of the interactor */
	UPROPERTY(EditAnywhere, Category = "Interactive Actor|Targeting")
	TObjectPtr<UCurveFloat> InteractorFacingScoreCurve;

	/** Interaction verb text to display when this actor is targeted */
	UPROPERTY(EditAnywhere, Category = "Interactive Actor|UI")
	FText InteractionVerbText;

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

	/** Allows BP to implement an interaction for this actor */
	UFUNCTION(BlueprintImplementableEvent, Category = "Interactive Actor", meta = (DisplayName = "On Interaction Started"))
	void BP_OnBeginInteraction(AActor* Interactor);
};
