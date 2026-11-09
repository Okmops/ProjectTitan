// Copyright Epic Games, Inc. All Rights Reserved.


#include "QuestComponent.h"
#include "QuestLogging.h"
#include "Engine/World.h"
#include "QuestSubsystem.h"
#include "QuestDefinition.h"
#include "StateTree.h"
#include "Components/StateTreeComponent.h"
#include "QuestFunctionLibrary.h"
#include "Engine/World.h"

UE_DEFINE_GAMEPLAY_TAG(TAG_Quest_QuestID, "QuestSystem.Quest");
UE_DEFINE_GAMEPLAY_TAG(TAG_Quest_QuestEvent, "QuestSystem.Event");

#define LOCTEXT_NAMESPACE "QuestManagerComponent"

UQuestManagerComponent::UQuestManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UQuestManagerComponent::StartQuest(FGameplayTag QuestTag, bool bSendQuestStarted)
{
	// ensure the quest tag is valid
	if (!IsQuestTag(QuestTag))
	{
		UE_LOG(LogQuest, Warning, TEXT("UQuestManagerComponent: Attempted to start a quest with an invalid QuestTag."));
		return false;
	}

	// ensure we're not trying to add multiple copies of an existing quest
	if (ActiveQuests.HasTag(QuestTag) || (ActiveQuestStateTreeComponents.Find(QuestTag) != nullptr))
	{
		UE_LOG(LogQuest, Warning, TEXT("UQuestManagerComponent: Attempted to start a quest that is already active. [%s]"), *QuestTag.ToString());
		return false;
	}

	// get the quest subsystem
	if (UWorld* World = GetWorld())
	{
		if (UQuestSubsystem* QuestSubsystem = World->GetSubsystem<UQuestSubsystem>())
		{
			// get the quest definition
			if (UQuestDefinition* QuestDef = QuestSubsystem->GetQuestDefinition(QuestTag))
			{
				// get the StateTree asset
				if (UStateTree* QuestStateTree = QuestDef->GetStateTree())
				{
					// create a new StateTree Component to run the Quest
					UStateTreeComponent* StateTreeComponent = Cast<UStateTreeComponent>(GetOwner()->AddComponentByClass(UStateTreeComponent::StaticClass(), false, FTransform::Identity, false));
			
					// ensure the component was actually created
					if (StateTreeComponent)
					{
						// set the StateTree
						StateTreeComponent->SetStateTree(QuestStateTree);

						// subscribe to status changed events on the StateTree so we can detect completion and failure
						StateTreeComponent->OnStateTreeRunStatusChanged.AddDynamic(this, &UQuestManagerComponent::OnStateTreeStatusChanged);

						// add the component to our active StateTree map
						ActiveQuestStateTreeComponents.Add(QuestTag, StateTreeComponent);

						// add the quest tag to the active quests
						ActiveQuests.AddTag(QuestTag);

						// remove the quest tag from completed and failed quests
						CompletedQuests.RemoveTag(QuestTag);
						FailedQuests.RemoveTag(QuestTag);

						// call the delegate
						if (bSendQuestStarted)
						{
							OnQuestStarted.Broadcast(QuestTag);
						}

						// start the quest StateTree
						StateTreeComponent->StartLogic();

						return true;
					}
					else
					{
						UE_LOG(LogQuest, Error, TEXT("UQuestManagerComponent: Could not crate a new StateTree Component to run quest [%s]"), *QuestTag.ToString());
					}

				}
				else
				{
					UE_LOG(LogQuest, Error, TEXT("UQuestManagerComponent: Attempted to start a quest with an invalid StateTree. [%s]"), *QuestTag.ToString());
				}
			}
			else
			{
				UE_LOG(LogQuest, Warning, TEXT("UQuestManagerComponent: No Quest Definition associated with Tag [%s]"), *QuestTag.ToString());
			}
		}
		else
		{
			UE_LOG(LogQuest, Error, TEXT("UQuestManagerComponent: Quest Subsystem not found."));
		}
	}

	return false;
}

