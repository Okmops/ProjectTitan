// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AsyncMessageId.h"
#include "StructUtils/InstancedStruct.h"
#include "AbilityTask_SendAsyncMessage.generated.h"

/**
 *  UAbilityTask_SendAsyncMessage
 *  Simple AbilityTask to send an Async Message immediately.
 */
UCLASS()
class SAMPLEFRAMEWORK_API UAbilityTask_SendAsyncMessage : public UAbilityTask
{
	GENERATED_BODY()
	
protected:

	/** Message we want to send */
	UPROPERTY(Transient)
	FAsyncMessageId SentMessageID;

	/** Payload we want to send */
	UPROPERTY(Transient)
	FInstancedStruct SentPayload;

public:

	/** Sends a message using the Async Messaging System, with an optional struct payload. */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UAbilityTask_SendAsyncMessage* SendAsyncMessage(UGameplayAbility* OwningAbility, FName TaskInstanceName, FAsyncMessageId MessageID, const FInstancedStruct& Payload);

	/** Initiates the async task */
	virtual void Activate() override;
};
