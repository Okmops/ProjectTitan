// Copyright Epic Games, Inc. All Rights Reserved.


#include "StateTree/STGASTasks.h"
#include "StateTreeExecutionContext.h"
#include "AbilitySystemComponent.h"
#include "SampleLogging.h"
#include "GAS/AbilitySet.h"
#include "GAS/SampleAbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

#define LOCTEXT_NAMESPACE "StateTreeGASTasks"

EStateTreeRunStatus FStateTreeGameplayEventTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	// get the instance data
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// skip if the event tag is not valid
	if (!EnterEventTag.IsValid())
	{
		return EStateTreeRunStatus::Running;
	}

	// get the ASC
	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(InstanceData.TargetActor);

	if (!ASC)
	{
		return EStateTreeRunStatus::Failed;
	}

	// build the event payload
	FGameplayEventData Payload;
	Payload.EventTag = EnterEventTag;
	Payload.Instigator = InstanceData.EventInstigator;
	Payload.InstigatorTags = InstanceData.InstigatorTags;
	Payload.EventMagnitude = InstanceData.EventMagnitude;
	Payload.OptionalObject = InstanceData.OptionalObject;
	Payload.OptionalObject2 = InstanceData.OptionalObject2;

	// send the event
	ASC->HandleGameplayEvent(EnterEventTag, &Payload);

	return EStateTreeRunStatus::Running;
}

void FStateTreeGameplayEventTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	// get the instance data
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// skip if the event tag is not valid
	if (!ExitEventTag.IsValid())
	{
		return;
	}

	// get the ASC
	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(InstanceData.TargetActor);

	if (!ASC)
	{
		return;
	}

	// build the event payload
	FGameplayEventData Payload;
	Payload.EventTag = ExitEventTag;
	Payload.Instigator = InstanceData.EventInstigator;
	Payload.InstigatorTags = InstanceData.InstigatorTags;
	Payload.EventMagnitude = InstanceData.EventMagnitude;
	Payload.OptionalObject = InstanceData.OptionalObject;
	Payload.OptionalObject2 = InstanceData.OptionalObject2;

	// send the event
	ASC->HandleGameplayEvent(ExitEventTag, &Payload);
}

EStateTreeRunStatus FStateTreeGameplayEventTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	// get the instance data
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// skip if the event tag is not valid
	if (!TickEventTag.IsValid())
	{
		return EStateTreeRunStatus::Running;
	}

	// get the ASC
	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(InstanceData.TargetActor);

	if (!ASC)
	{
		return EStateTreeRunStatus::Failed;
	}

	// build the event payload
	FGameplayEventData Payload;
	Payload.EventTag = TickEventTag;
	Payload.Instigator = InstanceData.EventInstigator;
	Payload.InstigatorTags = InstanceData.InstigatorTags;
	Payload.EventMagnitude = InstanceData.EventMagnitude;
	Payload.OptionalObject = InstanceData.OptionalObject;
	Payload.OptionalObject2 = InstanceData.OptionalObject2;

	// send the event
	ASC->HandleGameplayEvent(TickEventTag, &Payload);

	return EStateTreeRunStatus::Running;
}

#if WITH_EDITOR

FText FStateTreeGameplayEventTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting /*= EStateTreeNodeFormatting::Text*/) const
{
	// get the instance data
	const FInstanceDataType* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	check(InstanceData);

	// get the name of the target
	FText TargetText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, TargetActor)), Formatting);
	if (TargetText.IsEmpty())
	{
		return LOCTEXT("GameplayEventTaskInvalidTarget", "<b>WARNING: Invalid Target. Check the Target Actor Binding.</>");
	}

	if (!EnterEventTag.IsValid() && !ExitEventTag.IsValid() && !TickEventTag.IsValid())
	{
		return LOCTEXT("GameplayEventTaskNoTags", "<b>WARNING: No Event Tags. Add at least one of Enter, Exit or Tick Events.</>");
	}

	// build the description text
	FTextBuilder OutText;

	// add the target line
	OutText.AppendLine(FText::Format(LOCTEXT("GameplayEventTaskTarget", "Send Gameplay Events to <b>{0}</>"), TargetText));

	OutText.AppendLine();

	// build the enter event text
	FText EnterText;

	if (EnterEventTag.IsValid())
	{
		EnterText = FText::FromString(EnterEventTag.ToString());
	}

	if (!EnterText.IsEmpty())
	{
		OutText.AppendLine(FText::Format(LOCTEXT("GameplayEventTaskEnter", "<s>On Enter:</> {0}"), EnterText));
	}

	// build the exit event text
	FText ExitText;

	if (ExitEventTag.IsValid())
	{
		ExitText = FText::FromString(ExitEventTag.ToString());
	}

	if (!ExitText.IsEmpty())
	{
		OutText.AppendLine(FText::Format(LOCTEXT("GameplayEventTaskExit", "<s>On Exit:</> {0}"), ExitText));
	}

	// build the tick event text
	FText TickText;

	if (TickEventTag.IsValid())
	{
		TickText = FText::FromString(TickEventTag.ToString());
	}

	if (!TickText.IsEmpty())
	{
		OutText.AppendLine(FText::Format(LOCTEXT("GameplayEventTaskTick", "<s>On Tick:</> {0}"), TickText));
	}

	OutText.AppendLine();

	// build the payload text
	OutText.AppendLine(LOCTEXT("GameplayEventTaskPayloadStart", "<s>Payload</>\n<s>{</>"));
	
	// add the magnitude
	FNumberFormattingOptions Options;
	Options.MinimumFractionalDigits = 1;
	Options.MaximumFractionalDigits = 3;

	// add the instigator, if any
	FText MagnitudeText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, EventMagnitude)), Formatting);
	if (MagnitudeText.IsEmpty())
	{
		MagnitudeText = FText::AsNumber(InstanceData->EventMagnitude, &Options);
	}
	OutText.AppendLine(FText::Format(LOCTEXT("GameplayEventTaskMagnitude", "   <s>Magnitude:</> {0}"), MagnitudeText));

	// add the instigator, if any
	FText InstigatorText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, EventInstigator)), Formatting);
	if (!InstigatorText.IsEmpty())
	{
		OutText.AppendLine(FText::Format(LOCTEXT("GameplayEventTaskInstigator", "   <s>Instigator:</> {0}"), InstigatorText));

		// add the instigator tags, if any
		FText TagsText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, InstigatorTags)), Formatting);
		if (TagsText.IsEmpty())
		{
			if (InstanceData->InstigatorTags.IsValid())
			{
				OutText.AppendLine(LOCTEXT("GameplayEventTaskInstigatorTagsStart", "   <s>Tags</> \n   <s>{</>"));

				for (const FGameplayTag& CurrentTag : InstanceData->InstigatorTags.GetGameplayTagArray())
				{
					OutText.AppendLine(FText::Format(LOCTEXT("GameplayEventTaskInstigatorTagsLine", "      {0}"), FText::FromString(CurrentTag.ToString())));
				}

				OutText.AppendLine(LOCTEXT("GameplayEventTaskInstigatorTagsEnd", "   <s>}</>"));

			}
		}
		else
		{
			OutText.AppendLine(FText::Format(LOCTEXT("GameplayEventTaskInstigatorTagsBound", "   <s>Tags:</> {0}"), TagsText));
		}
	}

	// add the optional object 1, if any
	FText OptionalText1 = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, OptionalObject)), Formatting);

	// do we have an object set directly without a binding?
	if (OptionalText1.IsEmpty() && InstanceData->OptionalObject)
	{
		OptionalText1 = FText::FromString(InstanceData->OptionalObject->GetName());
	}

	if (!OptionalText1.IsEmpty())
	{
		OutText.AppendLine(FText::Format(LOCTEXT("GameplayEventTaskObject1", "   <s>Object 1:</> {0}"), OptionalText1));
	}

	// add the optional object 2, if any
	FText OptionalText2 = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, OptionalObject2)), Formatting);

	// do we have an object set directly without a binding?
	if (OptionalText2.IsEmpty() && InstanceData->OptionalObject2)
	{
		OptionalText2 = FText::FromString(InstanceData->OptionalObject2->GetName());
	}

	if (!OptionalText2.IsEmpty())
	{
		OutText.AppendLine(FText::Format(LOCTEXT("GameplayEventTaskObject2", "   <s>Object 2:</> {0}"), OptionalText2));
	}

	OutText.AppendLine(LOCTEXT("GameplayEventTaskPayloadEnd", "<s>}</>"));

	// return the description text
	return OutText.ToText();
}