bool UQuestManagerComponent::FinishQuest(FGameplayTag QuestTag, bool bQuestSucceeded /*= true*/)
{
	// ensure the quest tag is valid
	if (!IsQuestTag(QuestTag))
	{
		UE_LOG(LogQuest, Warning, TEXT("UQuestManagerComponent: Attempted to finish a quest with an invalid QuestTag."));
		return false;
	}

	// ensure we're not trying to add multiple copies of an existing quest
	if (!ActiveQuests.HasTag(QuestTag))
	{
		UE_LOG(LogQuest, Warning, TEXT("UQuestManagerComponent: Attempted to finish a quest that is not yet active. [%s]"), *QuestTag.ToString());
		return false;
	}

	// get the StateTree
	TObjectPtr<UStateTreeComponent>* QuestPtr = ActiveQuestStateTreeComponents.Find(QuestTag);

	if (QuestPtr)
	{
		if (TObjectPtr<UStateTreeComponent> Quest = *QuestPtr)
		{
			// ensure the quest is stopped 
			Quest->StopLogic(FString("UQuestManagerComponent::FinishQuest"));

			// destroy and remove the component from Active Quests
			Quest->DestroyComponent();

			// remove the quest from active quests
			ActiveQuestStateTreeComponents.Remove(QuestTag);
			ActiveQuests.RemoveTag(QuestTag);

			// add the quest to completed or failed quests
			if (bQuestSucceeded)
			{
				CompletedQuests.AddTag(QuestTag);
			}
			else
			{
				FailedQuests.AddTag(QuestTag);
			}

			// call the delegate
			OnQuestCompleted.Broadcast(QuestTag, bQuestSucceeded);

			return true;
		}
	}

	return false;
}

bool UQuestManagerComponent::UpdateQuest(FGameplayTag QuestTag, FGameplayTag QuestStepTag, const FQuestUpdateData& UpdateData)
{
	// ensure the tags are valid
	if (!IsQuestTag(QuestTag) || !IsQuestTag(QuestStepTag))
	{
		return false;
	}

	// find the quest definition
	if (UQuestDefinition* QuestDef = UQuestFunctionLibrary::GetQuestDefinition(this, QuestTag))
	{
		FQuestStep QuestStep;

		// find the quest step
		if (QuestDef->FindQuestStep(QuestStepTag, QuestStep))
		{
			// format the quest update text
			
			// use the description by default
			FText NotifyText = QuestStep.Description;

			// do we have a progress string to format?
			FText FormattedProgress;

			if (!QuestStep.ProgressFormat.IsEmpty())
			{
				// compile the optional data into format named arguments
				FFormatNamedArguments ProgressArguments;
				ProgressArguments.Empty(UpdateData.FloatValues.Num() + UpdateData.IntValues.Num() + UpdateData.TextValues.Num() + 1);

				// try to get the quest progress quantity
				int32 ProgressQuantity = GetQuestQuantity(QuestStep.QuestStepProgressQuantityTag);

				// save the progress quantity, defaults to 0 if not existent
				ProgressArguments.Add("q0", ProgressQuantity);

				// save all float values as f0 - fn
				for (int32 i = 0; i < UpdateData.FloatValues.Num(); ++i)
				{
					FString KeyString = FString::Printf(TEXT("f%d"), i);

					ProgressArguments.Add(*KeyString, UpdateData.FloatValues[i]);
				}

				// save all int values as i0 - in
				for (int32 i = 0; i < UpdateData.IntValues.Num(); ++i)
				{
					FString KeyString = FString::Printf(TEXT("i%d"), i);

					ProgressArguments.Add(*KeyString, UpdateData.IntValues[i]);
				}

				// save all text values as t0 - tn
				for (int32 i = 0; i < UpdateData.TextValues.Num(); ++i)
				{
					FString KeyString = FString::Printf(TEXT("t%d"), i);

					ProgressArguments.Add(*KeyString, UpdateData.TextValues[i]);
				}

				// format the progress text
				FormattedProgress = FText::Format(QuestStep.ProgressFormat, ProgressArguments);

				// format the notification text
				NotifyText = FText::Format(LOCTEXT("HUDQuestProgress", "{0} {1}"), QuestStep.Description, FormattedProgress);
			}

			// is this step completed?
			const bool bCompleted = IsQuestStepCompleted(QuestStep);

			// call the delegate
			OnQuestUpdated.Broadcast(QuestTag, QuestStepTag, NotifyText, bCompleted);

			return true;
		}
	}

	// failed
	return false;
}

