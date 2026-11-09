// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "GameplayTagContainer.h"
#include "QuestComponent.h"
#include "STQuestTasks.generated.h"

/**
 *  Instance data struct for FStateTreeStartQuestTag
 */
USTRUCT()
struct QUESTSYSTEM_API FStateTreeQuestInstanceData
{
	GENERATED_BODY()

	/** Actor holding the Quest Manager Component to affect. */
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> TargetActor = nullptr;

	/** Tag of the Quest to affect. */
	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (Categories = "QuestSystem.Quest"))
	FGameplayTag QuestTag;
};

/**
 *  StateTree Task to start a Quest on the Target Actor
 *  Assumes the Target Actor to have a Quest Manager Component
 *  Succeeds if the Quest is successfully started, fails otherwise.
 */
USTRUCT(meta=(DisplayName="Start Quest", Category = "Quest"))
struct QUESTSYSTEM_API FStateTreeStartQuestTag : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	/** Constructor */
	FStateTreeStartQuestTag()
	{
		// no tick needed
		bShouldCallTick = false;

		// skip state change events if this is sustained
		bShouldStateChangeOnReselect = false;
	}

	/* Ensure we're using the correct instance data struct */
	using FInstanceDataType = FStateTreeQuestInstanceData;
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

/**
 *  StateTree Task to finish a Quest for the Target Actor
 *  The quest can be flagged as Completed or Failed
 *  Assumes the Target Actor to have a Quest Manager Component
 *  Succeeds if the Quest is successfully finished, fails otherwise.
 */
USTRUCT(meta=(DisplayName="Finish Quest", Category = "Quest"))
struct QUESTSYSTEM_API FStateTreeFinishQuestTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	/** Constructor */
	FStateTreeFinishQuestTask()
	{
		// no tick needed
		bShouldCallTick = false;

		// skip state change events if this is sustained
		bShouldStateChangeOnReselect = false;
	}

	/** If true, the quest is finished as Failed, otherwise it's finished as Completed */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	bool bFailQuest = false;

	/* Ensure we're using the correct instance data struct */
	using FInstanceDataType = FStateTreeQuestInstanceData;
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
 *  Instance data struct for FStateTreeSendQuestUpdateTask
 */
USTRUCT()
struct QUESTSYSTEM_API FStateTreeSendQuestUpdateTaskInstanceData
{
	GENERATED_BODY()

	/** Actor holding the Quest Manager Component to affect. */
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> TargetActor = nullptr;

	/** Tag of the Quest to affect. */
	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (Categories = "QuestSystem.Quest"))
	FGameplayTag QuestTag;

	/** Tag of the Quest Step to affect. */
	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (Categories = "QuestSystem.Quest"))
	FGameplayTag QuestStepTag;

	/** Optional Data to send along with the update. */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	FQuestUpdateData OptionalData;
};

/**
 *  StateTree Task to send a Quest Update message to the Target Actor
 *  Assumes the Target Actor to have a Quest Manager Component
 *  Succeeds if the Quest Update is successfully sent, fails otherwise.
 */
USTRUCT(meta=(DisplayName="Send Quest Update", Category = "Quest"))
struct QUESTSYSTEM_API FStateTreeSendQuestUpdateTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	/** Constructor */
	FStateTreeSendQuestUpdateTask()
	{
		// no tick needed
		bShouldCallTick = false;

		// skip state change events if this is sustained
		bShouldStateChangeOnReselect = false;
	}

	/* Ensure we're using the correct instance data struct */
	using FInstanceDataType = FStateTreeSendQuestUpdateTaskInstanceData;
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
 *  Instance data struct for FStateTreeQuestStatusTagTask
 */
USTRUCT()
struct QUESTSYSTEM_API FStateTreeQuestStatusTagTaskInstanceData
{
	GENERATED_BODY()

	/** Actor holding the Quest Manager Component to affect. */
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> TargetActor = nullptr;

	/** Quest Status Tag to affect. */
	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (Categories = "QuestSystem.Quest"))
	FGameplayTag QuestStatusTag;
};

/**
 *  StateTree Task to add or remove a Quest Status Tag on the Target Actor
 *  Assumes the Target Actor to have a Quest Manager Component
 *  Succeeds if the Tag is successfully changed, fails otherwise.
 */
USTRUCT(meta=(DisplayName="Change Quest Status Tag", Category = "Quest"))
struct QUESTSYSTEM_API FStateTreeQuestStatusTagTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	/** Constructor */
	FStateTreeQuestStatusTagTask()
	{
		// no tick needed
		bShouldCallTick = false;

		// skip state change events if this is sustained
		bShouldStateChangeOnReselect = false;
	}

	/** If true, the status tag will be removed, otherwise it will be added. */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	bool bRemoveTag = false;

	/* Ensure we're using the correct instance data struct */
	using FInstanceDataType = FStateTreeQuestStatusTagTaskInstanceData;
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
 *  Instance data struct for FStateTreeSendQuestEventTask
 */