EDataValidationResult FStateTreeGameplayEventTask::Compile(UE::StateTree::ICompileNodeContext& CompileContext)
{
	const FInstanceDataType& InstanceData = CompileContext.GetInstanceDataView().Get<FInstanceDataType>();

	EDataValidationResult Result = EDataValidationResult::Valid;

	// ensure we have at least one event to send
	if (!EnterEventTag.IsValid() && !ExitEventTag.IsValid() && !TickEventTag.IsValid())
	{
		CompileContext.AddValidationError(LOCTEXT("GameplayEventTaskErrorMissingEvents", "No Event Tags specified. Add at least one of Enter, Exit or Tick Events."));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

#endif // WITH_EDITOR

EStateTreeRunStatus FStateTreeGameplayEffectTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	// get the instance data
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (!IsValid(InstanceData.GameplayEffect))
	{
		UE_LOG(LogSampleStateTree, Error, TEXT("FStateTreeGameplayEffectTask: Attempted to apply an invalid Gameplay Effect."));
		return EStateTreeRunStatus::Failed;
	}

	// get the ASC
	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(InstanceData.TargetActor);

	if (!ASC)
	{
		return EStateTreeRunStatus::Failed;
	}

	// initialize the context handle
	// @TODO: maybe we want to expose some of these properties?
	FGameplayEffectContextHandle ContextHandle;

	// initialize the prediction key
	// @TODO: do we want to expose this? Most StateTrees will run server side so this may be unnecessary.
	FPredictionKey PredictionKey;

	// initialize the effect spec
	FGameplayEffectSpec EffectSpec;
	EffectSpec.Initialize(InstanceData.GameplayEffect->GetDefaultObject<UGameplayEffect>(), ContextHandle, InstanceData.Level);
	EffectSpec.SetStackCount(InstanceData.Stacks);

	// add the Set by Caller Magnitudes
	for (const FStateTreeSetByCallerMagnitude Current : InstanceData.SetByCallerMagnitudes)
	{
		EffectSpec.SetSetByCallerMagnitude(Current.Tag, Current.Magnitude);
	}

	// apply the effect
	InstanceData.ActiveEffectHandle = ASC->ApplyGameplayEffectSpecToTarget(EffectSpec, ASC, PredictionKey);

	return EStateTreeRunStatus::Running;
}

void FStateTreeGameplayEffectTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	// get the instance data
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// get the ASC
	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(InstanceData.TargetActor);

	if (!ASC)
	{
		return;
	}

	// remove the effect using the handle stored in the InstanceData
	if (InstanceData.bRemoveOnStateEnd)
	{
		ASC->RemoveActiveGameplayEffect(InstanceData.ActiveEffectHandle, InstanceData.Stacks);
	}
}

#if WITH_EDITOR

FText FStateTreeGameplayEffectTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting /*= EStateTreeNodeFormatting::Text*/) const
{
	// get the instance data
	const FInstanceDataType* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	check(InstanceData);

	// get the name of the target
	FText TargetText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, TargetActor)), Formatting);
	if (TargetText.IsEmpty())
	{
		return LOCTEXT("GameplayEffectTaskInvalidTarget", "<b>WARNING: Invalid Target. Check the Target Actor Binding.</>");
	}

	// get the name of the GE
	FText EffectText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, GameplayEffect)), Formatting);
	if (EffectText.IsEmpty())
	{
		// check if we have a GE set directly
		if (IsValid(InstanceData->GameplayEffect))
		{
			EffectText = InstanceData->GameplayEffect->GetDisplayNameText();
		}
		else
		{
			return LOCTEXT("GameplayEffectTaskInvalidEffect", "<b>WARNING: Invalid Gameplay Effect. Check the Gameplay Effect Binding.</>");
		}
	}

	FTextBuilder OutText;

	// add the target line
	OutText.AppendLine(FText::Format(LOCTEXT("GameplayEffectTaskGrantEffect", "Grant <b>{0}</> to <b>{1}</>"), EffectText, TargetText));

	// build the level text
	FNumberFormattingOptions Options;
	Options.MinimumFractionalDigits = 1;
	Options.MaximumFractionalDigits = 3;

	// add the level line
	FText LevelText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, Level)), Formatting);
	if (LevelText.IsEmpty())
	{
		LevelText = FText::AsNumber(InstanceData->Level, &Options);
	}
	OutText.AppendLine(FText::Format(LOCTEXT("GameplayEffectTaskLevel", "<s>Level:</> {0}"), LevelText));

	// add the stacks line
	FText StacksText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, Stacks)), Formatting);
	if (StacksText.IsEmpty())
	{
		StacksText = FText::AsNumber(InstanceData->Stacks, &Options);
	}
	OutText.AppendLine(FText::Format(LOCTEXT("GameplayEffectTaskStacks", "<s>Stacks:</> {0}"), StacksText));
	
	// add the remove on end line
	OutText.AppendLine(InstanceData->bRemoveOnStateEnd ? LOCTEXT("GameplayEffectTaskRemove", "Remove <s>the Effect on State End</>") :  LOCTEXT("GameplayEffectNoRemove", "Do Not Remove <s>the Effect on State End</>"));

	// build the set by caller magnitude lines
	FText MagnitudesText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, SetByCallerMagnitudes)), Formatting);
	if (MagnitudesText.IsEmpty())
	{
		if (InstanceData->SetByCallerMagnitudes.Num() > 0)
		{
			OutText.AppendLine(LOCTEXT("GameplayEffectTaskSetByCallerStart", "<s>Set By Caller Magnitudes\n{</>"));

			for (const FStateTreeSetByCallerMagnitude Current : InstanceData->SetByCallerMagnitudes)
			{
				OutText.AppendLine(FText::Format(LOCTEXT("GameplayEffectTaskSetByCallerLine", " {0}: {1}"), FText::FromString(Current.Tag.ToString()), FText::AsNumber(Current.Magnitude, &Options)));
			}

			OutText.AppendLine(LOCTEXT("GameplayEffectTaskSetByCallerEnd", "<s>}</>"));
		}
	}
	else
	{
		OutText.AppendLine(FText::Format(LOCTEXT("GameplayEffectTaskSetByCallerBound", "<s>Set By Caller Magnitudes:</> {0}"), MagnitudesText));
	}

	// return the description text
	return OutText.ToText();
}

