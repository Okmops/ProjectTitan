// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "NativeGameplayTags.h"
#include "SampleAbilityTypes.h"
#include "SampleGameplayAbility.generated.h"

class UAbilityCost;
class APlayerController;
class APlayerState;
class APawn;
class AAIController;

/**
 *  EAbilityActivationPolicy
 *  Defines how an ability is meant to activate.
 */
UENUM(BlueprintType)
enum class EAbilityActivationPolicy : uint8
{
	// Try to activate the ability when the input is triggered.
	OnInputTriggered,

	// Continually try to activate the ability while the input is active.
	WhileInputActive,

	// Try to activate the ability when an avatar is assigned.
	OnSpawn
};

/**
 *  USampleGameplayAbility
 *  Extends GameplayAbility with extra functionality:
 *  - Enhanced initialization events
 *  - Automatic activation and deactivation on input or spawn conditions
 *  - Input event handling
 *  - Additional extensible costs
 */
UCLASS(abstract)
class SAMPLEFRAMEWORK_API USampleGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
protected:

	/** Constructor */
	USampleGameplayAbility(const FObjectInitializer& ObjectInitializer);
		
protected:

	/////////////////////////////////////
	// Input Event Handling

protected:

	/** Input event tag to associate with this ability.
	 *  If its input tag matches and its activation policy is input-based,
	 *  it will attempt to activate or deactivate accordingly.
	 *  It will also receive an Input Ongoing event for as long as the input is held.
	 **/
	UPROPERTY(EditDefaultsOnly, Category="Input")
	FGameplayTag InputEventTag;

public:

	/** Returns the input event tag associated with this ability */
	const FGameplayTag& GetInputEventTag() const { return InputEventTag; }

	/** Called while the matching input is held, but only if its activation policy is set to while input held */
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "Input Event Ongoing", Category="Ability")
	void K2_InputEventOngoing();

	/////////////////////////////////////
	// Additional events and BP hooks

protected:

	/** Set to true when a valid owner is assigned through InitAbilityActorInfo */
	bool bOwnerInitialized = false;

	/** Set to true when the ability passes validation checks on InitAbilityActorInfo */
	bool bAbilityValidated = false;

	/** If this class is not None, the ability will check that its Owner matches the provided class to be considered valid. */
	UPROPERTY(EditDefaultsOnly, Category="Ability Validation")
	TSubclassOf<AActor> ExpectedOwnerClass;

	/** If this is true, the ability is only considered valid if its Ability System Component has an Avatar */
	UPROPERTY(EditDefaultsOnly, Category="Ability Validation")
	bool bValidationRequiresAvatar = false;

	/** If this class is not None, the ability will check that its Avatar matches the provided class to be considered valid. */
	UPROPERTY(EditDefaultsOnly, Category="Ability Validation")
	TSubclassOf<AActor> ExpectedAvatarClass;

public:

	/** Called when the ability fails to activate. Reason context is provided as gameplay tags */
	void OnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const
	{
		NativeOnAbilityFailedToActivate(FailedReason);
		K2_OnAbilityFailedToActivate(FailedReason);
	}

protected:

	/** Overriden to provide additional activation-time initialization flow */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	/** Called when the ability fails to activate */
	virtual void NativeOnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const;

	/** Override to provide a BP hook on ability added */
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	/** Override to provide a BP hook on ability removed */
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

public:

	/** Allows the ability to validate the actor info and run further initialization. */
	virtual bool ValidateAbilityActorInfo(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& AbilitySpec);

	/** Saves the latest validation status for this ability */
	void SetAbilityValidated(bool bValidated);

	/** Called when the owner has been initialized. */
	virtual void OnOwnerInitialized();

	/** Called when the avatar has been changed. */
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	/** Override to check that the ability is validated prior to activating */
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;

protected:

	/** Hook to allow subclasses to implement specific owner validations or behaviors */
	virtual void NativeOnOwnerInitialized();

	/** Hook to allow subclasses to implement specific avatar changed validations or behaviors */
	virtual void NativeOnAvatarChanged();

	/** Called when the ability fails to activate. Reason context is provided as gameplay tags  */
	UFUNCTION(BlueprintImplementableEvent, DisplayName="On Ability Failed to Activate", Category="Ability")
	void K2_OnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const;

	/** Called when this ability is granted to the ability system component. */
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "On Ability Added", Category="Ability")
	void K2_OnAbilityAdded();

	/** Called when this ability is removed from the ability system component. */
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "On Ability Removed", Category="Ability")
	void K2_OnAbilityRemoved();

	/** Called when this ability's owner is fully validated and initialized. */
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "On Owner Initialized", Category="Ability")
	void K2_OnOwnerInitialized();

	/** Called when the avatar for this pawn has changed */
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "On Avatar Changed", Category="Ability")
	void K2_OnAvatarChanged();

	/** BP hook to allow abilities to initialize themselves just prior to activation */
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "Initialize Ability for Activation", Category="Ability")
	void K2_InitAbilityForActivation();

	/////////////////////////////////////
	// Activation Features

protected:

	// Defines how this ability is meant to activate.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Activation")
	EAbilityActivationPolicy ActivationPolicy;

public:

	/** Returns the activation policy for this ability */
	EAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }

	/** Attempts to activate this ability when it is first granted to the ASC */
	void TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const;

	/////////////////////////////////////
	// Additional costs

protected:

	/** Additional costs that must be paid to activate this ability */
	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Costs")
	TArray<TObjectPtr<UAbilityCost>> AdditionalCosts;

public:

	/** Override to check for additional costs */
	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	/** Override to apply additional costs */
	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;

	/////////////////////////////////////
	// Additional AbilityActorInfo accessors

protected:

	/** Returns the expanded Sample Ability Actor Info struct */
	UFUNCTION(BlueprintPure, Category = "Ability")
	FSampleGameplayAbilityActorInfo GetSampleActorInfo() const;

	/** Returns the PlayerController associated with this ability. WARNING: This can be None! */
	UFUNCTION(BlueprintPure, Category = "Ability")
	APlayerController* GetPlayerControllerFromActorInfo() const;

	/** Returns the AIController associated with this ability. WARNING: This can be None! */
	UFUNCTION(BlueprintPure, Category = "Ability")
	AAIController* GetAIControllerFromActorInfo() const;

	/** Returns the PlayerState associated with this ability. WARNING: This can be None! */
	UFUNCTION(BlueprintPure, Category = "Ability")
	APlayerState* GetPlayerStateFromActorInfo() const;

	/** Returns the Pawn Avatar associated with this ability. WARNING: This can be None! */
	UFUNCTION(BlueprintPure, Category = "Ability")
	APawn* GetPawnAvatarFromActorInfo() const;

	/** Returns true if the avatar actor is controlled by an AIController */
	UFUNCTION(BlueprintPure, Category = "Ability")
	bool IsAIControlled() const;

	/** Returns true if the avatar actor is controlled by an PlayerController */
	UFUNCTION(BlueprintPure, Category = "Ability")
	bool IsPlayerControlled() const;

private:

	/** Utility function to cast and validate the specialized SampleActorInfo struct */
	const FSampleGameplayAbilityActorInfo* GetSampleActorInfoPtr() const;
};