USTRUCT()
struct QUESTSYSTEM_API FStateTreeSendQuestEventTaskInstanceData
{
	GENERATED_BODY()

	/** Actor holding the Quest Manager Component to affect. */
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> TargetActor = nullptr;

	/** Quest Event to send on state Enter. */
	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (Categories = "QuestSystem.Event"))
	FGameplayTag EnterEventTag;

	/** Quest Event to send on state Exit. */
	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (Categories = "QuestSystem.Event"))
	FGameplayTag ExitEventTag;

	/** Optional data payload to send with the event. */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	FQuestEventPayload Payload;
};

/**
 *  StateTree Task to send a Quest Event to the Target Actor
 *  Supports separate Enter and Exit events. At least one is mandatory. Both share the same payload. 
 *  Assumes the Target Actor to have a Quest Manager Component
 *  Succeeds if the event is successfully sent, fails otherwise.
 */
USTRUCT(meta=(DisplayName="Send Quest Event", Category = "Quest"))
struct QUESTSYSTEM_API FStateTreeSendQuestEventTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	/** Constructor */
	FStateTreeSendQuestEventTask()
	{
		// no tick needed
		bShouldCallTick = false;

		// skip state change events if this is sustained
		bShouldStateChangeOnReselect = false;

		#if WITH_EDITORONLY_DATA

		// do not consider for completion
		bConsideredForCompletion = false;

		#endif
	}

	/* Ensure we're using the correct instance data struct */
	using FInstanceDataType = FStateTreeSendQuestEventTaskInstanceData;
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
 *  Instance data struct for FStateTreeChangeQuestQuantityTask
 */
USTRUCT()
struct QUESTSYSTEM_API FStateTreeChangeQuestQuantityTaskInstanceData
{
	GENERATED_BODY()

	/** Actor holding the Quest Manager Component to affect. */
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> TargetActor = nullptr;

	/** Quest Quantity Tag to affect. */
	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (Categories = "QuestSystem.Quest"))
	FGameplayTag QuestQuantityTag;

	/** Amount to add or subtract from the quantity */
	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (Categories = "QuestSystem.Quest"))
	int32 CountDelta = 1;
};

/**
 *  StateTree Task to increase or decrease a Quest Quantity on the Target Actor
 *  Assumes the Target Actor to have a Quest Manager Component
 *  Succeeds if the quantity is successfully changed, fails otherwise.
 */
USTRUCT(meta=(DisplayName="Change Quest Quantity", Category = "Quest"))
struct QUESTSYSTEM_API FStateTreeChangeQuestQuantityTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	/** Constructor */
	FStateTreeChangeQuestQuantityTask()
	{
		// no tick needed
		bShouldCallTick = false;

		// skip state change events if this is sustained
		bShouldStateChangeOnReselect = false;
	}

	/* Ensure we're using the correct instance data struct */
	using FInstanceDataType = FStateTreeChangeQuestQuantityTaskInstanceData;
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
 *  Instance data struct for FStateTreeWaitQuestQuantityTask
 */
USTRUCT()
struct QUESTSYSTEM_API FStateTreeWaitQuestQuantityTaskInstanceData
{
	GENERATED_BODY()

	/** Actor holding the Quest Manager Component to affect. */
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> TargetActor = nullptr;

	/** Quest Quantity Tag to affect. */
	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (Categories = "QuestSystem.Quest"))
	FGameplayTag QuestQuantityTag;

	/** Minimum value for the Quantity to succeed the task. */
	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (Categories = "QuestSystem.Quest", ClampMin = 1))
	int32 TargetValue = 1;
};

/**
 *  StateTree Task to check if a specified Quest Quantity meets a target value
 *  Assumes the Target Actor to have a Quest Manager Component
 *  Succeeds if the quantity is at least the target value, fails otherwise.
 */
USTRUCT(meta=(DisplayName="Check Quest Quantity", Category = "Quest"))
struct QUESTSYSTEM_API FStateTreeWaitQuestQuantityTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	/** Constructor */
	FStateTreeWaitQuestQuantityTask()
	{
		// no tick needed
		bShouldCallTick = false;

		// skip state change events if this is sustained
		bShouldStateChangeOnReselect = false;
	}

	/* Ensure we're using the correct instance data struct */
	using FInstanceDataType = FStateTreeWaitQuestQuantityTaskInstanceData;
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