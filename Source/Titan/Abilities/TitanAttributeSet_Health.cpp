// Copyright Epic Games, Inc. All Rights Reserved.


#include "Abilities/TitanAttributeSet_Health.h"
#include "GameplayEffectExtension.h"
#include "NativeGameplayTags.h"

// damage tags
UE_DEFINE_GAMEPLAY_TAG(TAG_Titan_DamageSelfDestruct, "Titan.Damage.SelfDestruct");
UE_DEFINE_GAMEPLAY_TAG(TAG_Titan_DamageImmunity, "Titan.Damage.Immunity");

// initialize the min allowed MaxHealth value
const float UTitanAttributeSet_Health::MaxHealthMinValue = 5.0f;

UTitanAttributeSet_Health::UTitanAttributeSet_Health() :
	Health(MaxHealthMinValue),
	MaxHealth(MaxHealthMinValue)
{
}

bool UTitanAttributeSet_Health::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	// anything low level go wrong?
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}

	// handle modifying incoming normal damage greater than zero
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		if (Data.EvaluatedData.Magnitude > 0.0f)
		{
			// is this damage the result of a self-destruct?
			const bool bIsDamageFromSelfDestruct = Data.EffectSpec.GetDynamicAssetTags().HasTagExact(TAG_Titan_DamageSelfDestruct);

			// if we have damage immunity and are not self destructing, do not decrease health
			if (Data.Target.HasMatchingGameplayTag(TAG_Titan_DamageImmunity) && !bIsDamageFromSelfDestruct)
			{
				Data.EvaluatedData.Magnitude = 0.0f;
				return false;
			}
		}
	}

	// save the current health values
	HealthBeforeAttributeChange = GetHealth();
	MaxHealthBeforeAttributeChange = GetMaxHealth();

	return true;
}

void UTitanAttributeSet_Health::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const float MinimumHealth = 0.0f;
		
	// get the effect data
	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
	AActor* Instigator = EffectContext.GetOriginalInstigator();
	AActor* Causer = EffectContext.GetEffectCauser();

	// are we receiving damage?
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		// convert into a health decrease and clamp
		SetHealth(FMath::Clamp(GetHealth() - GetDamage(), MinimumHealth, GetMaxHealth()));
		SetDamage(0.0f);
	}
	// are we receiving healing?
	else if (Data.EvaluatedData.Attribute == GetHealingAttribute())
	{
		// convert into a health increase clamp
		SetHealth(FMath::Clamp(GetHealth() + GetHealing(), MinimumHealth, GetMaxHealth()));
		SetHealing(0.0f);
	}
	// are we modifying health?
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// clamp and fall into out of health handling below
		SetHealth(FMath::Clamp(GetHealth(), MinimumHealth, GetMaxHealth()));
	}

	// if the health value has actually changed, call the delegate
	if (GetHealth() != HealthBeforeAttributeChange)
	{
		OnHealthChanged.Broadcast(Instigator, Causer, Data.EffectSpec, Data.EvaluatedData.Magnitude, HealthBeforeAttributeChange, GetHealth());
	}

	// if this is the first time we detect out of health, call the delegate
	if ((GetHealth() <= 0.0f) && !bOutOfHealth)
	{
		OnOutOfHealth.Broadcast(Instigator, Causer, Data.EffectSpec, Data.EvaluatedData.Magnitude, HealthBeforeAttributeChange, GetHealth());
	}

	// check health again in case an event above changed it.
	bOutOfHealth = (GetHealth() <= 0.0f);
}

void UTitanAttributeSet_Health::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	// ensure the changed value is clamped
	ClampAttribute(Attribute, NewValue);
}

void UTitanAttributeSet_Health::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// ensure the changed value is clamped
	ClampAttribute(Attribute, NewValue);
}

void UTitanAttributeSet_Health::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	// ensure current health does not exceed the max health
	if (Attribute == GetMaxHealthAttribute())
	{
		// notify on any requested max health changes
		OnMaxHealthChanged.Broadcast(OldValue, NewValue);

		// clamp the health value through a mod application
		if (GetHealth() > NewValue)
		{
			GetOwningAbilitySystemComponent()->ApplyModToAttribute(GetHealthAttribute(), EGameplayModOp::Override, NewValue);
		}
	}

	// reset the out of health flag if health came back up from zero
	if (bOutOfHealth && (GetHealth() > 0.0f))
	{
		bOutOfHealth = false;
	}
}

void UTitanAttributeSet_Health::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetHealthAttribute())
	{
		// Do not allow health to go negative or above max health.
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
		// Do not allow max health to drop below its min value.
		NewValue = FMath::Max(NewValue, MaxHealthMinValue);
	}
}