EDataValidationResult FStateTreeGameplayEffectTask::Compile(UE::StateTree::ICompileNodeContext& CompileContext)
{
	const FInstanceDataType& InstanceData = CompileContext.GetInstanceDataView().Get<FInstanceDataType>();

	EDataValidationResult Result = EDataValidationResult::Valid;

	// ensure the gameplay effect class is valid
	if (!IsValid(InstanceData.GameplayEffect) && !CompileContext.HasBindingForProperty(GET_MEMBER_NAME_CHECKED(FInstanceDataType, GameplayEffect)))
	{
		CompileContext.AddValidationError(LOCTEXT("GameplayEffectTaskErrorInvalidGE", "Invalid Gameplay Effect. Check the Gameplay Effect Binding."));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

#endif // WITH_EDITOR

EStateTreeRunStatus FStateTreeGameplayCueTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	// get the instance data
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (!InstanceData.GameplayCue.IsValid())
	{
		UE_LOG(LogSampleStateTree, Error, TEXT("FStateTreeGameplayCueTask: Attempted to apply an invalid Gameplay Cue."));
		return EStateTreeRunStatus::Failed;
	}

	// get the ASC
	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(InstanceData.TargetActor);

	if (!ASC)
	{
		return EStateTreeRunStatus::Failed;
	}

	// initialize the gameplay cue params
	FGameplayCueParameters GameplayCueParams;
	GameplayCueParams.Instigator = InstanceData.TargetActor;
	GameplayCueParams.EffectCauser = InstanceData.EffectCauser;
	GameplayCueParams.AbilityLevel = InstanceData.AbilityLevel;
	GameplayCueParams.RawMagnitude = InstanceData.RawMagnitude;
	GameplayCueParams.Location = InstanceData.Location;
	GameplayCueParams.Normal = InstanceData.Normal;

	ASC->ExecuteGameplayCue(InstanceData.GameplayCue, GameplayCueParams);

	return EStateTreeRunStatus::Succeeded;
}

#if WITH_EDITOR

FText FStateTreeGameplayCueTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting /*= EStateTreeNodeFormatting::Text*/) const
{
	// get the instance data
	const FInstanceDataType* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	check(InstanceData);

	// get the name of the target
	FText TargetText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, TargetActor)), Formatting);
	if (TargetText.IsEmpty())
	{
		return LOCTEXT("GameplayCueTaskInvalidTarget", "<b>WARNING: Invalid Target. Check the Target Actor Binding.</>");
	}

	// get the name of the GameplayCue
	FText CueText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, GameplayCue)), Formatting);
	if (CueText.IsEmpty())
	{
		// check if we have a GameplayCue set directly
		if (InstanceData->GameplayCue.IsValid())
		{
			CueText = FText::FromString(InstanceData->GameplayCue.ToString());
		}
		else
		{
			return LOCTEXT("GameplayCueTaskInvalidEffect", "<b>WARNING: Invalid Gameplay Cue Tag. Check the Gameplay Cue Binding.</>");
		}
	}

	FTextBuilder OutText;

	// add the target line
	OutText.AppendLine(FText::Format(LOCTEXT("GameplayCueTaskTriggerCue", "Trigger <b>{0}</> on <b>{1}</>"), CueText, TargetText));

	// add the causer line
	FText CauserText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, EffectCauser)), Formatting);
	if (CauserText.IsEmpty())
	{
		CauserText = LOCTEXT("GameplayCueTaskNoCauser", "None");
	}
	OutText.AppendLine(FText::Format(LOCTEXT("GameplayCueTaskCauser", "<s>Effect Causer:</> {0}"), CauserText));

	// build the level text
	FNumberFormattingOptions Options;
	Options.MinimumFractionalDigits = 1;
	Options.MaximumFractionalDigits = 3;

	// add the level line
	FText LevelText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, AbilityLevel)), Formatting);
	if (LevelText.IsEmpty())
	{
		LevelText = FText::AsNumber(InstanceData->AbilityLevel, &Options);
	}
	OutText.AppendLine(FText::Format(LOCTEXT("GameplayCueTaskLevel", "<s>Level:</> {0}"), LevelText));

	// add the magnitude line
	FText MagnitudeText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, RawMagnitude)), Formatting);
	if (MagnitudeText.IsEmpty())
	{
		MagnitudeText = FText::AsNumber(InstanceData->RawMagnitude, &Options);
	}
	OutText.AppendLine(FText::Format(LOCTEXT("GameplayCueTaskMagnitude", "<s>Raw Magnitude:</> {0}"), MagnitudeText));

	// add the location line
	FText LocationText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, Location)), Formatting);
	if (LocationText.IsEmpty())
	{
		LocationText = FText::FromString(InstanceData->Location.ToCompactString());
	}
	OutText.AppendLine(FText::Format(LOCTEXT("GameplayCueTaskLocation", "<s>Location:</> {0}"), LocationText));

	// add the normal line
	FText NormalText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, Normal)), Formatting);
	if (NormalText.IsEmpty())
	{
		NormalText = FText::FromString(InstanceData->Normal.ToCompactString());
	}
	OutText.AppendLine(FText::Format(LOCTEXT("GameplayCueTaskNormal", "<s>Normal:</> {0}"), NormalText));

	// return the description text
	return OutText.ToText();
}

