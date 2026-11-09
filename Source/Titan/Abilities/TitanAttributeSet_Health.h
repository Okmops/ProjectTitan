// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GAS/SampleAttributeSet.h"
#include "TitanAttributeSet_Health.generated.h"

/**
 *  UTitanAttributeSet_Health
 *  Implements health value tracking
 *  - Clamping to MaxHealth
 *  - Damage and Healing meta-attributes
 *  - Out of Health checks and notifications
 *  - Health and MaxHealth changed notifications
 */
UCLASS()
class TITAN_API UTitanAttributeSet_Health : public USampleAttributeSet
{
	GENERATED_BODY()
	
public:

	/** Constructor */
	UTitanAttributeSet_Health();

public:

	/** Getters and setters for the attributes */
	ATTRIBUTE_ACCESSORS_BASIC(UTitanAttributeSet_Health, Health);
	ATTRIBUTE_ACCESSORS_BASIC(UTitanAttributeSet_Health, MaxHealth);
	ATTRIBUTE_ACCESSORS_BASIC(UTitanAttributeSet_Health, Healing);
	ATTRIBUTE_ACCESSORS_BASIC(UTitanAttributeSet_Health, Damage);

	/** Delegate when health changes due to damage or healing */
	UPROPERTY(BlueprintAssignable, Category = "Titan")
	mutable FSampleAttributeEvent OnHealthChanged;

	/** Delegate when max health changes */
	UPROPERTY(BlueprintAssignable, Category = "Titan")
	mutable FSampleAttributeChangedEvent OnMaxHealthChanged;

	/** Delegate to broadcast when the health attribute reaches zero */
	UPROPERTY(BlueprintAssignable, Category = "Titan")
	mutable FSampleAttributeEvent OnOutOfHealth;

protected:

	/** Override to modify incoming damage prior to applying it */
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;

	/** Override to convert meta-attributes into attribute changes, and call the delegates */
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	/** Override to clamp health value to max health */
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;

	/** Override to clamp health value to max health */
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	/** Override to handle max health clamping and out of health checks */
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	/** Clamps an attribute to its min and max values */
	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

private:

	/** Current health value, capped at MaxHealth. This is hidden from modifiers to prevent it from being set directly. */
	UPROPERTY(BlueprintReadOnly, Category = "Titan|Health", Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Health;

	/** Health cap. This is not hidden from modifiers so GEs can increase or lower it. */
	UPROPERTY(BlueprintReadOnly, Category = "Titan|Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealth;

	/** Determines if current health has reached zero */
	bool bOutOfHealth = false;

	/** Temporarily stores health value during effect processing in case the actual change has been modified by a clamp or immunity. */
	float HealthBeforeAttributeChange = 0.0f;

	/** Temporarily stores max health value during effect processing in case the actual change has been modified by a clamp or immunity. */
	float MaxHealthBeforeAttributeChange = 0.0f;

	/** Minimum allowed Max Health value */
	static const float MaxHealthMinValue;

	// Meta-Attributes
	// These do not hold stateful values, but modify health internally.

	/** Incoming healing. This is mapped to a health increase. */
	UPROPERTY(BlueprintReadOnly, Category="Titan|Health", Meta=(AllowPrivateAccess = true))
	FGameplayAttributeData Healing;

	/** Incoming damage. This is mapped to a health decrease. */
	UPROPERTY(BlueprintReadOnly, Category="Titan|Health", Meta=(AllowPrivateAccess = true))
	FGameplayAttributeData Damage;

};
