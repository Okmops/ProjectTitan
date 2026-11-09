// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilityTasks/AbilityTask_WaitAsyncMessage.h"
#include "AsyncGameplayMessageSystem.h"
#include "AsyncMessageWorldSubsystem.h"
#include "SampleLogging.h"

UAbilityTask_WaitAsyncMessage* UAbilityTask_WaitAsyncMessage::WaitAsyncMessage(UGameplayAbility* OwningAbility, FName TaskInstanceName, FAsyncMessageId MessageID, bool TriggerOnce)
{
	// create the task instance
	UAbilityTask_WaitAsyncMessage* MyObj = NewAbilityTask<UAbilityTask_WaitAsyncMessage>(OwningAbility, TaskInstanceName);

	// save the message ID
	MyObj->ListenedMessageID = MessageID;
	MyObj->bTriggerOnce = TriggerOnce;

	return MyObj;
}

void UAbilityTask_WaitAsyncMessage::Activate()
{
	Super::Activate();

	// get the subsystem
	if (TSharedPtr<FAsyncGameplayMessageSystem> MessageSystem = UAsyncMessageWorldSubsystem::GetSharedMessageSystem<FAsyncGameplayMessageSystem>(GetWorld()))
	{
		// set the tick group
		FAsyncMessageBindingOptions Options;
		Options.SetTickGroup(TG_PostUpdateWork);

		// set up the message listener
		MessageListenerHandle = MessageSystem->BindListener(ListenedMessageID, TWeakObjectPtr<UAbilityTask_WaitAsyncMessage>(this), &UAbilityTask_WaitAsyncMessage::OnAsyncMessageReceived, Options);
	}
	else
	{
		UE_LOG(LogSampleAbilitySystem, Error, TEXT("[%s]: Failed to acquire the Async Message subsystem."), *GetName());

		EndTask();
		return;
	}
}

void UAbilityTask_WaitAsyncMessage::OnDestroy(bool AbilityIsEnding)
{
	// get the subsystem
	if (TSharedPtr<FAsyncGameplayMessageSystem> MessageSystem = UAsyncMessageWorldSubsystem::GetSharedMessageSystem<FAsyncGameplayMessageSystem>(GetWorld()))
	{
		// unbind the listener
		MessageSystem->UnbindListener(MessageListenerHandle);
	}

	Super::OnDestroy(AbilityIsEnding);
}

void UAbilityTask_WaitAsyncMessage::OnAsyncMessageReceived(const FAsyncMessage& Message)
{
	// broadcast the BP delegate
	OnMessageReceived.Broadcast(Message);

	// if we only trigger once, end the task
	if (bTriggerOnce)
	{
		EndTask();
	}
}
