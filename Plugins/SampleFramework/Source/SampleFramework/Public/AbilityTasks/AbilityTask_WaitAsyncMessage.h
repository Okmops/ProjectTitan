// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AsyncMessage.h"
#include "AsyncMessageId.h"
#include "AsyncMessageHandle.h"
#include "AbilityTask_WaitAsyncMessage.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitAsyncMessageDelegate, const FAsyncMessage&, Message);

/**
 *  UAbilityTask_WaitAsyncMessage
 *	Sets up a delegate that listens for an Async Messaging system message and allows you to handle it from the ability for as long as the task is active.
 */
UCLASS()
class SAMPLEFRAMEWORK_API UAbilityTask_WaitAsyncMessage : public UAbilityTask
{
	GENERATED_BODY()
	
public:

	/** Called when the bound message is received */
	UPROPERTY(BlueprintAssignable)
	FWaitAsyncMessageDelegate OnMessageReceived;

protected:

	/** Message we want to listen for */
	FAsyncMessageId ListenedMessageID;

	/** Internal handle for the registered message listener */
	FAsyncMessageHandle MessageListenerHandle;

	/** If true, the task will be ended after the first instance of the async message is handled. */
	bool bTriggerOnce = false;

public:

	/** Sets up a handler with the Async Messaging System for the given message. */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UAbilityTask_WaitAsyncMessage* WaitAsyncMessage(UGameplayAbility* OwningAbility, FName TaskInstanceName, FAsyncMessageId MessageID, bool TriggerOnce = false);

	/** Initiates the async task */
	virtual void Activate() override;
	
	/** Cleans up the ability task on destruction */
	virtual void OnDestroy(bool AbilityIsEnding) override;

protected:

	/** Internal handler for the received async message */
	void OnAsyncMessageReceived(const FAsyncMessage& Message);
};