EDataValidationResult FStateTreeGameplayCueTask::Compile(UE::StateTree::ICompileNodeContext& CompileContext)
{
	const FInstanceDataType& InstanceData = CompileContext.GetInstanceDataView().Get<FInstanceDataType>();

	EDataValidationResult Result = EDataValidationResult::Valid;

	// ensure the gameplay cue tag is valid
	if (!InstanceData.GameplayCue.IsValid() && !CompileContext.HasBindingForProperty(GET_MEMBER_NAME_CHECKED(FInstanceDataType, GameplayCue)))
	{
		CompileContext.AddValidationError(LOCTEXT("GameplayCueTaskErrorMissingTag", "Invalid Gameplay Cue Tag. Check the Gameplay Cue Binding."));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

#endif // WITH_EDITOR

EStateTreeRunStatus FStateTreeAbilitySetTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	// get the instance data
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (!AbilitySet)
	{
		UE_LOG(LogSampleStateTree, Error, TEXT("FStateTreeAbilitySetTask: Attempted to grant an invalid Ability Set."));
		return EStateTreeRunStatus::Failed;
	}

	// get the ASC
	USampleAbilitySystemComponent* ASC = Cast<USampleAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(InstanceData.TargetActor));

	if (!ASC)
	{
		return EStateTreeRunStatus::Failed;
	}

	// grant the ability set and store the granted handles so we can remove them later if needed
	AbilitySet->GiveToAbilitySystem(ASC, &InstanceData.GrantedHandles, Context.GetOwner());

	return EStateTreeRunStatus::Running;
}

void FStateTreeAbilitySetTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	// get the instance data
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// do we want to remove the handles on state exit?
	if (bRemoveOnStateExit)
	{
		if (!AbilitySet)
		{
			UE_LOG(LogSampleStateTree, Error, TEXT("FStateTreeAbilitySetTask: Attempted to remove an invalid Ability Set."));
			return;
		}

		// get the ASC
		USampleAbilitySystemComponent* ASC = Cast<USampleAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(InstanceData.TargetActor));

		if (!ASC)
		{
			return;
		}

		// use the granted handles to remove the ASC
		InstanceData.GrantedHandles.TakeFromAbilitySystem(ASC);
	}
}

#if WITH_EDITOR

FText FStateTreeAbilitySetTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting /*= EStateTreeNodeFormatting::Text*/) const
{
	// get the instance data
	const FInstanceDataType* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	check(InstanceData);

	// get the name of the target
	FText TargetText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, TargetActor)), Formatting);
	if (TargetText.IsEmpty())
	{
		return LOCTEXT("AbilitySetTaskInvalidTarget", "<b>WARNING: Invalid Target. Check the Target Actor Binding.</>");
	}

	// get the name of the AbilitySet
	FText SetText;

	// check if we have an AbilitySet
	if (AbilitySet)
	{
		SetText = FText::FromString(AbilitySet->GetName());
	}
	else
	{
		return LOCTEXT("AbilitySetTaskInvalidSet", "<b>WARNING: Invalid Ability Set. Assign an Ability Set asset.</>");
	}

	FTextBuilder OutText;

	// add the target line
	OutText.AppendLine(FText::Format(LOCTEXT("AbilitySetTaskGrant", "Grant AbilitySet <b>{0}</> to <b>{1}</>"), SetText, TargetText));

	// add the remove on exit line
	OutText.AppendLine(bRemoveOnStateExit ? LOCTEXT("AbilitySetTaskRemove", "<s>Remove AbilitySet on Exit</>") :  LOCTEXT("AbilitySetTaskNoRemove", "<s>Do Not Remove AbilitySet on Exit</>"));

	return OutText.ToText();
}

EDataValidationResult FStateTreeAbilitySetTask::Compile(UE::StateTree::ICompileNodeContext& CompileContext)
{
	const FInstanceDataType& InstanceData = CompileContext.GetInstanceDataView().Get<FInstanceDataType>();

	EDataValidationResult Result = EDataValidationResult::Valid;

	// ensure the Ability Set ref is valid
	if (!AbilitySet)
	{
		CompileContext.AddValidationError(LOCTEXT("AbilitySetTaskErrorMissingSet", "Invalid Ability Set. Assign an Ability Set asset."));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

#endif // WITH_EDITOR

EStateTreeRunStatus FStateTreeGiveAndActivateAbilityTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	// get the instance data
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (!InstanceData.Ability)
	{
		UE_LOG(LogSampleStateTree, Error, TEXT("FStateTreeGiveAndActivateAbilityTask: Attempted to activate an invalid ability."));
		return EStateTreeRunStatus::Failed;
	}

	// get the ASC
	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(InstanceData.TargetActor);

	if (!ASC)
	{
		return EStateTreeRunStatus::Failed;
	}

	// build the ability spec
	FGameplayAbilitySpec AbilitySpec(InstanceData.Ability, InstanceData.Level, -1, Context.GetOwner());

	// grant and activate the ability and store the handle. It will be automatically removed when it finishes.
	InstanceData.AbilityHandle = ASC->GiveAbilityAndActivateOnce(AbilitySpec);

	return EStateTreeRunStatus::Running;
}

void FStateTreeGiveAndActivateAbilityTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	// get the instance data
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// do we want to remove the handles on state exit?
	if (InstanceData.bCancelAbilityOnStateExit)
	{
		if (!InstanceData.Ability)
		{
			UE_LOG(LogSampleStateTree, Error, TEXT("FStateTreeGiveAndActivateAbilityTask: Attempted to cancel an invalid Ability."));
			return;
		}

		// get the ASC
		UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(InstanceData.TargetActor);

		if (!ASC)
		{
			return;
		}

		// use the stored handle to cancel the ability
		ASC->CancelAbilityHandle(InstanceData.AbilityHandle);
	}
}

#if WITH_EDITOR

FText FStateTreeGiveAndActivateAbilityTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting /*= EStateTreeNodeFormatting::Text*/) const
{
	// get the instance data
	const FInstanceDataType* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	check(InstanceData);

	// get the name of the target
	FText TargetText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, TargetActor)), Formatting);
	if (TargetText.IsEmpty())
	{
		return LOCTEXT("GiveAndActivateAbilityTaskInvalidTarget", "<b>WARNING: Invalid Target. Check the Target Actor Binding.</>");
	}

	// get the name of the Ability
	FText AbilityText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, Ability)), Formatting);
	if (AbilityText.IsEmpty())
	{
		// check if we have a valid Ability
		if (InstanceData->Ability)
		{
			AbilityText = InstanceData->Ability->GetDisplayNameText();
		}
		else
		{
			return LOCTEXT("GiveAndActivateAbilityTaskInvalidAbility", "<b>WARNING: Invalid Ability. Check the Ability Binding.</>");
		}
	}

	FTextBuilder OutText;

	// add the target line
	OutText.AppendLine(FText::Format(LOCTEXT("GiveAndActivateAbilityTaskGive", "Give Ability <b>{0}</> to <b>{1}</> and Activate Once"), AbilityText, TargetText));

	// build the level text
	FNumberFormattingOptions Options;
	Options.MinimumFractionalDigits = 1;
	Options.MaximumFractionalDigits = 3;

	// add the level line
	FText LevelText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, Level)), Formatting);
	if (LevelText.IsEmpty())
	{
		LevelText = FText::AsNumber(InstanceData->Level, &Options);
	}
	OutText.AppendLine(FText::Format(LOCTEXT("GiveAndActivateAbilityTaskLevel", "<s>Level:</> {0}"), LevelText));

	// add the cancel on exit line
	OutText.AppendLine(InstanceData->bCancelAbilityOnStateExit ? LOCTEXT("GiveAndActivateAbilityTaskCancel", "<s>Cancel the Ability on Exit</>") :  LOCTEXT("GiveAndActivateAbilityTaskNoCancel", "<s>Do Not Cancel the Ability on Exit</>"));

	return OutText.ToText();
}

EDataValidationResult FStateTreeGiveAndActivateAbilityTask::Compile(UE::StateTree::ICompileNodeContext& CompileContext)
{
	const FInstanceDataType& InstanceData = CompileContext.GetInstanceDataView().Get<FInstanceDataType>();

	EDataValidationResult Result = EDataValidationResult::Valid;

	// ensure the Ability ref is valid
	if (!InstanceData.Ability && !CompileContext.HasBindingForProperty(GET_MEMBER_NAME_CHECKED(FInstanceDataType, Ability)))
	{
		CompileContext.AddValidationError(LOCTEXT("GiveAndActivateAbilityTaskErrorMissingAbility", "Invalid Ability. Check the Ability Binding."));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

#endif // WITH_EDITOR

EStateTreeRunStatus FStateTreeActivateAbilityByClassTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	// get the instance data
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (!InstanceData.Ability)
	{
		UE_LOG(LogSampleStateTree, Error, TEXT("FStateTreeActivateAbilityByClassTask: Attempted to activate an invalid ability."));
		return EStateTreeRunStatus::Failed;
	}

	// get the ASC
	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(InstanceData.TargetActor);

	if (!ASC)
	{
		return EStateTreeRunStatus::Failed;
	}

	// activate the ability. Note we can't store the handle to cancel automatically
	if (ASC->TryActivateAbilityByClass(InstanceData.Ability, InstanceData.bAllowRemoteActivation))
	{
		// succeed if the ability was activated
		return EStateTreeRunStatus::Succeeded;
	}

	return EStateTreeRunStatus::Failed;
}

#if WITH_EDITOR

FText FStateTreeActivateAbilityByClassTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting /*= EStateTreeNodeFormatting::Text*/) const
{
	// get the instance data
	const FInstanceDataType* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	check(InstanceData);

	// get the name of the target
	FText TargetText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, TargetActor)), Formatting);
	if (TargetText.IsEmpty())
	{
		return LOCTEXT("ActivateAbilityByClassTaskInvalidTarget", "<b>WARNING: Invalid Target. Check the Target Actor Binding.</>");
	}

	// get the name of the Ability
	FText AbilityText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, Ability)), Formatting);
	if (AbilityText.IsEmpty())
	{
		// check if we have a valid Ability
		if (InstanceData->Ability)
		{
			AbilityText = InstanceData->Ability->GetDisplayNameText();
		}
		else
		{
			return LOCTEXT("ActivateAbilityByClassTaskInvalidAbility", "<b>WARNING: Invalid Ability. Check the Ability Binding.</>");
		}
	}

	FTextBuilder OutText;

	// add the target line
	OutText.AppendLine(FText::Format(LOCTEXT("ActivateAbilityByClassTaskActivate", "Activate Ability <b>{0}</> on <b>{1}</>"), AbilityText, TargetText));

	// add the remote activation line
	OutText.AppendLine(InstanceData->bAllowRemoteActivation ? LOCTEXT("ActivateAbilityByClassTaskAllowRemote", "<s>Allow Remote Activation</>") :  LOCTEXT("ActivateAbilityByClassTaskNoRemote", "<s>Do Not Allow Remote Activation</>"));

	return OutText.ToText();
}

