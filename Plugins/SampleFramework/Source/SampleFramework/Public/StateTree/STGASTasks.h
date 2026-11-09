// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayAbilitySpecHandle.h"
#include "GAS/AbilitySet.h"
#include "STGASTasks.generated.h"

class UGameplayEffect;
class UGameplayAbility;

/**
 *  Instance data struct for FStateTreeGameplayEventTask
 */
USTRUCT()
struct SAMPLEFRAMEWORK_API FStateTreeGameplayEventTaskInstanceData
{
	GENERATED_BODY()

	/** Actor that will receive the event. The Task expects the actor to implement IAbilitySystemInterface and provide a valid Ability System Component. */
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> TargetActor = nullptr;

	/** Event Instigator. */
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> EventInstigator = nullptr;

	/** Additional tags associated with the event instigator. */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	FGameplayTagContainer InstigatorTags;

	/** Magnitude of the event. */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	float EventMagnitude = 0.0f;

	/** Optional object reference to send with the event. */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	TObjectPtr<UObject> OptionalObject = nullptr;

	/** Second Optional object reference to send with the event. */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	TObjectPtr<UObject> OptionalObject2 = nullptr;
};

/**
 *  StateTree Task to send Gameplay Events to the Ability System Component associated with the target Actor.
 *  The task supports events on Enter, Exit and Tick. Events will only be sent if the associated tags are not empty.
 */
USTRUCT(meta=(DisplayName="Send Gameplay Event", Category = "GAS"))
struct SAMPLEFRAMEWORK_API FStateTreeGameplayEventTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	/** Constructor */
	FStateTreeGameplayEventTask()
	{
		// tick needed
		bShouldCallTick = true;

		// skip state change events if this is sustained
		bShouldStateChangeOnReselect = false;
	}

	/** Tag identifying the gameplay event to be sent on EnterState. The event will only be sent if the tag is not empty. */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	FGameplayTag EnterEventTag;

	/** Tag identifying the gameplay event to be sent on ExitState. The event will only be sent if the tag is not empty. */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	FGameplayTag ExitEventTag;

	/** Tag identifying the gameplay event to be sent on StateTree tick. The event will only be sent if the tag is not empty. Note that the tick rate may not be the same as the Actor's */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	FGameplayTag TickEventTag;

	/* Ensure we're using the correct instance data struct */
	using FInstanceDataType = FStateTreeGameplayEventTaskInstanceData;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	/** Runs when the owning state is entered */
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	/** Runs when the owning state is ended */
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	/** Runs while the owning state is active and ticking */
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

#if WITH_EDITOR

	/** Details panel description*/
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;

	/** Compile-time validations */
	virtual EDataValidationResult Compile(UE::StateTree::ICompileNodeContext& CompileContext) override;

#endif // WITH_EDITOR
};

////////////////////////////////////////////////////////////////////

/**
 *  Encapsulates data for a Gameplay Effect's Set by Caller Magnitude.
 */
USTRUCT()
struct SAMPLEFRAMEWORK_API FStateTreeSetByCallerMagnitude
{
	GENERATED_BODY()

	/** Tag to identify the Set by Caller Magnitude */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	FGameplayTag Tag;

	/** Magnitude value */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	float Magnitude = 0.0f;
};

/**
 *  Instance data struct for FStateTreeGameplayEffectTask
 */
USTRUCT()
struct SAMPLEFRAMEWORK_API FStateTreeGameplayEffectTaskInstanceData
{
	GENERATED_BODY()

	/** Actor that will receive the gameplay effect. The Task expects the actor to implement IAbilitySystemInterface and provide a valid Ability System Component. */
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> TargetActor = nullptr;

	/** Gameplay Effect to grant */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	TSubclassOf<UGameplayEffect> GameplayEffect;

	/** Level to apply the Gameplay Effect at */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	float Level = 1.0f;

	/** Number of stacks of the effect to apply */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	int32 Stacks = 1;

