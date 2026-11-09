// Copyright Epic Games, Inc. All Rights Reserved.


#include "Fragments/ItemFragments_GAS.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"

bool UItemFragment_ActivateAbility::ActivateItem(const UItemDefinition* OwningItem, AActor* OwningActor, AActor* TargetActor) const
{
	// ensure the ability class is loaded
    UClass* LoadedClass = Ability.Get(); 

    if (LoadedClass == nullptr)
    {
        LoadedClass = Ability.LoadSynchronous();
    }

    if (LoadedClass)
    {
		// get the ability CDO
		if (UGameplayAbility* AbilityCDO = Cast<UGameplayAbility>(LoadedClass->GetDefaultObject()))
		{
			// get the ability system
			if (IAbilitySystemInterface* AbilityInterface = Cast<IAbilitySystemInterface>(OwningActor))
			{
				if (UAbilitySystemComponent* ASC = AbilityInterface->GetAbilitySystemComponent())
				{
					// build the ability spec
					FGameplayAbilitySpec AbilitySpec(AbilityCDO, Level, INDEX_NONE);

					// grant and activate the ability
					FGameplayAbilitySpecHandle SpecHandle = ASC->GiveAbilityAndActivateOnce(AbilitySpec);

					// return true if the ability was successfully granted and activated
					return SpecHandle.IsValid();
				}
			}
		}
    }

	return false;
}

bool UItemFragment_GameplayEvent::ActivateItem(const UItemDefinition* OwningItem, AActor* OwningActor, AActor* TargetActor) const
{
	if (EventTag.IsValid())
	{
		// get the ability system
		if (IAbilitySystemInterface* AbilityInterface = Cast<IAbilitySystemInterface>(OwningActor))
		{
			if (UAbilitySystemComponent* ASC = AbilityInterface->GetAbilitySystemComponent())
			{
				// build the payload and pass the owning item and target
				FGameplayEventData Payload;
				Payload.EventTag = EventTag;
				Payload.OptionalObject = OwningItem;
				Payload.Instigator = OwningActor;
				Payload.Target = TargetActor;

				// send the event
				ASC->HandleGameplayEvent(EventTag, &Payload);

				return true;
			}
		}
	}

	return false;
}

bool UItemFragment_GameplayCue::ActivateItem(const UItemDefinition* OwningItem, AActor* OwningActor, AActor* TargetActor) const
{
	if (GameplayCueTag.IsValid())
	{
		// get the ability system
		if (IAbilitySystemInterface* AbilityInterface = Cast<IAbilitySystemInterface>(OwningActor))
		{
			if (UAbilitySystemComponent* ASC = AbilityInterface->GetAbilitySystemComponent())
			{
				// build the cue params. Pass the owning actor as the instigator.
				FGameplayCueParameters Params;
				Params.Instigator = OwningActor;

				// trigger the gameplay cue
				ASC->ExecuteGameplayCue(GameplayCueTag, Params);
				return true;
			}
		}
	}

	return false;
}
