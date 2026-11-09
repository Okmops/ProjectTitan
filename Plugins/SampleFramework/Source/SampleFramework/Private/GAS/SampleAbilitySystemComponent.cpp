// Copyright Epic Games, Inc. All Rights Reserved.


#include "GAS/SampleAbilitySystemComponent.h"
#include "GAS/AbilitySet.h"
#include "GAS/InputEventSet.h"
#include "GAS/SampleGameplayAbility.h"
#include "GAS/SampleGameplayTags.h"
#include "SampleLogging.h"
#include "GameFramework/Pawn.h"
#include "AbilitySystemGlobals.h"
#include "GameplayTagResponseTable.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"

void USampleAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();

	// grant the built-in AbilitySets
	// these abilities will have the same lifespan as the component
	for (TObjectPtr<UAbilitySet> AbilitySet : AbilitySets)
	{
		if (AbilitySet)
		{
			AbilitySet->GiveToAbilitySystem(this, nullptr);
		}
	}
}

void USampleAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	// @TODO
	// This is temporarily overriding all of UAbilitySystemComponent's InitAbilityActorInfo code
	// until we can find a more robust way of introducing ability validation and owner initialization

	// ensure the ability actor info is valid
	check(AbilityActorInfo.IsValid());

	// Validate the ActorInfo at the ability system level
	// This should be considered a system failure point
	const bool bValidActorInfo = ValidateActorInfo(AbilityActorInfo.Get(), InOwnerActor, InAvatarActor);
	UE_CLOG(!bValidActorInfo, LogSampleAbilitySystem, Error, TEXT("Ability System Component [%s] failed ActorInfo validation."), *GetName());
	check(bValidActorInfo);

	// determine if the owner and avatar pointers have changed
	bool WasOwnerNull = (AbilityActorInfo->OwnerActor == nullptr);
	bool WasAvatarNull = (AbilityActorInfo->AvatarActor == nullptr);
	bool AvatarChanged = (InAvatarActor != AbilityActorInfo->AvatarActor);

	// initialize the ActorInfo struct
	AbilityActorInfo->InitFromActor(InOwnerActor, InAvatarActor, this);

	// update the owner
	SetOwnerActor(InOwnerActor);

	// caching the previous value of the actor so we can check against it but then setting the value to the new because it may get used
	const AActor* PrevAvatarActor = GetAvatarActor_Direct();
	SetAvatarActor_Direct(InAvatarActor);

	// if the avatar actor was null but won't be after this, we want to run the deferred gameplaycues that may not have run in NetDeltaSerialize
	// Conversely, if the ability actor was previously null, then the effects would not run in the NetDeltaSerialize. As such we want to run them now.
	if ((WasAvatarNull || PrevAvatarActor == nullptr) && InAvatarActor != nullptr)
	{
		HandleDeferredGameplayCues(&ActiveGameplayEffects);
	}

	// ensure the ability list is not edited while we iterate
	ABILITYLIST_SCOPE_LOCK();

	// go through each granted ability
	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		TArray<UGameplayAbility*> Instances = AbilitySpec.GetAbilityInstances();
		
		for (UGameplayAbility* AbilityInstance : Instances)
		{
			// do enhanced validation and initialization
			if (USampleGameplayAbility* SampleAbilityInstance = Cast<USampleGameplayAbility>(AbilityInstance))
			{
				// allow the ability to validate the ActorInfo
				SampleAbilityInstance->SetAbilityValidated(SampleAbilityInstance->ValidateAbilityActorInfo(AbilityActorInfo.Get(), AbilitySpec));

				// initialize the owner
				if (WasOwnerNull)
				{
					SampleAbilityInstance->OnOwnerInitialized();
				}
			}

			// only call the avatar set hook if the avatar changed
			if (AvatarChanged)
			{
				AbilityInstance->OnAvatarSet(AbilityActorInfo.Get(), AbilitySpec);
			}
		}
	}

	// register with the tag response table
	if (UGameplayTagReponseTable* TagTable = UAbilitySystemGlobals::Get().GetGameplayTagResponseTable())
	{
		TagTable->RegisterResponseForEvents(this);
	}
	
	// update the anim montage info
	LocalAnimMontageInfo = FGameplayAbilityLocalAnimMontage();
	if (IsOwnerActorAuthoritative())
	{
		SetRepAnimMontageInfo(FGameplayAbilityRepAnimMontage());
	}

	// update the replicated montage if it was pending
	if (bPendingMontageRep)
	{
		OnRep_ReplicatedAnimMontage();
	}

	// activate any abilities that should automatically activate on spawn
	TryActivateAbilitiesOnSpawn();
}