bool UQuestManagerComponent::AddQuestStatusTag(FGameplayTag QuestStatusTag)
{
	// ensure the tag to add is valid
	if (!IsQuestTag(QuestStatusTag))
	{
		return false;
	}

	// ensure the tag is not already present
	if (QuestStatusData.HasTagExact(QuestStatusTag))
	{
		return false;
	}

	// tag is not present, so add it and return true
	QuestStatusData.AddTag(QuestStatusTag);
	return true;
}

bool UQuestManagerComponent::RemoveQuestStatusTag(FGameplayTag QuestStatusTag)
{
	// ensure the tag to remove is valid
	if (!IsQuestTag(QuestStatusTag))
	{
		return false;
	}

	// ensure we have the tag before removing it
	if (QuestStatusData.HasTagExact(QuestStatusTag))
	{
		QuestStatusData.RemoveTag(QuestStatusTag);
		return true;
	}

	// tag is not present, so return false
	return false;
}

bool UQuestManagerComponent::HasQuestStatusTag(FGameplayTag QuestStatusTag) const
{
	// ensure the queried tag is valid
	if (!IsQuestTag(QuestStatusTag))
	{
		return false;
	}

	// query quest status for the tag
	return QuestStatusData.HasTagExact(QuestStatusTag);
}

bool UQuestManagerComponent::DoQuestStatusQuery(const FGameplayTagQuery& QuestStatusQuery) const
{
	// ensure the query is not empty
	if (QuestStatusQuery.IsEmpty())
	{
		return false;
	}

	// run the query and return the result
	return QuestStatusData.MatchesQuery(QuestStatusQuery);
}

bool UQuestManagerComponent::AddQuestQuantity(FGameplayTag Tag, int32 CountDelta /*= 0*/)
{
	// ensure this is a valid tag and count
	if (IsQuestTag(Tag) && CountDelta != 0)
	{
		// get the previous count
		const int32 PreviousCount = GetQuestQuantity(Tag);

		int32 UpdatedCount = 0;

		// update and check for overflow
		if (!FMath::AddAndCheckForOverflow(PreviousCount, CountDelta, UpdatedCount))
		{
			// clamp to int max/0
			if (CountDelta > 0)
			{
				UpdatedCount = INT_MAX;
			}
			else
			{
				UpdatedCount = 0;
			}
		}
		else
		{
			// ensure we don't go below zero
			UpdatedCount = FMath::Max(UpdatedCount, 0);
		}

		// do we still have stacks left?
		if (UpdatedCount > 0)
		{
			// update the container
			QuestQuantityData.Add(Tag, UpdatedCount);
		}
		else
		{
			// remove the tag from the container
			QuestQuantityData.Remove(Tag);
		}

		// return true if the count has changed
		return PreviousCount != UpdatedCount;
	}

	return false;
}

int32 UQuestManagerComponent::GetQuestQuantity(FGameplayTag Tag)
{
	// ensure this is a valid tag
	if (IsQuestTag(Tag))
	{
		// find and return the quantity
		if (int32* Qty = QuestQuantityData.Find(Tag))
		{
			return *Qty;
		}
	}

	// return zero if the tag is invalid or the quantity was not found
	return 0;
}

