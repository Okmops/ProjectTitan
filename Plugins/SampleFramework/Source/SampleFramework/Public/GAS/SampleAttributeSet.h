// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "SampleAttributeSet.generated.h"

/** 
 * Delegate used to broadcast attribute events, some of these parameters may be null on clients depending on replication
 * @param EffectInstigator	The original instigating actor for this event
 * @param EffectCauser		The physical actor that caused the change
 * @param EffectSpec		The full effect spec for this change
 * @param EffectMagnitude	The raw magnitude, this is before clamping
 * @param OldValue			The value of the attribute before it was changed
 * @param NewValue			The value after it was changed
*/
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FSampleAttributeEvent, AActor*, EffectInstigator, AActor*, EffectCauser, const FGameplayEffectSpec&, EffectSpec, float, EffectMagnitude, float, OldValue, float, NewValue);

/**
 *  Simpler delegate used to broadcast attribute changes.
 *  @param OldValue			The value of the attribute before it was changed
 *  @param NewValue			The value after it was changed
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSampleAttributeChangedEvent, float, OldValue, float, NewValue);

/**
 *  USampleAttributeSet
 *  Base class for Attribute Sets using the Sample gameplay framework.
 */
UCLASS()
class SAMPLEFRAMEWORK_API USampleAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:

	USampleAttributeSet();

protected:

	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
		
};
