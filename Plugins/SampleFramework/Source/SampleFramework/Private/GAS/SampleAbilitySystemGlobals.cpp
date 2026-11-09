// Copyright Epic Games, Inc. All Rights Reserved.


#include "GAS/SampleAbilitySystemGlobals.h"
#include "GAS/SampleAbilityTypes.h"

FGameplayAbilityActorInfo* USampleAbilitySystemGlobals::AllocAbilityActorInfo() const
{
	return new FSampleGameplayAbilityActorInfo();
}