void UQuestManagerComponent::SendQuestEvent(FGameplayTag QuestEventTag, const FQuestEventPayload& EventPayload)
{
	// build the StateTree event
	FStateTreeEvent QuestEvent;
	QuestEvent.Tag = QuestEventTag;
	QuestEvent.Payload = FInstancedStruct::Make(EventPayload);

	// send the event to each active quest StateTree
	for (auto& Current : ActiveQuestStateTreeComponents)
	{
		if (UStateTreeComponent* CurrentStateTree = Current.Value)
		{
			CurrentStateTree->SendStateTreeEvent(QuestEvent);
		}
	}
}

bool UQuestManagerComponent::IsQuestActive(FGameplayTag QuestTag) const
{
	// ensure the queried tag is valid
	if (!IsQuestTag(QuestTag))
	{
		return false;
	}

	// query active quests for the tag
	return ActiveQuests.HasTagExact(QuestTag);
}

bool UQuestManagerComponent::IsQuestCompleted(FGameplayTag QuestTag) const
{
	// ensure the queried tag is valid
	if (!IsQuestTag(QuestTag))
	{
		return false;
	}

	// query completed quests for the tag
	return CompletedQuests.HasTagExact(QuestTag);
}

bool UQuestManagerComponent::IsQuestFailed(FGameplayTag QuestTag) const
{
	// ensure the queried tag is valid
	if (!IsQuestTag(QuestTag))
	{
		return false;
	}

	// query failed quests for the tag
	return FailedQuests.HasTagExact(QuestTag);
}

bool UQuestManagerComponent::IsQuestStepCompleted(const FQuestStep& QuestStep)
{
	// does this step have a valid 
	if (IsQuestTag(QuestStep.StepCompletedTag))
	{
		// return true if we have the quest status tag that identifies this step as completed
		return HasQuestStatusTag(QuestStep.StepCompletedTag);
	}

	return false;
}

const TMap<FGameplayTag, int32>& UQuestManagerComponent::GetQuestQuantityData() const
{
	return QuestQuantityData;
}

const FGameplayTagContainer& UQuestManagerComponent::GetActiveQuestTags() const
{
	return ActiveQuests;
}

void UQuestManagerComponent::GetActiveQuestTagsArray(TArray<FGameplayTag>& OutTags)
{
	return MakeQuestTagsArray(ActiveQuests, OutTags);
}

const FGameplayTagContainer& UQuestManagerComponent::GetCompletedQuestTags() const
{
	return CompletedQuests;
}

void UQuestManagerComponent::GetCompletedQuestTagsArray(TArray<FGameplayTag>& OutTags)
{
	return MakeQuestTagsArray(CompletedQuests, OutTags);
}

const FGameplayTagContainer& UQuestManagerComponent::GetFailedQuestTags() const
{
	return FailedQuests;
}

void UQuestManagerComponent::GetFailedQuestTagsArray(TArray<FGameplayTag>& OutTags)
{
	return MakeQuestTagsArray(FailedQuests, OutTags);
}

const FGameplayTagContainer& UQuestManagerComponent::GetQuestStatusData() const
{
	return QuestStatusData;
}

void UQuestManagerComponent::OnStateTreeStatusChanged(EStateTreeRunStatus RunStatus)
{
	// has this comp succeeded or failed?
	if (RunStatus == EStateTreeRunStatus::Succeeded || RunStatus == EStateTreeRunStatus::Failed)
	{

		for (auto& CurrentStateTree : ActiveQuestStateTreeComponents)
		{
			TObjectPtr<UStateTreeComponent> CurrentComp = CurrentStateTree.Value;

			if (CurrentComp->GetStateTreeRunStatus() == RunStatus)
			{
				// remove the quest tag from active quests and add it to completed or failed
				ActiveQuests.RemoveTag(CurrentStateTree.Key);

				if (RunStatus == EStateTreeRunStatus::Succeeded)
				{
					CompletedQuests.AddTag(CurrentStateTree.Key);
				}
				else
				{
					FailedQuests.AddTag(CurrentStateTree.Key);
				}

				// call the delegate
				OnQuestCompleted.Broadcast(CurrentStateTree.Key, CurrentComp->GetStateTreeRunStatus() == EStateTreeRunStatus::Succeeded);

				// destroy and remove the StateTree component from the Actor
				CurrentComp->DestroyComponent();
				ActiveQuestStateTreeComponents.Remove(CurrentStateTree.Key);

				return;
			}
		}
	}
}

