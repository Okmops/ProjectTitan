// Copyright Epic Games, Inc. All Rights Reserved.


#include "StateTree/STSampleUtilityTasks.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeAsyncExecutionContext.h"
#include "SampleLogging.h"
#include "Engine/World.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"

#define LOCTEXT_NAMESPACE "StateTreeSampleUtilityTasks"

EStateTreeRunStatus FStateTreeSpawnActorTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	// get the instance data
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// skip if the owning actor is not valid
	if (!IsValid(InstanceData.OwningActor))
	{
		UE_LOG(LogSampleStateTree, Error, TEXT("FStateTreeSpawnActorTask: Attempted to spawn with an invalid owner."));
		return EStateTreeRunStatus::Failed;
	}

	// skip if the spawned actor class is not valid
	if (!InstanceData.ActorClass)
	{
		UE_LOG(LogSampleStateTree, Error, TEXT("FStateTreeSpawnActorTask: Attempted to spawn an invalid Actor class."));
		return EStateTreeRunStatus::Failed;
	}

	// set the spawn parameters
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = InstanceData.OwningActor;
	SpawnParams.SpawnCollisionHandlingOverride = CollisionHandlingMethod;
	SpawnParams.TransformScaleMethod = ScaleMethod;

	// clear the output pointer in case spawn fails
	InstanceData.SpawnedActor = nullptr;

	// spawn the actor
	if (UWorld* MyWorld = InstanceData.OwningActor->GetWorld())
	{
		InstanceData.SpawnedActor = MyWorld->SpawnActor<AActor>(InstanceData.ActorClass, InstanceData.SpawnTransform, SpawnParams);
	}

	// if spawning was unsuccessful, fail the task
	if (!InstanceData.SpawnedActor)
	{
		UE_LOG(LogSampleStateTree, Warning, TEXT("FStateTreeSpawnActorTask: Failed to spawn Actor of Class [%s]."), *InstanceData.ActorClass->GetName());
		return EStateTreeRunStatus::Failed;
	}

	// the task runs indefinitely so it can remove the actor on state change if necessary
	return EStateTreeRunStatus::Running;
}

void FStateTreeSpawnActorTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	// get the instance data
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// should we destroy the spawned actor?
	if (bDestroyOnStateExit && IsValid(InstanceData.SpawnedActor))
	{
		InstanceData.SpawnedActor->Destroy();
		InstanceData.SpawnedActor = nullptr;
	}
}

#if WITH_EDITOR

FText FStateTreeSpawnActorTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting /*= EStateTreeNodeFormatting::Text*/) const
{
	// get the instance data
	const FInstanceDataType* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	check(InstanceData);

	// get the name of the owner
	FText OwnerText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, OwningActor)), Formatting);
	if (OwnerText.IsEmpty())
	{
		return LOCTEXT("SpawnActorTaskInvalidOwner", "<b>WARNING: Invalid Owner. Check the Owning Actor Binding.</>");
	}

	// get the name of the actor class
	FText ClassText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, ActorClass)), Formatting);
	if (ClassText.IsEmpty())
	{
		if (InstanceData->ActorClass)
		{
			ClassText = InstanceData->ActorClass->GetDisplayNameText();
		}
		else
		{
			return LOCTEXT("SpawnActorTaskInvalidClass", "<b>WARNING: Invalid Actor Class. Check the Actor Class Binding.</>");
		}
	}

	FTextBuilder OutText;

	OutText.AppendLine(FText::Format(LOCTEXT("SpawnActorTaskSpawn", "Spawn Actor of Class <b>{0}</> with Owner <b>{1}</>"), ClassText, OwnerText));

	// get the transform binding
	FText TransformText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, SpawnTransform)), Formatting);
	if (TransformText.IsEmpty())
	{
		// if there's no binding, output the transform as three lines for better readability
		OutText.AppendLine(FText::Format(LOCTEXT("SpawnActorTaskLoc", "<s>Loc: </> {0}"), FText::FromString(InstanceData->SpawnTransform.GetLocation().ToCompactString())));
		OutText.AppendLine(FText::Format(LOCTEXT("SpawnActorTaskRot", "<s>Rot: </> {0}"), FText::FromString(InstanceData->SpawnTransform.GetRotation().ToString())));
		OutText.AppendLine(FText::Format(LOCTEXT("SpawnActorTaskScale", "<s>Scale: </> {0}"), FText::FromString(InstanceData->SpawnTransform.GetScale3D().ToCompactString())));
	}
	else
	{

		OutText.AppendLine(FText::Format(LOCTEXT("SpawnActorTaskTransform", "<s>Transform: </> {0}"), TransformText));

	}

	// get the collision method binding
	FText CollisionText = FText::FromString(UEnum::GetValueAsString(CollisionHandlingMethod));

	OutText.AppendLine(FText::Format(LOCTEXT("SpawnActorTaskCollision", "<s>Collision Method: </> {0}"), CollisionText));

	// get the scaling method binding
	FText ScaleText = FText::FromString(UEnum::GetValueAsString(ScaleMethod));

	OutText.AppendLine(FText::Format(LOCTEXT("SpawnActorTaskScaleMethod", "<s>Scaling Method: </> {0}"), ScaleText));

	OutText.AppendLine(bDestroyOnStateExit ? LOCTEXT("SpawnActorTaskDestroyOnExit", "<s>Destroy on State Exit</>") : LOCTEXT("SpawnActorTaskNoDestroyOnExit", "<s>Do Not Destroy on State Exit</>"));
	
	return OutText.ToText();
}

