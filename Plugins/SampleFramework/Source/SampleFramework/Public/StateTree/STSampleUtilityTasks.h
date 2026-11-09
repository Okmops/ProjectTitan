// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/Actor.h"
#include "STSampleUtilityTasks.generated.h"

class AActor;
class UAnimMontage;
class UAnimInstance;

/**
 *  Instance data struct for FStateTreeSpawnActorTask
 */
USTRUCT()
struct SAMPLEFRAMEWORK_API FStateTreeSpawnActorTaskInstanceData
{
	GENERATED_BODY()

	/** Owner of the spawned Actor. */
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> OwningActor = nullptr;

	/** Class of the Actor to spawn */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	TSubclassOf<AActor> ActorClass;

	/** Transform to spawn the Actor at, in world space */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	FTransform SpawnTransform;

	/** Pointer to the spawned actor */
	UPROPERTY(EditAnywhere, Category = "Output")
	TObjectPtr<AActor> SpawnedActor = nullptr;
};

/**
 *  StateTree Task to spawn an Actor on state enter.
 *  The Actor can optionally be destroyed automatically on state exit.
 */
USTRUCT(meta=(DisplayName="Spawn Actor", Category = "Actor"))
struct SAMPLEFRAMEWORK_API FStateTreeSpawnActorTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	/** Constructor */
	FStateTreeSpawnActorTask()
	{
		// no tick needed
		bShouldCallTick = false;

		// skip state change events if this is sustained
		bShouldStateChangeOnReselect = false;
	}

	/** Collision handling method when spawning the actor */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	ESpawnActorCollisionHandlingMethod CollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	/** Scaling method when spawning the actor */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	ESpawnActorScaleMethod ScaleMethod = ESpawnActorScaleMethod::OverrideRootScale;

	/** If true, the spawned Actor will be automatically destroyed when the owning state exits */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	bool bDestroyOnStateExit = true;

	/* Ensure we're using the correct instance data struct */
	using FInstanceDataType = FStateTreeSpawnActorTaskInstanceData;
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
 *  Instance data struct for FStateTreePlayMontageTask
 */
USTRUCT()
struct SAMPLEFRAMEWORK_API FStateTreePlayMontageTaskInstanceData
{
	GENERATED_BODY()

	/** Actor to play the montage on. */
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<AActor> TargetActor = nullptr;

	/** AnimMontage to play. */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	TObjectPtr<UAnimMontage> Montage = nullptr;

	/** Play rate for the Montage */
	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (ClampMin = 0))
	float PlayRate = 1.0f;

	/** Time to start playing the montage at, in seconds */
	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (ClampMin = 0))
	float StartTime = 0.0f;

	/** Resolved AnimInstance that plays the montage, so we can stop it on exit if necessary */
	UPROPERTY()
	TObjectPtr<UAnimInstance> AnimInstance = nullptr;

	/** Set to true if the montage end delegate was successfully bound. Prevents accidentally unbinding another previously set delegate. */
	bool bDelegateBound = false;
};

/**
 *  StateTree Task to play an AnimMontage on an Actor
 *  Succeeds when the montage completes.
 *  Fails when the montage is unable to be played or is cancelled
 */
USTRUCT(meta=(DisplayName="Play Montage on Actor", Category = "Animation"))
struct SAMPLEFRAMEWORK_API FStateTreePlayMontageTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	/** Constructor */
	FStateTreePlayMontageTask()
	{
		// no tick needed
		bShouldCallTick = false;

		// skip state change events if this is sustained
		bShouldStateChangeOnReselect = false;
	}

	/** If true, all other montages on the AnimInstance will be stopped */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	bool bStopAllMontages = true;

	/** If true, the montage will stop playing when the owning state exits */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	bool bStopOnExit = true;

	/** Blend-out time to apply to the montage if it's stopped due to state end */
	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (ClampMin = 0))
	float BlendOutTime = 0.0f;

	/* Ensure we're using the correct instance data struct */
	using FInstanceDataType = FStateTreePlayMontageTaskInstanceData;
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
 *  Instance data struct for FStateTreeWaitForEventTask
 */
USTRUCT()
struct SAMPLEFRAMEWORK_API FStateTreeWaitForEventTaskInstanceData
{
	GENERATED_BODY()

	/** StateTree Event Tag to listen for. */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	FGameplayTag EventTag;

	/** Number of times the Event must be received to succeed the task */
	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (ClampMin = 1))
	int32 TargetCount = 1;

	/** Number of times the Event has been received so far */
	int32 CurrentCount = 0;
};

/**
 *  StateTree Task to listen for a specific StateTree Event
 *  The task succeeds when the StateTree event is received the target number of times.
 */
USTRUCT(meta=(DisplayName="Wait for Event", Category = "StateTree Events"))
struct SAMPLEFRAMEWORK_API FStateTreeWaitForEventTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	/** Constructor */
	FStateTreeWaitForEventTask()
	{
		// tick needed only when there's new events to process
		bShouldCallTick = false;
		bShouldCallTickOnlyOnEvents = true;
		bConsideredForScheduling = false;

		// skip state change events if this is sustained
		bShouldStateChangeOnReselect = false;
	}

	/* Ensure we're using the correct instance data struct */
	using FInstanceDataType = FStateTreeWaitForEventTaskInstanceData;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	/** Runs when the owning state is entered */
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	/** Runs when the owning state receives a tick (only on new events due to flags) */
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

#if WITH_EDITOR

	/** Details panel description*/
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;

	/** Compile-time validations */
	virtual EDataValidationResult Compile(UE::StateTree::ICompileNodeContext& CompileContext) override;

#endif // WITH_EDITOR
};

////////////////////////////////////////////////////////////////////