void UQuestManagerComponent::SetQuestStatusData(const FGameplayTagContainer& QuestStatusTags)
{
	if (!QuestStatusData.IsEmpty())
	{
		UE_LOG(LogQuest, Warning, TEXT("UQuestManagerComponent: Setting Quest Status Data when container is not empty. All existing data will be overwritten."));
		QuestStatusData.Reset();
	}

	// copy the tag container
	QuestStatusData = QuestStatusTags;
}

void UQuestManagerComponent::SetQuestQuantityData(const TMap<FGameplayTag, int32>& QuestQuantities)
{
	if (!QuestQuantityData.IsEmpty())
	{
		UE_LOG(LogQuest, Warning, TEXT("UQuestManagerComponent: Setting Quest Quantity Data when container is not empty. All existing data will be overwritten."));

		// ensure the quantity data is cleared
		QuestQuantityData.Reset();
	}

	// copy the tag container
	QuestQuantityData = QuestQuantities;
}

void UQuestManagerComponent::SetCompletedQuests(const FGameplayTagContainer& QuestTags)
{
	if (!CompletedQuests.IsEmpty())
	{
		UE_LOG(LogQuest, Warning, TEXT("UQuestManagerComponent: Setting Completed Quests when container is not empty. All existing data will be overwritten."));
		CompletedQuests.Reset();
	}

	// copy the tag container
	CompletedQuests = QuestTags;
}

void UQuestManagerComponent::SetFailedQuests(const FGameplayTagContainer& QuestTags)
{
	if (!FailedQuests.IsEmpty())
	{
		UE_LOG(LogQuest, Warning, TEXT("UQuestManagerComponent: Setting Failed Quests when container is not empty. All existing data will be overwritten."));
		FailedQuests.Reset();
	}

	// copy the tag container
	FailedQuests = QuestTags;
}

void UQuestManagerComponent::SetAndInitializeActiveQuests(const FGameplayTagContainer& QuestTags)
{
	if (!ActiveQuests.IsEmpty())
	{
		UE_LOG(LogQuest, Error, TEXT("UQuestManagerComponent: Setting Active Quests when container is not empty. The container will be overwritten, and quest StateTrees may have been orphaned."));
		ActiveQuests.Reset();
	}

	// reset the tag container
	ActiveQuests.Reset();

	// sync up any active quests that were not included in the data
	for (auto CurrentRunningQuest : ActiveQuestStateTreeComponents)
	{
		ActiveQuests.AddTag(CurrentRunningQuest.Key);
	}

	// attempt to start all quests. This will add any not already active to ActiveQuests
	for (const FGameplayTag& CurrentTag : QuestTags)
	{
		StartQuest(CurrentTag, false);
	}
}

bool UQuestManagerComponent::IsQuestTag(const FGameplayTag& Tag) const
{
	// ensure the tag's parent is the quest root
	return Tag.MatchesTag(TAG_Quest_QuestID);
}

void UQuestManagerComponent::MakeQuestTagsArray(const FGameplayTagContainer& TagsContainer, TArray<FGameplayTag>& OutTags)
{
	OutTags.Empty();

	// get the Quest subsystem from the world
	if (UWorld* World = GetWorld())
	{
		if (UQuestSubsystem* QuestSubsystem = World->GetSubsystem<UQuestSubsystem>())
		{
			// iterate through the tags in the container. This will include non-leaf nodes
			for (const FGameplayTag& Current : TagsContainer)
			{
				// check if the tag maps to a quest
				if(UQuestDefinition* QuestDef = QuestSubsystem->GetQuestDefinition(Current))
				{
					// add the tag to the out array
					OutTags.Add(Current);
				}
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE