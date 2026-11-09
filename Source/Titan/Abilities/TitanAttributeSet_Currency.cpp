// Copyright Epic Games, Inc. All Rights Reserved.


#include "Abilities/TitanAttributeSet_Currency.h"
#include "GameplayEffectExtension.h"

UTitanAttributeSet_Currency::UTitanAttributeSet_Currency() :
	Gold(0.0f),
	MaxGold(9999.0f)
{

}

bool UTitanAttributeSet_Currency::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	// anything low level go wrong?
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}

	// TODO: infinite money cheat?

	// save the current attribute values
	GoldBeforeAttributeChange = GetGold();
	MaxGoldBeforeAttributeChange = GetMaxGold();

	return true;
}

void UTitanAttributeSet_Currency::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const float MinimumCurrency = 0.0f;

	// get the effect data
	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
	AActor* Instigator = EffectContext.GetOriginalInstigator();
	AActor* Causer = EffectContext.GetEffectCauser();

	// are we losing gold?
	if (Data.EvaluatedData.Attribute == GetGoldLostAttribute())
	{
		// convert into a gold decrease and clamp
		SetGold(FMath::Clamp(GetGold() - GetGoldLost(), MinimumCurrency, GetMaxGold()));
		SetGoldLost(0.0f);
	}
	// are we earning gold?
	else if (Data.EvaluatedData.Attribute == GetGoldEarnedAttribute())
	{
		// convert into a gold increase and clamp
		SetGold(FMath::Clamp(GetGold() + GetGoldEarned(), MinimumCurrency, GetMaxGold()));
		SetGoldEarned(0.0f);
	}
	// are we directly modifying gold?
	else if (Data.EvaluatedData.Attribute == GetGoldAttribute())
	{
		SetGold(FMath::Clamp(GetGold(), MinimumCurrency, GetMaxGold()));
	}
	// are we directly modifying max gold?
	else if (Data.EvaluatedData.Attribute == GetMaxGoldAttribute())
	{
		// notify on any requested max gold changes
		OnMaxGoldChanged.Broadcast(Instigator, Causer, Data.EffectSpec, Data.EvaluatedData.Magnitude, MaxGoldBeforeAttributeChange, GetMaxGold());
	}

	// if the gold value has actually changed, call the delegate
	if (GetGold() != GoldBeforeAttributeChange)
	{
		OnGoldChanged.Broadcast(Instigator, Causer, Data.EffectSpec, Data.EvaluatedData.Magnitude, GoldBeforeAttributeChange, GetGold());
	}
}

void UTitanAttributeSet_Currency::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	// ensure the changed value is clamped
	ClampAttribute(Attribute, NewValue);
}

void UTitanAttributeSet_Currency::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// ensure the changed value is clamped
	ClampAttribute(Attribute, NewValue);
}

void UTitanAttributeSet_Currency::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	// ensure current gold does not exceed the gold cap
	if (Attribute == GetMaxGoldAttribute())
	{
		// clamp the value through a mod application
		if (GetGold() > NewValue)
		{
			GetOwningAbilitySystemComponent()->ApplyModToAttribute(GetGoldAttribute(), EGameplayModOp::Override, NewValue);
		}
	}
}

void UTitanAttributeSet_Currency::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetGoldAttribute())
	{
		// Do not allow the attribute to go negative or above the cap
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxGold());
	}
	else if (Attribute == GetMaxGoldAttribute())
	{
		// Do not allow the gold cap to drop below 1
		NewValue = FMath::Max(NewValue, 1.0f);
	}
}