void USampleAbilitySystemComponent::TryActivateAbilitiesOnSpawn()
{
	// ensure the ability list is not edited while we iterate
	ABILITYLIST_SCOPE_LOCK();

	// go through each granted ability
	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		// attempt to activate the ability
		if (const USampleGameplayAbility* SampleAbilityCDO = Cast<USampleGameplayAbility>(AbilitySpec.Ability))
		{
			SampleAbilityCDO->TryActivateAbilityOnSpawn(AbilityActorInfo.Get(), AbilitySpec);
		}
	}
}

void USampleAbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnGiveAbility(AbilitySpec);

	// resolve the ability instance depending on the instancing policy
	UGameplayAbility* AbilityInstance = AbilitySpec.Ability;
	
	if (AbilityInstance->GetInstancingPolicy() == EGameplayAbilityInstancingPolicy::InstancedPerActor)
	{
		AbilityInstance = AbilitySpec.GetPrimaryInstance();
	}

	// this function may get called before ActorInfo is created so ensure the ActorInfo is valid
	const FGameplayAbilityActorInfo* ActorInfo = AbilityActorInfo.Get();

	if (ActorInfo)
	{
		// do enhanced validation and initialization
		if (USampleGameplayAbility* SampleAbilityInstance = Cast<USampleGameplayAbility>(AbilityInstance))
		{
			// allow the ability to validate the ActorInfo
			SampleAbilityInstance->SetAbilityValidated(SampleAbilityInstance->ValidateAbilityActorInfo(AbilityActorInfo.Get(), AbilitySpec));

			// initialize the owner
			SampleAbilityInstance->OnOwnerInitialized();
		}

		// initialize the avatar
		AbilityInstance->OnAvatarSet(AbilityActorInfo.Get(), AbilitySpec);
	}
}

bool USampleAbilitySystemComponent::ValidateActorInfo(FGameplayAbilityActorInfo* ActorInfo, AActor* InOwnerActor, AActor* InAvatarActor)
{
	// Subclasses should override to implement owner/avatar validations, etc.
	// Return true if all validations pass
	// Failing this validation should be considered a fatal error
	return true;
}

void USampleAbilitySystemComponent::InitializeInputEvents(UEnhancedInputComponent* EnhancedInputComponent)
{
	// bind the default input event set
	BindInputEventSet(InputEventSet, EnhancedInputComponent);
}

void USampleAbilitySystemComponent::BindInputEventSet(const UInputEventSet* EventSet, UEnhancedInputComponent* EnhancedInputComponent)
{
	if (!EventSet)
	{
		UE_LOG(LogSampleAbilitySystem, Warning, TEXT("Attempted to bind invalid Input Event Set to Ability System Component [%s]."), *GetNameSafe(this));
		return;
	}

	if (!EnhancedInputComponent)
	{
		UE_LOG(LogSampleAbilitySystem, Warning, TEXT("Attempted to bind an Input Event Set to Ability System Component [%s] with an invalid Input Component."), *GetNameSafe(this));
		return;
	}

	// grant the input event set to this ASC
	EventSet->GiveToAbilitySystem(this, EnhancedInputComponent);
}

void USampleAbilitySystemComponent::BindInputEvent(const UInputAction* InputAction, FGameplayTag EventTag, UEnhancedInputComponent* EnhancedInputComponent)
{
	// ensure the input component is valid
	if (!EnhancedInputComponent)
	{
		UE_LOG(LogSampleAbilitySystem, Warning, TEXT("Attempted to bind to invalid Enhanced Input Component [%s]."), *GetNameSafe(this));
		return;
	}

	// ensure the input action is valid
	if (!InputAction)
	{
		UE_LOG(LogSampleAbilitySystem, Warning, TEXT("Attempted to bind invalid Input Action to Ability System Component [%s]."), *GetNameSafe(this));
		return;
	}

	// ensure we only have one binding per input action
	if (FGameplayTag* BoundTag = InputEventMap.Find(InputAction))
	{
		UE_LOG(LogSampleAbilitySystem, Warning, TEXT("Attempted to bind to previously bound Input Action Tag [%s] on Ability System Component [%s]."), *BoundTag->GetTagName().ToString(), *GetNameSafe(this));
		return;
	}

	// ensure the event tag is valid
	if (EventTag == FGameplayTag::EmptyTag)
	{
		UE_LOG(LogSampleAbilitySystem, Warning, TEXT("Attempted to bind to empty Input Action Tag on Ability System Component [%s]."), *GetNameSafe(this));
		return;
	}

	// save the event tag to the action map
	InputEventMap.Add(InputAction, EventTag);

	// create the bindings
	EnhancedInputComponent->BindAction(InputAction, ETriggerEvent::Started, this, &USampleAbilitySystemComponent::HandleInputPressed);
	EnhancedInputComponent->BindAction(InputAction, ETriggerEvent::Ongoing, this, &USampleAbilitySystemComponent::HandleInputOngoing);
	EnhancedInputComponent->BindAction(InputAction, ETriggerEvent::Completed, this, &USampleAbilitySystemComponent::HandleInputReleased);
}