	/** Optional Set By Caller Magnitudes to pass to the Gameplay Effect */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	TArray<FStateTreeSetByCallerMagnitude> SetByCallerMagnitudes;

	/** If true, the effect will be automatically removed when the state ends. Tick this off for one-shot effects such as damage. */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	bool bRemoveOnStateEnd = true;

	/** Active gameplay effect handle received by the GE application, so we can remove it when the state ends. */
	FActiveGameplayEffectHandle ActiveEffectHandle;
};

/**
 *  StateTree Task to grant a Gameplay Effect to the Ability System Component associated with the target Actor.
 *  The effect can be removed automatically when the owning state ends.
 */
USTRUCT(meta=(DisplayName="Grant Gameplay Effect", Category = "GAS"))
struct SAMPLEFRAMEWORK_API FStateTreeGameplayEffectTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	/** Constructor */
	FStateTreeGameplayEffectTask()
	{
		// no tick needed
		bShouldCallTick = false;

		// skip state change events if this is sustained
		bShouldStateChangeOnReselect = false;
	}

	/* Ensure we're using the correct instance data struct */
	using FInstanceDataType = FStateTreeGameplayEffectTaskInstanceData;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	/** Runs when the owning state is entered */
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	/** Runs when the owning state is ended */
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

#if WITH_EDITOR

	/** Details panel description*/
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;

	/** Compile-time validations */
	virtual EDataValidationResult Compile(UE::StateTree::ICompileNodeContext& CompileContext) override;

#endif // WITH_EDITOR
};

////////////////////////////////////////////////////////////////////

/**
 *  Instance data struct for FStateTreeGameplayCueTask
 */
USTRUCT()
struct SAMPLEFRAMEWORK_API FStateTreeGameplayCueTaskInstanceData
{
	GENERATED_BODY()

	/** Actor that will receive the Gameplay Cue trigger. The Task expects the actor to implement IAbilitySystemInterface and provide a valid Ability System Component. */
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> TargetActor = nullptr;

	/** Tag of the Gameplay Cue to trigger */
	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (Categories="GameplayCue"))
	FGameplayTag GameplayCue;

	/** Optional causer of the Gameplay Cue */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	TObjectPtr<AActor> EffectCauser = nullptr;

	/** AbilityLevel of the Gameplay Cue on the passed Gameplay Cue Params */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	int32 AbilityLevel = 1;

	/** RawMagnitude of the Gameplay Cue on the passed Gameplay Cue Params */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	float RawMagnitude = 1.0f;

	/** Location of the Gameplay Cue on the passed Gameplay Cue Params */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	FVector Location = FVector::ZeroVector;

	/** Normal of the Gameplay Cue on the passed Gameplay Cue Params */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	FVector Normal = FVector::UpVector;
};

/**
 *  StateTree Task to trigger a one-shot Gameplay Cue on state Enter.
 *  Supports some Gameplay Cue parameters to be passed to the handler.
 *  The Cue's Instigator is set to the TargetActor.
 *  If you need persisting Gameplay Cues, trigger it through a persisting Gameplay Effect instead.
 */
USTRUCT(meta=(DisplayName="Trigger Gameplay Cue", Category = "GAS"))
struct SAMPLEFRAMEWORK_API FStateTreeGameplayCueTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	/** Constructor */
	FStateTreeGameplayCueTask()
	{
		// no tick needed
		bShouldCallTick = false;

		// skip state change events if this is sustained
		bShouldStateChangeOnReselect = false;

		#if WITH_EDITORONLY_DATA

		// do not consider this task for state completion by default
		bConsideredForCompletion = false;

		#endif 
	}

	/* Ensure we're using the correct instance data struct */
	using FInstanceDataType = FStateTreeGameplayCueTaskInstanceData;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	/** Runs when the owning state is entered */
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

#if WITH_EDITOR

	/** Details panel description*/
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;

	/** Compile-time validations */
	virtual EDataValidationResult Compile(UE::StateTree::ICompileNodeContext& CompileContext) override;

