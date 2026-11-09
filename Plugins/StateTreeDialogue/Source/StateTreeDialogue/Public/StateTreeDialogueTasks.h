// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/Actor.h"
#include "DialogueNode.h"
#include "StateTreeDialogueTasks.generated.h"

class UDialogueWindow;

/**
 *  Instance data struct for StateTreeUpdateDialogueTask
 */
USTRUCT()
struct STATETREEDIALOGUE_API FStateTreeUpdateDialogueTaskInstanceData
{
	GENERATED_BODY()

	/** Pointer to the dialogue window that will be populated. */
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UDialogueWindow> DialogueWindow = nullptr;

	/** Dialogue node containing the information to display */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	FDialogueNode DialogueNode;

};

/**
 *  StateTree Task to populate a dialogue window with the given node values.
 *  Expects a previously created Dialogue Window
 *  Waits on user responses and converts them to StateTree events for the owning component.
 *  Succeeds when all dialogue node pages have been completed.
 *  Fails when the dialogue cancelled message event is received.
 */
USTRUCT(meta=(DisplayName="Update Dialogue Window", Category = "Dialogue System"))
struct STATETREEDIALOGUE_API FStateTreeUpdateDialogueTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	/** Constructor */
	FStateTreeUpdateDialogueTask()
	{
		// no tick needed
		bShouldCallTick = false;

		// skip state change events if this is sustained
		bShouldStateChangeOnReselect = false;
	}

	/* Ensure we're using the correct instance data struct */
	using FInstanceDataType = FStateTreeUpdateDialogueTaskInstanceData;
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
 *  Instance data struct for FStateTreeCreateDialogueTask
 */
USTRUCT()
struct STATETREEDIALOGUE_API FStateTreeCreateDialogueTaskInstanceData
{
	GENERATED_BODY()

	/** PlayerController that handle input for the dialogue window */
	UPROPERTY(EditAnywhere, Category = "Input")
	APlayerController* InteractingPlayer = nullptr;

	/** Pointer to the created dialogue window */
	UPROPERTY(EditAnywhere, Category = "Output")
	TObjectPtr<UDialogueWindow> DialogueWindow = nullptr;
};

/**
 *  StateTree Task to create a dialogue window for a given player.
 *  Succeeds if the window is created.
 *  Fails if the window is not created.
 */
USTRUCT(meta=(DisplayName="Create Dialogue Window", Category = "Dialogue System"))
struct STATETREEDIALOGUE_API FStateTreeCreateDialogueTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	/** Constructor */
	FStateTreeCreateDialogueTask()
	{
		// no tick needed
		bShouldCallTick = false;

		// skip state change events if this is sustained
		bShouldStateChangeOnReselect = false;

	}

	/** Type of dialogue window to create */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	TSubclassOf<UDialogueWindow> DialogueWindowClass;

	/* Ensure we're using the correct instance data struct */
	using FInstanceDataType = FStateTreeCreateDialogueTaskInstanceData;
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
 *  Instance data struct for FStateTreeCloseDialogueTask
 */
USTRUCT()
struct STATETREEDIALOGUE_API FStateTreeCloseDialogueTaskInstanceData
{
	GENERATED_BODY()

	/** Pointer to the created dialogue window */
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UDialogueWindow> DialogueWindow = nullptr;
};

/**
 *  StateTree Task to close a dialogue window on state exit.
 */
USTRUCT(meta=(DisplayName="Close Dialogue Window", Category = "Dialogue System"))
struct STATETREEDIALOGUE_API FStateTreeCloseDialogueTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	/** Constructor */
	FStateTreeCloseDialogueTask()
	{
		// no tick needed
		bShouldCallTick = false;

		// skip state change events if this is sustained
		bShouldStateChangeOnReselect = false;

		#if WITH_EDITORONLY_DATA

		// not considered for state completion
		bConsideredForCompletion = false;

		#endif
	}

	/* Ensure we're using the correct instance data struct */
	using FInstanceDataType = FStateTreeCloseDialogueTaskInstanceData;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	/** Runs when the owning state is ended */
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

#if WITH_EDITOR

	/** Details panel description*/
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;

#endif // WITH_EDITOR
};