void USampleAbilitySystemComponent::ClearInputEvents(UEnhancedInputComponent* EnhancedInputComponent)
{
	if (!EnhancedInputComponent)
	{
		UE_LOG(LogSampleAbilitySystem, Warning, TEXT("Attempted to clear input events on invalid Input Component on Ability System Component [%s]."), *GetNameSafe(this));
		return;
	}

	// clear all bindings for this pawn
	EnhancedInputComponent->ClearBindingsForObject(this);

	// clear the input event map
	InputEventMap.Empty();
}

void USampleAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	// check if input is currently blocked at the ASC level
	if (HasMatchingGameplayTag(SampleGameplayTags::TAG_Ability_Input_Blocked))
	{
		ClearAbilityInput();
		return;
	}

	static TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
	AbilitiesToActivate.Reset();

	static TArray<FGameplayAbilitySpecHandle> AbilitiesToCancel;
	AbilitiesToCancel.Reset();

	//@TODO: See if we can use FScopedServerAbilityRPCBatcher ScopedRPCBatcher in some of these loops

	// Process all abilities that activate when the input is held.
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputHeldSpecHandles)
	{
		if (const FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability && !AbilitySpec->IsActive())
			{
				const USampleGameplayAbility* SampleAbilityCDO = Cast<USampleGameplayAbility>(AbilitySpec->Ability);
				if (SampleAbilityCDO && SampleAbilityCDO->GetActivationPolicy() == EAbilityActivationPolicy::WhileInputActive)
				{
					AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
				}
			}
		}
	}

	// Process all abilities that had their input pressed this frame.
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputPressedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = true;

				if (AbilitySpec->IsActive())
				{
					// Ability is active so pass along the input event.
					AbilitySpecInputPressed(*AbilitySpec);
				}
				else
				{
					const USampleGameplayAbility* SampleAbilityCDO = Cast<USampleGameplayAbility>(AbilitySpec->Ability);

					if (SampleAbilityCDO && SampleAbilityCDO->GetActivationPolicy() == EAbilityActivationPolicy::OnInputTriggered)
					{
						AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
					}
				}
			}
		}
	}

	// Try to activate all the abilities that are from presses and holds.
	// We do it all at once so that held inputs don't activate the ability
	// and then also send a input event to the ability because of the press.
	for (const FGameplayAbilitySpecHandle& AbilitySpecHandle : AbilitiesToActivate)
	{
		TryActivateAbility(AbilitySpecHandle);
	}

	// Process all abilities that had their input released this frame.
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputReleasedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = false;

				if (AbilitySpec->IsActive())
				{
					// Ability is active so pass along the input event.
					AbilitySpecInputReleased(*AbilitySpec);

					// check if the ability should be canceled on release
					const USampleGameplayAbility* SampleAbilityCDO = Cast<USampleGameplayAbility>(AbilitySpec->Ability);
					if (SampleAbilityCDO && SampleAbilityCDO->GetActivationPolicy() == EAbilityActivationPolicy::WhileInputActive)
					{
						AbilitiesToCancel.AddUnique(SpecHandle);
					}
				}
			}
		}
	}

	// Cancel all the abilities that are from releases and holds.
	// @TODO: do we want to change this so it's ended by normal flow instead of canceled?
	for (const FGameplayAbilitySpecHandle& AbilitySpecHandle : AbilitiesToCancel)
	{
		CancelAbilityHandle(AbilitySpecHandle);
	}

	// Clear the cached ability handles.
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();

	// Process all abilities that have their input held this frame
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputHeldSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability && !AbilitySpec->IsActive())
			{
				USampleGameplayAbility* SampleAbilityCDO = Cast<USampleGameplayAbility>(AbilitySpec->Ability);
				if (SampleAbilityCDO && SampleAbilityCDO->GetActivationPolicy() == EAbilityActivationPolicy::WhileInputActive)
				{
					// send the ongoing event to the ability
					SampleAbilityCDO->K2_InputEventOngoing();
				}
			}
		}
	}

}

void USampleAbilitySystemComponent::ClearAbilityInput()
{
	// reset the cached spec handles
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();
}

void USampleAbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputPressed(Spec);

	// We don't support UGameplayAbility::bReplicateInputDirectly.
	// Use replicated events instead so that the WaitInputPress ability task works.
	if (Spec.IsActive())
	{
		// get the prediction key for this activation
		const UGameplayAbility* Instance = Spec.GetPrimaryInstance();
		FPredictionKey OriginalPredictionKey = Instance ? Instance->GetCurrentActivationInfo().GetActivationPredictionKey() : FPredictionKey();

		// Invoke the InputPressed event. This is not replicated here. If someone is listening, they may replicate the InputPressed event to the server.
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, OriginalPredictionKey);
	}
}

void USampleAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputReleased(Spec);

	// We don't support UGameplayAbility::bReplicateInputDirectly.
	// Use replicated events instead so that the WaitInputRelease ability task works.
	if (Spec.IsActive())
	{
		// get the prediction key for this activation
		const UGameplayAbility* Instance = Spec.GetPrimaryInstance();
		FPredictionKey OriginalPredictionKey = Instance ? Instance->GetCurrentActivationInfo().GetActivationPredictionKey() : FPredictionKey();

		// Invoke the InputReleased event. This is not replicated here. If someone is listening, they may replicate the InputReleased event to the server.
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, OriginalPredictionKey);
	}
}

void USampleAbilitySystemComponent::HandleInputPressed(const FInputActionInstance& ActionInstance)
{
	// get the source input action
	const UInputAction* InputAction = ActionInstance.GetSourceAction();

	// find the event mapped to the input action
	if (FGameplayTag* EventMapping = InputEventMap.Find(InputAction))
	{
		// process all activatable abilities
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			// check if the ability has a matching input activation tag
			if (USampleGameplayAbility* SampleAbility = Cast<USampleGameplayAbility>(AbilitySpec.Ability))
			{
				if (SampleAbility->GetInputEventTag() == *EventMapping)
				{
					// flag this ability spec for press
					InputPressedSpecHandles.AddUnique(AbilitySpec.Handle);
					InputHeldSpecHandles.AddUnique(AbilitySpec.Handle);
				}
			}
		}

		// process the input event
		FGameplayEventData Payload;
		Payload.InstigatorTags.AddTag(SampleGameplayTags::TAG_Ability_Input_Pressed);

		HandleGameplayEvent(*EventMapping, &Payload);
	}
}

void USampleAbilitySystemComponent::HandleInputOngoing(const FInputActionInstance& ActionInstance)
{
	// get the source input action
	const UInputAction* InputAction = ActionInstance.GetSourceAction();

	// find the event mapped to the input action
	if (FGameplayTag* EventMapping = InputEventMap.Find(InputAction))
	{
		// process the input event
		FGameplayEventData Payload;
		Payload.InstigatorTags.AddTag(SampleGameplayTags::TAG_Ability_Input_Ongoing);

		HandleGameplayEvent(*EventMapping, &Payload);
	}
}

void USampleAbilitySystemComponent::HandleInputReleased(const FInputActionInstance& ActionInstance)
{
	// get the source input action
	const UInputAction* InputAction = ActionInstance.GetSourceAction();

	// find the event mapped to the input action
	if (FGameplayTag* EventMapping = InputEventMap.Find(InputAction))
	{
		// process all activatable abilities
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			// check if the ability has a matching input activation tag
			if (USampleGameplayAbility* SampleAbility = Cast<USampleGameplayAbility>(AbilitySpec.Ability))
			{
				if (SampleAbility->GetInputEventTag() == *EventMapping)
				{
					// flag this ability spec for release
					InputReleasedSpecHandles.AddUnique(AbilitySpec.Handle);
					InputHeldSpecHandles.Remove(AbilitySpec.Handle);
				}
			}
		}

		// process the input event
		FGameplayEventData Payload;
		Payload.InstigatorTags.AddTag(SampleGameplayTags::TAG_Ability_Input_Released);

		HandleGameplayEvent(*EventMapping, &Payload);
	}
}

void USampleAbilitySystemComponent::NotifyAbilityFailed(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason)
{
	// handle default failure
	Super::NotifyAbilityFailed(Handle, Ability, FailureReason);

	// is this failure being handled remotely?
	if (APawn* Avatar = Cast<APawn>(GetAvatarActor()))
	{
		if (!Avatar->IsLocallyControlled() && Ability->IsSupportedForNetworking())
		{
			// pass the message to the avatar's client
			ClientNotifyAbilityFailed(Ability, FailureReason);
			return;
		}
	}

	// handle ability failure locally
	HandleAbilityFailed(Ability, FailureReason);
}


void USampleAbilitySystemComponent::ClientNotifyAbilityFailed_Implementation(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason)
{
	HandleAbilityFailed(Ability, FailureReason);
}

void USampleAbilitySystemComponent::HandleAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason)
{
	// allow the ability to handle the failure
	if (const USampleGameplayAbility* SampleAbility = Cast<const USampleGameplayAbility>(Ability))
	{
		SampleAbility->OnAbilityFailedToActivate(FailureReason);
	}
}