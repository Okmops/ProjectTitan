// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilityTasks/AbilityTask_RunStateTree.h"
#include "StateTree.h"
#include "Components/StateTreeComponent.h"
#include "SampleLogging.h"
#include "AbilitySystemComponent.h"

UAbilityTask_RunStateTree* UAbilityTask_RunStateTree::RunStateTree(UGameplayAbility* OwningAbility, FName TaskInstanceName, UStateTree* InStateTree, UStateTreeComponent* InComponent, bool bCreateNewComponentInstance, AActor* NewComponentOwner, bool bInStartImmediately)
{
	// create the task instance
	UAbilityTask_RunStateTree* MyObj = NewAbilityTask<UAbilityTask_RunStateTree>(OwningAbility, TaskInstanceName);

	// save the property values
	MyObj->StateTreeAsset = InStateTree;
	MyObj->StateTreeComponent = InComponent;
	MyObj->ComponentOwner = NewComponentOwner;
	MyObj->bInstanceComponent = bCreateNewComponentInstance;
	MyObj->bStartImmediately = bInStartImmediately;

	return MyObj;
}

void UAbilityTask_RunStateTree::Activate()
{
	Super::Activate();

	// ensure the StateTree asset is valid
	if (!StateTreeAsset)
	{
		UE_LOG(LogSampleAbilitySystem, Error, TEXT("[%s]: Invalid StateTree asset."), *GetName());

		EndTask();
		return;
	}

	// do we have a valid component to run the StateTree on?
	if (!IsValid(StateTreeComponent))
	{
		// should we create a new component instance to run the StateTree?
		if (bInstanceComponent)
		{
			// ensure we have a valid owner for the newly instanced component
			if (!IsValid(ComponentOwner))
			{
				// no ComponentOwner, so default to the ASC's owner and warn
				ComponentOwner = AbilitySystemComponent->GetOwnerActor();

				UE_LOG(LogSampleAbilitySystem, Warning, TEXT("[%s]: No Actor Owner provided for the new StateTreeComponent instance. Will default to the ASC's owner."), *GetName());

				// if there's no valid owner still, fail the task
				if (!IsValid(ComponentOwner))
				{
					UE_LOG(LogSampleAbilitySystem, Error, TEXT("[%s]: ASC's owner is not valid, the instanced StateTree component cannot be created."), *GetName());

					EndTask();
					return;
				}
			}

			// create the new StateTree component instance
			StateTreeComponent = Cast<UStateTreeComponent>(ComponentOwner->AddComponentByClass(UStateTreeComponent::StaticClass(), false, FTransform::Identity, false));
			
			// ensure the component was actually created
			if (!StateTreeComponent)
			{
				UE_LOG(LogSampleAbilitySystem, Error, TEXT("[%s]: Failed to create StateTree Component."), *GetName());

				EndTask();
				return;
			}

			// raise the owns component flag so we remember to destroy it after we're done
			bOwnsComponent = true;

		}
		else
		{
			// we're not supposed to instance a new component, but there's no existing component to run the StateTree on
			UE_LOG(LogSampleAbilitySystem, Error, TEXT("[%s]: Invalid StateTree Component, and bInstanceComponent is false."), *GetName());

			EndTask();
			return;
		}
	}

	// ensure the StateTree is stopped before we mess with it
	StateTreeComponent->StopLogic("Stopped by AbilityTask_RunStateTree");

	// subscribe to status changed events on the StateTree
	StateTreeComponent->OnStateTreeRunStatusChanged.AddDynamic(this, &UAbilityTask_RunStateTree::OnStateTreeStatusChanged);

	// set the StateTree
	StateTreeComponent->SetStateTree(StateTreeAsset);

	// call the ready delegate
	OnReady.Broadcast(StateTreeComponent);

	// run the StateTree if we're set to start immediately
	if (bStartImmediately)
	{
		StateTreeComponent->StartLogic();
	}
}

void UAbilityTask_RunStateTree::OnDestroy(bool AbilityIsEnding)
{
	if (StateTreeComponent)
	{
		// unsubscribe from status changed events
		StateTreeComponent->OnStateTreeRunStatusChanged.RemoveDynamic(this, &UAbilityTask_RunStateTree::OnStateTreeStatusChanged);

		// stop the StateTree
		if (StateTreeComponent->IsRunning())
		{
			StateTreeComponent->StopLogic("AbilityTask_RunStateTree Destroyed");
		}

		// if this is an instanced component, destroy it
		if (bOwnsComponent && IsValid(ComponentOwner))
		{
			StateTreeComponent->DestroyComponent(false);
			StateTreeComponent = nullptr;
			bOwnsComponent = false;
		}
	}

	Super::OnDestroy(AbilityIsEnding);
}

void UAbilityTask_RunStateTree::OnStateTreeStatusChanged(EStateTreeRunStatus RunStatus)
{
	// call the corresponding delegate based on the status change
	switch (RunStatus)
	{
		case EStateTreeRunStatus::Failed:
			OnFailed.Broadcast(StateTreeComponent);
			EndTask();
			break;
		case EStateTreeRunStatus::Running:
			OnRunning.Broadcast(StateTreeComponent);
			break;
		case EStateTreeRunStatus::Stopped:
			OnStopped.Broadcast(StateTreeComponent);
			EndTask();
			break;
		case EStateTreeRunStatus::Succeeded:
			OnSucceeded.Broadcast(StateTreeComponent);
			EndTask();
			break;
	}
}