#endif // WITH_EDITOR
};

////////////////////////////////////////////////////////////////////

/**
 *  Instance data struct for FStateTreeAbilitySetTask
 */
USTRUCT()
struct SAMPLEFRAMEWORK_API FStateTreeAbilitySetTaskInstanceData
{
	GENERATED_BODY()

	/** Actor that will be granted the Ability Set. The Task expects the actor to implement IAbilitySystemInterface and provide a valid Ability System Component. */
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> TargetActor = nullptr;

	/** AbilitySet granted handles, so we can remove them on exit if needed. */
	FAbilitySet_GrantedHandles GrantedHandles;
};

/**
 *  StateTree Task to grant an Ability Set to an Actor when the owning State enters
 *  The Ability Set can be removed automatically when the owning State exits
 */
USTRUCT(meta=(DisplayName="Grant Ability Set", Category = "GAS"))
struct SAMPLEFRAMEWORK_API FStateTreeAbilitySetTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	/** Constructor */
	FStateTreeAbilitySetTask()
	{
		// no tick needed
		bShouldCallTick = false;

		// skip state change events if this is sustained
		bShouldStateChangeOnReselect = false;
	}

	/** AbilitySet that will be granted. */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	TObjectPtr<UAbilitySet> AbilitySet = nullptr;

	/** If true, the AbilitySet will be automatically removed when the owning State exits */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	bool bRemoveOnStateExit = true;

	/* Ensure we're using the correct instance data struct */
	using FInstanceDataType = FStateTreeAbilitySetTaskInstanceData;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	/** Runs when the owning state is entered */
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	/** Runs when the owning state is ended */
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

#if WITH_EDITOR

	/** Details panel description*/
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;

	/** Compile-time validations */
	virtual EDataValidationResult Compile(UE::StateTree::ICompileNodeContext& CompileContext) override;

#endif // WITH_EDITOR
};

////////////////////////////////////////////////////////////////////

/**
 *  Instance data struct for FStateTreeGiveAndActivateAbilityTask
 */
USTRUCT()
struct SAMPLEFRAMEWORK_API FStateTreeGiveAndActivateAbilityTaskInstanceData
{
	GENERATED_BODY()

	/** Actor that will be granted the Ability. The Task expects the actor to implement IAbilitySystemInterface and provide a valid Ability System Component. */
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> TargetActor = nullptr;

	/** Ability that will be granted and activated. */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	TSubclassOf<UGameplayAbility> Ability = nullptr;

	/** Level to grant the ability at. */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	int32 Level = 1;

	/** If true, the Ability will be automatically cancelled when the owning State exits */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	bool bCancelAbilityOnStateExit = true;

	/** Ability Spec Handle so we can cancel it later if needed */
	FGameplayAbilitySpecHandle AbilityHandle;
};

/**
 *  StateTree Task to grant and activate a Gameplay Ability when the owning State enters.
 *  The Gameplay Ability will be automatically removed when it ends. NOTE: this will not complete this task!
 *  The Gameplay Ability can also be automatically cancelled when the owning State exits
 */
USTRUCT(meta=(DisplayName="Give and Activate Ability", Category = "GAS"))
struct SAMPLEFRAMEWORK_API FStateTreeGiveAndActivateAbilityTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	/** Constructor */
	FStateTreeGiveAndActivateAbilityTask()
	{
		// no tick needed
		bShouldCallTick = false;

		// skip state change events if this is sustained
		bShouldStateChangeOnReselect = false;
	}

	/* Ensure we're using the correct instance data struct */
	using FInstanceDataType = FStateTreeGiveAndActivateAbilityTaskInstanceData;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	/** Runs when the owning state is entered */
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	/** Runs when the owning state is ended */
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

