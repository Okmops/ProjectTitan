// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "StateTreeExecutionTypes.h"
#include "AbilityTask_RunStateTree.generated.h"

class UStateTree;
class UStateTreeComponent;
class AActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRunStateTreeStatusChangedDelegate, UStateTreeComponent*, RunningComponent);

/**
 *  UAbilityTask_RunStateTree
 *  Runs a StateTree on the provided StateTree component.
 *  Alternatively, creates a new StateTree component and registers it with the provided Actor to run the StateTree.
 *  The task passes the Component running the StateTree to all event execs.
 *  If you do not set bStartImmediately to true, it's your responsibility to call StartLogic on the Component.
 */
UCLASS()
class SAMPLEFRAMEWORK_API UAbilityTask_RunStateTree : public UAbilityTask
{
	GENERATED_BODY()
	
protected:

	/** The StateTree asset to run */
	UPROPERTY()
	TObjectPtr<UStateTree> StateTreeAsset;

	/** The component to run the StateTree on. It can be instanced by this AbilityTask. */
	UPROPERTY()
	TObjectPtr<UStateTreeComponent> StateTreeComponent;

	/** For instanced StateTree comps, the Actor to register the component under, usually the ability owner or avatar. */
	UPROPERTY()
	TObjectPtr<AActor> ComponentOwner;

	/** If true and no StateTreeComponent is passed, a new StateTreeComponent is created to run the StateTree. */
	bool bInstanceComponent = false;

	/** If true, the AbilityTask succeeded at creating a new StateTreeComponent and is responsible for destroying it when it's done. */
	bool bOwnsComponent = false;

	/** If true, StartLogic will be called automatically by the task, otherwise it's the user's responsibility to call it. */
	bool bStartImmediately = false;

public:

	/** Delegate called when the StateTree is initialized and ready to run but before it starts. Useful if we want to start the StateTree manually or change its properties before it runs */
	UPROPERTY(BlueprintAssignable)
	FRunStateTreeStatusChangedDelegate OnReady;

	/** Status change delegate called if the StateTree succeeds */
	UPROPERTY(BlueprintAssignable)
	FRunStateTreeStatusChangedDelegate OnSucceeded;

	/** Status change delegate called if the StateTree fails */
	UPROPERTY(BlueprintAssignable)
	FRunStateTreeStatusChangedDelegate OnFailed;

	/** Status change delegate called if the StateTree starts running */
	UPROPERTY(BlueprintAssignable)
	FRunStateTreeStatusChangedDelegate OnRunning;

	/** Status change delegate called if the StateTree stops */
	UPROPERTY(BlueprintAssignable)
	FRunStateTreeStatusChangedDelegate OnStopped;

public:

	/** Runs a StateTree on the provided StateTree component. If no component is provided and bCreateNewComponent is true, a new StateTree component will be created on the provided owning Actor to run it. */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UAbilityTask_RunStateTree* RunStateTree(UGameplayAbility* OwningAbility, FName TaskInstanceName, UStateTree* StateTree, UStateTreeComponent* Component, bool bCreateNewComponent = true, AActor* NewComponentOwner = nullptr, bool bStartImmediately = true);

	/** Initiates the async task */
	virtual void Activate() override;
	
	/** Cleans up the ability task on destruction */
	virtual void OnDestroy(bool AbilityIsEnding) override;

protected:

	UFUNCTION()
	void OnStateTreeStatusChanged(EStateTreeRunStatus RunStatus);
};