EDataValidationResult FStateTreeSpawnActorTask::Compile(UE::StateTree::ICompileNodeContext& CompileContext)
{
	const FInstanceDataType& InstanceData = CompileContext.GetInstanceDataView().Get<FInstanceDataType>();

	EDataValidationResult Result = EDataValidationResult::Valid;

	// ensure the actor class is valid
	if (!InstanceData.ActorClass && !CompileContext.HasBindingForProperty(GET_MEMBER_NAME_CHECKED(FInstanceDataType, ActorClass)))
	{
		CompileContext.AddValidationError(LOCTEXT("SpawnActorTaskErrorInvalidClass", "Invalid Actor Class. Check the Actor Class Binding."));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

#endif // WITH_EDITOR

EStateTreeRunStatus FStateTreePlayMontageTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	// get the instance data
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// skip if the montage is not valid
	if (!InstanceData.Montage)
	{
		UE_LOG(LogSampleStateTree, Error, TEXT("FStateTreePlayMontageTask: Attempted to play an invalid montage."));
		return EStateTreeRunStatus::Failed;
	}

	// skip if the owning actor is not valid
	if (!IsValid(InstanceData.TargetActor))
	{
		UE_LOG(LogSampleStateTree, Error, TEXT("FStateTreePlayMontageTask: Attempted to play montage on invalid actor."));
		return EStateTreeRunStatus::Failed;
	}

	// get the skeletal mesh
	USkeletalMeshComponent* SkelMesh = InstanceData.TargetActor->FindComponentByClass<USkeletalMeshComponent>();

	if (!SkelMesh)
	{
		UE_LOG(LogSampleStateTree, Error, TEXT("FStateTreePlayMontageTask: Could not find a Skeletal Mesh Component on the target Actor [%s]."), *InstanceData.TargetActor->GetName());
		return EStateTreeRunStatus::Failed;
	}

	// get the AnimInstance
	InstanceData.AnimInstance = SkelMesh->GetAnimInstance();

	if (InstanceData.AnimInstance == nullptr)
	{
		UE_LOG(LogSampleStateTree, Error, TEXT("FStateTreePlayMontageTask: Target Actor does not have a valid AnimInstance [%s]."), *InstanceData.TargetActor->GetName());
		return EStateTreeRunStatus::Failed;
	}

	// attempt to play the montage
	if (InstanceData.AnimInstance->Montage_Play(InstanceData.Montage, InstanceData.PlayRate, EMontagePlayReturnType::MontageLength, InstanceData.StartTime, bStopAllMontages) <= 0.0f)
	{
		UE_LOG(LogSampleStateTree, Error, TEXT("FStateTreePlayMontageTask: Could not play AnimMontage [%s] on Actor [%s]."), *InstanceData.Montage->GetName(), *InstanceData.TargetActor->GetName());
		return EStateTreeRunStatus::Failed;
	}

	// bind to the montage end delegate
	if (FOnMontageEnded* MontageEndDelegate = InstanceData.AnimInstance->Montage_GetEndedDelegate(InstanceData.Montage))
	{
		// avoid overwriting other bindings. For now we treat bound delegates as a task failure.
		// @TODO: see if we can find a more robust way of binding this
		if (!MontageEndDelegate->IsBound())
		{
			// flag it so we can unbind the delegate when we're done
			InstanceData.bDelegateBound = true;

			MontageEndDelegate->BindLambda(
			[WeakContext = Context.MakeWeakExecutionContext()](UAnimMontage* AnimMontage, bool bInterrupted)
				{
					// was the montage interrupted
					if (bInterrupted)
					{
						WeakContext.FinishTask(EStateTreeFinishTaskType::Failed);
					}
					else
					{
						WeakContext.FinishTask(EStateTreeFinishTaskType::Succeeded);
					}
				}
			);
		}
		else
		{
			UE_LOG(LogSampleStateTree, Error, TEXT("FStateTreePlayMontageTask: AnimMontage [%s] on Actor [%s] already has a Montage End delegate bound."), *InstanceData.Montage->GetName(), *InstanceData.TargetActor->GetName());
			return EStateTreeRunStatus::Failed;
		}
			
	}
	else
	{
		UE_LOG(LogSampleStateTree, Error, TEXT("FStateTreePlayMontageTask: No Montage End Delegate available for AnimMontage [%s] on Actor [%s]."), *InstanceData.Montage->GetName(), *InstanceData.TargetActor->GetName());
		return EStateTreeRunStatus::Failed;
	}

	return EStateTreeRunStatus::Running;
}

void FStateTreePlayMontageTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	// get the instance data
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// do we still have a valid AnimInstance?
	if (IsValid(InstanceData.AnimInstance))
	{
		// should we stop the montage on exit?
		if (InstanceData.Montage)
		{
			// only unbind if we managed to successfully bind the delegate, we don't want to accidentally unbind something else
			if (InstanceData.bDelegateBound)
			{
				// reset the flag for future runs
				InstanceData.bDelegateBound = false;

				// unbind from the montage end delegate
				if (FOnMontageEnded* MontageEndDelegate = InstanceData.AnimInstance->Montage_GetEndedDelegate(InstanceData.Montage))
				{
					MontageEndDelegate->Unbind();
				}
			}

			if (bStopOnExit)
			{
				InstanceData.AnimInstance->Montage_Stop(BlendOutTime, InstanceData.Montage);
			}

		}
	}
}

#if WITH_EDITOR