#if WITH_EDITOR

	/** Details panel description*/
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;

	/** Compile-time validations */
	virtual EDataValidationResult Compile(UE::StateTree::ICompileNodeContext& CompileContext) override;

#endif // WITH_EDITOR
};

////////////////////////////////////////////////////////////////////

/**
 *  Instance data struct for FStateTreeActivateAbilityByClassTask
 */
USTRUCT()
struct SAMPLEFRAMEWORK_API FStateTreeActivateAbilityByClassTaskInstanceData
{
	GENERATED_BODY()

	/** Actor that the Ability will be activated on. The Task expects the actor to implement IAbilitySystemInterface and provide a valid Ability System Component. */
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> TargetActor = nullptr;

	/** Ability that will be activated. */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	TSubclassOf<UGameplayAbility> Ability = nullptr;

	/** If true, the Ability can be remotely activated from Clients, otherwise it's server-only. */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	bool bAllowRemoteActivation = false;
};

/**
 *  StateTree Task to activate a Gameplay Ability when the owning State enters.
 *  The Gameplay Ability is identified by its Class.
 *  It is not granted, and will not activate if it's not present in the ASC.
 *  Succeeds if the ability is activated, fails otherwise. Does not wait for the ability to end.
 */
USTRUCT(meta=(DisplayName="Activate Ability by Class", Category = "GAS"))
struct SAMPLEFRAMEWORK_API FStateTreeActivateAbilityByClassTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	/** Constructor */
	FStateTreeActivateAbilityByClassTask()
	{
		// no tick needed
		bShouldCallTick = false;

		// skip state change events if this is sustained
		bShouldStateChangeOnReselect = false;
	}

	/* Ensure we're using the correct instance data struct */
	using FInstanceDataType = FStateTreeActivateAbilityByClassTaskInstanceData;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	/** Runs when the owning state is entered */
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

#if WITH_EDITOR

	/** Details panel description*/
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;

	/** Compile-time validations */
	virtual EDataValidationResult Compile(UE::StateTree::ICompileNodeContext& CompileContext) override;

#endif // WITH_EDITOR
};

////////////////////////////////////////////////////////////////////

/**
 *  Instance data struct for FStateTreeActivateAbilitiesByTagTask
 */
USTRUCT()
struct SAMPLEFRAMEWORK_API FStateTreeActivateAbilitiesByTagTaskInstanceData
{
	GENERATED_BODY()

	/** Actor that the Ability will be activated on. The Task expects the actor to implement IAbilitySystemInterface and provide a valid Ability System Component. */
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> TargetActor = nullptr;

	/** Activation Tags for the Abilities to activate. */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	FGameplayTagContainer ActivationTags;

	/** If true, the Ability can be remotely activated from Clients, otherwise it's server-only. */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	bool bAllowRemoteActivation = false;
};

/**
 *  StateTree Task to activate Gameplay Abilities when the owning State enters.
 *  The Gameplay Abilities are identified by its Activation Tag. All matching abilities are activated.
 *  It is not granted, and will not activate if it's not present in the ASC.
 *  Succeeds if at least one ability was activated, fails otherwise. Does not wait for abilities to end.
 */
USTRUCT(meta=(DisplayName="Activate Abilities by Tag", Category = "GAS"))
struct SAMPLEFRAMEWORK_API FStateTreeActivateAbilitiesByTagTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	/** Constructor */
	FStateTreeActivateAbilitiesByTagTask()
	{
		// no tick needed
		bShouldCallTick = false;

		// skip state change events if this is sustained
		bShouldStateChangeOnReselect = false;
	}

	/* Ensure we're using the correct instance data struct */
	using FInstanceDataType = FStateTreeActivateAbilitiesByTagTaskInstanceData;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	/** Runs when the owning state is entered */
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

#if WITH_EDITOR

	/** Details panel description*/
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;

	/** Compile-time validations */
	virtual EDataValidationResult Compile(UE::StateTree::ICompileNodeContext& CompileContext) override;

#endif // WITH_EDITOR
};