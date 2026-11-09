// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "SampleAbilitySystemComponent.generated.h"

class UAbilitySet;
class UInputEventSet;
class UInputAction;
struct FInputActionInstance;
class USampleGameplayAbility;
class UEnhancedInputComponent;

/**
 *  USampleAbilitySystemComponent
 *  
 *  Base ability system component class used by this project.
 */
UCLASS()
class SAMPLEFRAMEWORK_API USampleAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

protected:

	/** Ability Sets to grant to the ability system on initialization */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TArray<TObjectPtr<UAbilitySet>> AbilitySets;

protected:

	/** Override to initialize the ASC with the built-in AbilitySet */
	virtual void BeginPlay() override;

public:

	/** Initializes the component for the given owner and avatar */
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;

protected:

	/** Override to handle enhanced ability initialization on instanced per execution abilities */
	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;

	/** Called during InitAbilityActorInfo to allow subclasses to validate ActorInfo data. Should return true if all info is valid */
	virtual bool ValidateActorInfo(FGameplayAbilityActorInfo* ActorInfo, AActor* InOwnerActor, AActor* InAvatarActor);

public:

	/** Attempts to activate any abilities with an activation policy set to OnSpawn */
	void TryActivateAbilitiesOnSpawn();

	/////////////////////////////////////
	// Input Event Handling

protected:

	/** Input Event Set to grant for triggering Gameplay Abilities */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category ="Input")
	TObjectPtr<UInputEventSet> InputEventSet;

	/** Input Event Map holding all translated input events and their triggering actions */
	UPROPERTY(Transient)
	TMap<TObjectPtr<const UInputAction>, FGameplayTag> InputEventMap;

	// Handles to abilities that had their input pressed this frame.
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;

	// Handles to abilities that had their input released this frame.
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;

	// Handles to abilities that have their input held.
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;

public:

	/** Binds the built-in input event set to an enhanced input component */
	void InitializeInputEvents(UEnhancedInputComponent* EnhancedInputComponent);

	/** Binds an input event set to an enhanced input component */
	void BindInputEventSet(const UInputEventSet* EventSet, UEnhancedInputComponent* EnhancedInputComponent);

	/** Binds an input action to a gameplay event */
	void BindInputEvent(const UInputAction* InputAction, FGameplayTag EventTag, UEnhancedInputComponent* EnhancedInputComponent);

	/** Clears all input events for an enhanced input component */
	void ClearInputEvents(UEnhancedInputComponent* EnhancedInputComponent);

	/** Called from PlayerController, allows the ASC to process ability inputs */
	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);

	/** Clears any cached ability inputs */
	void ClearAbilityInput();

protected:

	/** Override to handle input event replication */
	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;

	/** Ability input pressed event handler */
	void HandleInputPressed(const FInputActionInstance& ActionInstance);

	/** Ability input ongoing event handler */
	void HandleInputOngoing(const FInputActionInstance& ActionInstance);

	/** Ability input released event handler */
	void HandleInputReleased(const FInputActionInstance& ActionInstance);

	/////////////////////////////////////
	// Enhanced activation functionality

protected:

	/** Override to notify abilities of failure */
	virtual void NotifyAbilityFailed(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason) override;

	/** Notify client that an ability failed to activate */
	UFUNCTION(Client, Unreliable)
	void ClientNotifyAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason);

	/** Handle ability activation failure. Context is provided by the failure reason tag container. */
	void HandleAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason);

};
