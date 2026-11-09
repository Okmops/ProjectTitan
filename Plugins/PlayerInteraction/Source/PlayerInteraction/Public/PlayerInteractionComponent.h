// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameplayTagContainer.h"
#include "PlayerInteractionComponent.generated.h"

class IPlayerInteractionInterface;
class UAbilitySystemComponent;
class IAbilitySystemInterface;
class UCurveFloat;
class UMoverComponent;

PLAYERINTERACTION_API DECLARE_LOG_CATEGORY_EXTERN(LogInteractionComponent, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTargetChangedDelegate, TScriptInterface<IPlayerInteractionInterface>, NewTarget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTargetLostDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractionStartedDelegate, AActor*, InteractedActor);

/**
 *  UPlayerInteractionComponent
 *  Specialized box collision volume to detect, sort and maintain state of potential interaction targets.
 *  The component assumes that its owning actor implements IAbilitySystemInterface and can provide an ASC
 */
UCLASS()
class PLAYERINTERACTION_API UPlayerInteractionComponent : public UBoxComponent
{
	GENERATED_BODY()

public:

	/** Constructor */
	UPlayerInteractionComponent();
	
protected:

	/** Last chosen interaction target */
	UPROPERTY()
	TObjectPtr<AActor> CurrentTarget = nullptr;

	/** If true, the current target is no longer overlapping the interaction component and is pending removal */
	bool bCurrentTargetRemoved = false;

	/** Game time when this component was last added to the target list */
	float CurrentTargetAddedTime = 0.0f;

	/** Ability Interface on the owning actor */
	IAbilitySystemInterface* AbilityOwner = nullptr;

	/** Pointer to the owning actor's Mover component */
	TObjectPtr<UMoverComponent> MoverComp = nullptr;

	/** Timer to perform targeting updates */
	FTimerHandle UpdateTargetsTimer;

	/** Time between target update checks */
	UPROPERTY(EditAnywhere, Category = "Interaction Box|Targeting")
	float TimeBetweenTargetUpdates = 0.25f;
	
	/**  Time to wait before removing a recently added current target from the target list.
	 *   This helps reduce target throttling if the current target quickly leaves and reenters the collision box.
	 */
	UPROPERTY(EditAnywhere, Category = "Interaction Box|Targeting")
	float MinTargetingTime = 0.5f;

	/** Collision channel for the occlusion check */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category ="Interaction Box|Targeting")
	TEnumAsByte<ECollisionChannel> CollisionProbeChannel = ECollisionChannel::ECC_Visibility;

	/** Radius of the occlusion check swept sphere */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category ="Interaction Box|Targeting", meta = (ClampMin = 0))
	float CollisionProbeRadius = 32.0f;

	/** Tag query to perform on the Ability System Component to enable interactions. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category ="Interaction Box|Tags")
	FGameplayTagQuery AbilitySystemTagQuery;

	/** Tag query to perform on the Mover Component to enable interactions. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category ="Interaction Box|Tags")
	FGameplayTagQuery MoverTagQuery;

public:

	/** Called when a new interaction target is selected */
	UPROPERTY(BlueprintAssignable, BlueprintReadOnly, Category = "Interaction Box|Interaction")
	FTargetChangedDelegate OnTargetChanged;

	/** Called when no interaction targets are available */
	UPROPERTY(BlueprintAssignable, BlueprintReadOnly, Category = "Interaction Box|Interaction")
	FTargetLostDelegate OnTargetLost;

	/** Called when an interaction request on the current target has been accepted */
	UPROPERTY(BlueprintAssignable, BlueprintReadOnly, Category = "Interaction Box|Interaction")
	FInteractionStartedDelegate OnInteractionStarted;

protected:

	/** Curve used to score potential targets based on their distance to the owning actor */
	UPROPERTY(EditDefaultsOnly, Category = "Interaction Box|Target Scoring")
	TObjectPtr<UCurveFloat> DistanceScoreCurve;

	/** Curve used to score potential targets based on their facing relative to the targeting volume */
	UPROPERTY(EditDefaultsOnly, Category = "Interaction Box|Target Scoring")
	TObjectPtr<UCurveFloat> FacingScoreCurve;

public:

	/** Gameplay Init */
	virtual void BeginPlay() override;

	/** Gameplay Cleanup */
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	/** Requests an interaction with the currently selected target, if any. */
	UFUNCTION(BlueprintCallable, Category = "Interaction Box")
	void RequestInteraction();

protected:

	/** Handles BeginOverlap events to look for new targets */
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	/** Handles EndOverlap events to look for new targets */
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/** Updates the targeting list and reselects the current target if needed */
	UFUNCTION()
	void UpdateTargets();

	/** Checks the owning ASC for targeting restrictions and conditions */
	bool CheckTargetingConditions();

	/** Utility to deselect the current target */
	void DeselectCurrentTarget();
};
