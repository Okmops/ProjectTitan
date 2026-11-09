// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "SampleAbilitySystemGlobals.generated.h"

struct FGameplayAbilityActorInfo;

/**
 *  USampleAbilitySystemGlobals
 *  Expanded Ability System Globals class
 *  Configures GAS to use the expanded ability structs
 */
UCLASS(config = Game)
class SAMPLEFRAMEWORK_API USampleAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()
	
protected:

	/** Override to replace for expanded AbilityActorInfo struct */
	virtual FGameplayAbilityActorInfo* AllocAbilityActorInfo() const;
};
