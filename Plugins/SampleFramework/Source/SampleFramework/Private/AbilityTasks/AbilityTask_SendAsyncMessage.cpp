// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilityTasks/AbilityTask_SendAsyncMessage.h"
#include "AsyncGameplayMessageSystem.h"
#include "AsyncMessageWorldSubsystem.h"
#include "SampleLogging.h"

UAbilityTask_SendAsyncMessage* UAbilityTask_SendAsyncMessage::SendAsyncMessage(UGameplayAbility* OwningAbility, FName TaskInstanceName, FAsyncMessageId MessageID, const FInstancedStruct& Payload)
{
	// create the task instance
	UAbilityTask_SendAsyncMessage* MyObj = NewAbilityTask<UAbilityTask_SendAsyncMessage>(OwningAbility, TaskInstanceName);

	// save the message ID
	MyObj->SentMessageID = MessageID;

	// save the payload
	MyObj->SentPayload = Payload;

	return MyObj;
}

void UAbilityTask_SendAsyncMessage::Activate()
{
	Super::Activate();

	// get the subsystem
	if (TSharedPtr<FAsyncGameplayMessageSystem> MessageSystem = UAsyncMessageWorldSubsystem::GetSharedMessageSystem<FAsyncGameplayMessageSystem>(GetWorld()))
	{
		// queue the message, passing the payload
		MessageSystem->QueueMessageForBroadcast(SentMessageID, SentPayload);
	}
	else
	{
		UE_LOG(LogSampleAbilitySystem, Error, TEXT("[%s]: Async Message subsystem is not available. The message will not be sent."), *GetName());
	}

	// since this is a one-shot, end the task so it can be retriggered by BP logic if needed
	EndTask();
}