FText FStateTreePlayMontageTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting /*= EStateTreeNodeFormatting::Text*/) const
{
	// get the instance data
	const FInstanceDataType* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	check(InstanceData);

	// get the name of the Montage
	FText MontageText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, Montage)), Formatting);
	if (MontageText.IsEmpty())
	{
		if (!InstanceData->Montage)
		{
			return LOCTEXT("PlayMontageTaskInvalidOwner", "<b>WARNING: Invalid AnimMontage. Check the Montage Binding.</>");
		}

		MontageText = FText::FromString(InstanceData->Montage->GetName());

	}

	// get the binding name of the target
	FText TargetText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, TargetActor)), Formatting);
	if (TargetText.IsEmpty())
	{
		return LOCTEXT("PlayMontageTaskInvalidTarget", "<b>WARNING: Invalid Target Actor. Check the Target Actor Binding.</>");
	}

	FTextBuilder OutText;

	OutText.AppendLine(FText::Format(LOCTEXT("PlayMontageTaskPlay", "Play AnimMontage <b>{0}</> on <b>{1}</>"), MontageText, TargetText));
	
	FNumberFormattingOptions Options;
	Options.MaximumFractionalDigits = 2;
	Options.MinimumIntegralDigits = 1;
	Options.UseGrouping = false;

	// get the PlayRate binding
	FText PlayRateText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, PlayRate)), Formatting);
	if (PlayRateText.IsEmpty())
	{
		OutText.AppendLine(FText::Format(LOCTEXT("PlayMontageTaskPlayRateBound", "<s>Play Rate: </> {0}"), FText::AsNumber(InstanceData->PlayRate, &Options)));
	}
	else
	{
		OutText.AppendLine(FText::Format(LOCTEXT("PlayMontageTaskPlayRateNotBound", "<s>Play Rate: </> {0}"), PlayRateText));
	}

	// get the StartTime binding
	FText StartTimeText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, StartTime)), Formatting);
	if (StartTimeText.IsEmpty())
	{
		OutText.AppendLine(FText::Format(LOCTEXT("PlayMontageTaskStartTimeBound", "<s>Start Time: </> {0}"), FText::AsNumber(InstanceData->StartTime, &Options)));
	}
	else
	{
		OutText.AppendLine(FText::Format(LOCTEXT("PlayMontageTaskStartTimeNotBound", "<s>Start Time: </> {0}s"), StartTimeText));
	}

	// add the stop montages line
	OutText.AppendLine(bStopAllMontages ? LOCTEXT("PlayMontageTaskStopMontages", "<s>Stop All Montages</>") : LOCTEXT("PlayMontageTaskNoStopMontages", "<s>Do Not Stop All Montages</>"));

	if (bStopOnExit)
	{
		// get the BlendOut binding
		OutText.AppendLine(FText::Format(LOCTEXT("PlayMontageTaskBlendOut", "<s>Stop on Exit with Blend Out: </> {0}s"), FText::AsNumber(BlendOutTime, &Options)));

	}	
	
	return OutText.ToText();
}

