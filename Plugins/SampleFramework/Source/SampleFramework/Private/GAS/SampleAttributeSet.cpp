// Copyright Epic Games, Inc. All Rights Reserved.


#include "GAS/SampleAttributeSet.h"

USampleAttributeSet::USampleAttributeSet()
{
	
}

void USampleAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
}