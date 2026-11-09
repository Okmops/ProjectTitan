// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GAS/SampleAttributeSet.h"
#include "TitanAttributeSet_Currency.generated.h"

/**
 *  UTitanAttributeSet_Currency
 *  Implements various types of game currency.
 *  - Gold
 */
UCLASS()
class TITAN_API UTitanAttributeSet_Currency : public USampleAttributeSet
{
	GENERATED_BODY()
	
public:

	/** Constructor */
	UTitanAttributeSet_Currency();

public:

	/** Getters and setters for the attributes */
	ATTRIBUTE_ACCESSORS_BASIC(UTitanAttributeSet_Currency, Gold);
	ATTRIBUTE_ACCESSORS_BASIC(UTitanAttributeSet_Currency, MaxGold);
	ATTRIBUTE_ACCESSORS_BASIC(UTitanAttributeSet_Currency, GoldLost);
	ATTRIBUTE_ACCESSORS_BASIC(UTitanAttributeSet_Currency, GoldEarned);

	/** Delegate when gold changes */
	UPROPERTY(BlueprintAssignable, Category = "Titan")
	mutable FSampleAttributeEvent OnGoldChanged;

	/** Delegate when gold cap changes */
	UPROPERTY(BlueprintAssignable, Category = "Titan")
	mutable FSampleAttributeEvent OnMaxGoldChanged;

protected:

	/** Override to modify incoming attribute changes prior to applying them */
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;

	/** Override to convert meta-attributes into attribute changes, and call the delegates */
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	/** Override to clamp attributes */
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;

	/** Override to clamp attributes */
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	/** Override to handle attribute clamping and depletion checks */
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	/** Clamps an attribute to its min and max values */
	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

private:

	/** Current gold value, capped at MaxGold. This is hidden from modifiers to prevent it from being set directly. */
	UPROPERTY(BlueprintReadOnly, Category = "Titan|Currency", Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Gold;

	/** Gold cap. This is not hidden from modifiers so GEs can increase or lower it. */
	UPROPERTY(BlueprintReadOnly, Category = "Titan|Currency", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxGold;

	/** Temporarily hold gold value in case they are modified before the attribute value is actually changed */
	float GoldBeforeAttributeChange = 0.0f;

	/** Temporarily hold max gold value in case they are modified before the attribute value is actually changed */
	float MaxGoldBeforeAttributeChange = 0.0f;

	// Meta-Attributes
	// These do not hold stateful values, but modify currency internally.

	/** Lost gold currency. This is mapped to a gold decrease. */
	UPROPERTY(BlueprintReadOnly, Category="Titan|Currency", Meta=(AllowPrivateAccess = true))
	FGameplayAttributeData GoldLost;

	/** Earned gold currency. This is mapped to a gold increase. */
	UPROPERTY(BlueprintReadOnly, Category="Titan|Currency", Meta=(AllowPrivateAccess = true))
	FGameplayAttributeData GoldEarned;
};