EDataValidationResult FStateTreeActivateAbilityByClassTask::Compile(UE::StateTree::ICompileNodeContext& CompileContext)
{
	const FInstanceDataType& InstanceData = CompileContext.GetInstanceDataView().Get<FInstanceDataType>();

	EDataValidationResult Result = EDataValidationResult::Valid;

	// ensure the Ability ref is valid
	if (!InstanceData.Ability && !CompileContext.HasBindingForProperty(GET_MEMBER_NAME_CHECKED(FInstanceDataType, Ability)))
	{
		CompileContext.AddValidationError(LOCTEXT("ActivateAbilityByClassTaskErrorMissingAbility", "Invalid Ability. Check the Ability Binding."));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

#endif // WITH_EDITOR

EStateTreeRunStatus FStateTreeActivateAbilitiesByTagTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	// get the instance data
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (!InstanceData.ActivationTags.IsValid())
	{
		UE_LOG(LogSampleStateTree, Error, TEXT("FStateTreeActivateAbilitiesByTagTask: Attempted to activate abilities with an invalid Tag."));
		return EStateTreeRunStatus::Failed;
	}

	// get the ASC
	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(InstanceData.TargetActor);

	if (!ASC)
	{
		return EStateTreeRunStatus::Failed;
	}

	// activate the ability. Note we can't store the handle to cancel automatically
	if (ASC->TryActivateAbilitiesByTag(InstanceData.ActivationTags, InstanceData.bAllowRemoteActivation))
	{
		// succeed if at least one ability was activated
		return EStateTreeRunStatus::Succeeded;
	}

	return EStateTreeRunStatus::Failed;
}

#if WITH_EDITOR

FText FStateTreeActivateAbilitiesByTagTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting /*= EStateTreeNodeFormatting::Text*/) const
{
	// get the instance data
	const FInstanceDataType* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	check(InstanceData);

	// get the name of the target
	FText TargetText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, TargetActor)), Formatting);
	if (TargetText.IsEmpty())
	{
		return LOCTEXT("ActivateAbilityByTagTaskInvalidTarget", "<b>WARNING: Invalid Target. Check the Target Actor Binding.</>");
	}

	FTextBuilder OutText;

	// get the name of the Tags binding
	FText TagsText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, ActivationTags)), Formatting);
	if (TagsText.IsEmpty())
	{
		// check if we have activation tags
		if (InstanceData->ActivationTags.IsValid())
		{
			// add the target line
			OutText.AppendLine(FText::Format(LOCTEXT("ActivateAbilityByClassTaskActivateStart", "Activate Abilities on <b>{0}</> with Tags:\n<s>{</s>"), TargetText));

			for (const FGameplayTag& CurrentTag : InstanceData->ActivationTags.GetGameplayTagArray())
			{
				OutText.AppendLine(FText::Format(LOCTEXT("ActivateAbilityByClassTaskActivateLine", "   {0}"), FText::FromString(CurrentTag.ToString())));
			}

			OutText.AppendLine(LOCTEXT("ActivateAbilityByClassTaskActivateEnd", "<s>}</s>"));
		}
		else
		{
			return LOCTEXT("ActivateAbilityByTagTaskInvalidTags", "<b>WARNING: Invalid Activation Tags. Check the Activation Tags Binding.</>");
		}
	}
	else
	{
		// add the single target line
		OutText.AppendLine(FText::Format(LOCTEXT("ActivateAbilityByClassTaskActivateBound", "Activate Abilities with Tags <b>{0}</> on <b>{1}</>"), TagsText, TargetText));
	}

	// add the cancel on exit line
	OutText.AppendLine(InstanceData->bAllowRemoteActivation ? LOCTEXT("ActivateAbilityByClassTaskAllowRemote", "<s>Allow Remote Activation</>") :  LOCTEXT("ActivateAbilityByClassTaskNoRemote", "<s>Do Not Allow Remote Activation</>"));

	return OutText.ToText();
}

EDataValidationResult FStateTreeActivateAbilitiesByTagTask::Compile(UE::StateTree::ICompileNodeContext& CompileContext)
{
	const FInstanceDataType& InstanceData = CompileContext.GetInstanceDataView().Get<FInstanceDataType>();

	EDataValidationResult Result = EDataValidationResult::Valid;

	// ensure the Ability Set ref is valid
	if (!InstanceData.ActivationTags.IsValid() && !CompileContext.HasBindingForProperty(GET_MEMBER_NAME_CHECKED(FInstanceDataType, ActivationTags)))
	{
		CompileContext.AddValidationError(LOCTEXT("ActivateAbilityByTagTaskErrorMissingAbility", "Invalid Activation Tags. Check the Activation Tags Binding."));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

#endif // WITH_EDITOR

#undef LOCTEXT_NAMESPACE