// Copyright Epic Games, Inc. All Rights Reserved.


#include "GAS/AbilitySet.h"
#include "GAS/SampleAbilitySystemComponent.h"
#include "GAS/SampleGameplayAbility.h"
#include "GAS/SampleGameplayEffect.h"
#include "SampleLogging.h"

void FAbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
	if (Handle.IsValid())
	{
		AbilitySpecHandles.Add(Handle);
	}
}

void FAbilitySet_GrantedHandles::AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle)
{
	if (Handle.IsValid())
	{
		GameplayEffectHandles.Add(Handle);
	}
}

void FAbilitySet_GrantedHandles::AddAttributeSet(UAttributeSet* Set)
{
	if (IsValid(Set))
	{
		GrantedAttributeSets.Add(Set);
	}
}

void FAbilitySet_GrantedHandles::TakeFromAbilitySystem(USampleAbilitySystemComponent* ASC)
{
	check(ASC);

	// don't grant or remove ability sets unless the actor has authority
	if (!ASC->IsOwnerActorAuthoritative())
	{
		return;
	}

	// remove all abilities from the ASC
	for (const FGameplayAbilitySpecHandle& Handle : AbilitySpecHandles)
	{
		if (Handle.IsValid())
		{
			ASC->ClearAbility(Handle);
		}
	}

	// remove all gameplay effects from the ASC
	for (const FActiveGameplayEffectHandle& Handle : GameplayEffectHandles)
	{
		if (Handle.IsValid())
		{
			ASC->RemoveActiveGameplayEffect(Handle);
		}
	}

	// remove all attribute sets from the ASC
	for (UAttributeSet* Set : GrantedAttributeSets)
	{
		ASC->RemoveSpawnedAttribute(Set);
	}

	AbilitySpecHandles.Reset();
	GameplayEffectHandles.Reset();
	GrantedAttributeSets.Reset();
}

UAbilitySet::UAbilitySet(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

void UAbilitySet::GiveToAbilitySystem(USampleAbilitySystemComponent* ASC, FAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject) const
{
	check(ASC);

	// don't grant or remove ability sets unless the actor has authority
	if (!ASC->IsOwnerActorAuthoritative())
	{
		return;
	}

	// grant the attribute sets
	for (int32 SetIndex = 0; SetIndex < GrantedAttributeSets.Num(); ++SetIndex)
	{
		const FAbilitySet_AttributeSet& SetToGrant = GrantedAttributeSets[SetIndex];

		// check the ability set is valid
		if (!SetToGrant.AttributeSet)
		{
			UE_LOG(LogSampleAbilitySystem, Error, TEXT("GrantedAttributes[%d] on ability set [%s] is not valid"), SetIndex, *GetNameSafe(this));
			continue;
		}

		// create the attribute set and give it to the ASC
		if (UAttributeSet* NewSet = NewObject<UAttributeSet>(ASC->GetOwner(), SetToGrant.AttributeSet))
		{
			ASC->AddAttributeSetSubobject(NewSet);

			// save the handle
			if (OutGrantedHandles)
			{
				OutGrantedHandles->AddAttributeSet(NewSet);
			}
		}

	}

	// grant the gameplay effects
	for (int32 EffectIndex = 0; EffectIndex < GrantedGameplayEffects.Num(); ++EffectIndex)
	{
		const FAbilitySet_GameplayEffect& EffectToGrant = GrantedGameplayEffects[EffectIndex];

		if (!EffectToGrant.GameplayEffect)
		{
			UE_LOG(LogSampleAbilitySystem, Error, TEXT("GrantedGameplayEffects[%d] on ability set [%s] is not valid"), EffectIndex, *GetNameSafe(this));
			continue;
		}

		// get the CDO and grant the GE
		const USampleGameplayEffect* GameplayEffect = EffectToGrant.GameplayEffect->GetDefaultObject<USampleGameplayEffect>();
		const FActiveGameplayEffectHandle GameplayEffectHandle = ASC->ApplyGameplayEffectToSelf(GameplayEffect, EffectToGrant.EffectLevel, ASC->MakeEffectContext());

		// save the handle
		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddGameplayEffectHandle(GameplayEffectHandle);
		}
	}

	// grant the gameplay abilities
	for (int32 AbilityIndex = 0; AbilityIndex < GrantedGameplayAbilities.Num(); ++AbilityIndex)
	{
		const FAbilitySet_GameplayAbility& AbilityToGrant = GrantedGameplayAbilities[AbilityIndex];

		// ensure the ability is valid
		if (!AbilityToGrant.Ability)
		{
			UE_LOG(LogSampleAbilitySystem, Error, TEXT("GrantedGameplayAbilities[%d] on ability set [%s] is not valid."), AbilityIndex, *GetNameSafe(this));
			continue;
		}

		// get the CDO and build the ability spec
		USampleGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<USampleGameplayAbility>();

		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
		AbilitySpec.SourceObject = SourceObject;

		const FGameplayAbilitySpecHandle AbilitySpecHandle = ASC->GiveAbility(AbilitySpec);

		// save the handle
		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
		}

	}

}