EDataValidationResult FStateTreePlayMontageTask::Compile(UE::StateTree::ICompileNodeContext& CompileContext)
{
	const FInstanceDataType& InstanceData = CompileContext.GetInstanceDataView().Get<FInstanceDataType>();

	EDataValidationResult Result = EDataValidationResult::Valid;

	// ensure the actor class is valid
	if (!InstanceData.Montage && !CompileContext.HasBindingForProperty(GET_MEMBER_NAME_CHECKED(FInstanceDataType, Montage)))
	{
		CompileContext.AddValidationError(LOCTEXT("PlayMontageTaskErrorMontage", "Invalid AnimMontage. Check the Montage Binding."));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

#endif // WITH_EDITOR

EStateTreeRunStatus FStateTreeWaitForEventTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	// get the instance data
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// skip if the event tag is not valid
	if (!InstanceData.EventTag.IsValid())
	{
		UE_LOG(LogSampleStateTree, Error, TEXT("FStateTreeWaitForEventTask: Invalid Event Tag."));
		return EStateTreeRunStatus::Failed;
	}

	// skip if the target count is not valid
	if (InstanceData.TargetCount < 1)
	{
		UE_LOG(LogSampleStateTree, Error, TEXT("FStateTreeWaitForEventTask: Target Count is less than one."));
		return EStateTreeRunStatus::Failed;
	}

	// reset the current count to zero
	InstanceData.CurrentCount = 0;

	// keep the task running until all events have been received
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FStateTreeWaitForEventTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	// get the instance data
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// skip if the event tag is not valid
	if (!InstanceData.EventTag.IsValid())
	{
		UE_LOG(LogSampleStateTree, Error, TEXT("FStateTreeWaitForEventTask: Invalid Event Tag."));
		return EStateTreeRunStatus::Failed;
	}

	// skip if the target count is not valid
	if (InstanceData.TargetCount < 1)
	{
		UE_LOG(LogSampleStateTree, Error, TEXT("FStateTreeWaitForEventTask: Target Count is less than one."));
		return EStateTreeRunStatus::Failed;
	}

	// check the event queue for matching events
	int32 EventsReceived = 0;

	FStateTreeEventQueue& EventQueue = Context.GetMutableEventQueue();
	EventQueue.ForEachEvent([&](FStateTreeSharedEvent& Event)
	{
		// skip invalid events
		if (!Event.IsValid())
		{
			return EStateTreeLoopEvents::Next;
		}

		// skip events that don't match exactly
		if (!Event->Tag.MatchesTagExact(InstanceData.EventTag))
		{
			return EStateTreeLoopEvents::Next;
		}

		// increment the received counter
		++EventsReceived;

		// consume the event
		return EStateTreeLoopEvents::Consume;
	});

	// increment the current count
	InstanceData.CurrentCount += EventsReceived;

	// keep the task running until the target number of events have been received
	return InstanceData.CurrentCount < InstanceData.TargetCount ? EStateTreeRunStatus::Running : EStateTreeRunStatus::Succeeded;
}

#if WITH_EDITOR

FText FStateTreeWaitForEventTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting /*= EStateTreeNodeFormatting::Text*/) const
{
	// get the instance data
	const FInstanceDataType* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	check(InstanceData);

	// get the name of the event tag
	FText TagText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, EventTag)), Formatting);
	if (TagText.IsEmpty())
	{
		if (!InstanceData->EventTag.IsValid())
		{
			return LOCTEXT("WaitForEventTaskInvalidTag", "<b>WARNING: Invalid Event Tag. Check the Event Tag binding.</>");
		}

		// use the name of the tag
		TagText = FText::FromString(InstanceData->EventTag.ToString());
	}

	// get the name of the target count binding
	FText CountText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, TargetCount)), Formatting);
	if (CountText.IsEmpty())
	{
		if (InstanceData->TargetCount < 1)
		{
			return LOCTEXT("WaitForEventTaskInvalidCount", "<b>Invalid Target Count. Ensure the Target Count is greater than zero.</>");
		}

		// use the name of the tag
		CountText = FText::AsNumber(InstanceData->TargetCount);
	}

	// build the output text
	FTextBuilder OutText;

	OutText.AppendLine(FText::Format(LOCTEXT("WaitForEventTaskEvent", "Wait for StateTree Event <b>{0}</>"), TagText));
	OutText.AppendLine(FText::Format(LOCTEXT("WaitForEventTaskCount", "</s>Target Count: <b>{0}</></>"), CountText));
	
	return OutText.ToText();
}

EDataValidationResult FStateTreeWaitForEventTask::Compile(UE::StateTree::ICompileNodeContext& CompileContext)
{
	const FInstanceDataType& InstanceData = CompileContext.GetInstanceDataView().Get<FInstanceDataType>();

	EDataValidationResult Result = EDataValidationResult::Valid;

	// ensure the event tag is valid
	if (!InstanceData.EventTag.IsValid() && !CompileContext.HasBindingForProperty(GET_MEMBER_NAME_CHECKED(FInstanceDataType, EventTag)))
	{
		CompileContext.AddValidationError(LOCTEXT("WaitForEventTaskErrorInvalidTag", "Invalid Event Tag. Check the Event Tag binding."));
		Result = EDataValidationResult::Invalid;
	}

	// ensure the target count is valid
	if (InstanceData.TargetCount < 1 && !CompileContext.HasBindingForProperty(GET_MEMBER_NAME_CHECKED(FInstanceDataType, TargetCount)))
	{
		CompileContext.AddValidationError(LOCTEXT("WaitForEventTaskErrorInvalidCount", "Invalid Target Count. Ensure the Target Count is greater than zero."));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

#endif // WITH_EDITOR

#undef LOCTEXT_NAMESPACE