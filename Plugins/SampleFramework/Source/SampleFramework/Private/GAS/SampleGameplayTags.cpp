// Copyright Epic Games, Inc. All Rights Reserved.


#include "GAS/SampleGameplayTags.h"
#include "GameplayTagsManager.h"
#include "SampleLogging.h"

namespace SampleGameplayTags
{
	// activation fail message tags
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Ability_ActivateFail_Cooldown, "Ability.ActivateFail.Cooldown", "Ability failed to activate because it is on cool down.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Ability_ActivateFail_Cost, "Ability.ActivateFail.Cost", "Ability failed to activate because it did not pass the cost checks.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Ability_ActivateFail_TagsBlocked, "Ability.ActivateFail.TagsBlocked", "Ability failed to activate because tags are blocking it.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Ability_ActivateFail_TagsMissing, "Ability.ActivateFail.TagsMissing", "Ability failed to activate because tags are missing.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Ability_ActivateFail_Networking, "Ability.ActivateFail.Networking", "Ability failed to activate because it did not pass the network checks.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Ability_ActivateFail_ActivationGroup, "Ability.ActivateFail.ActivationGroup", "Ability failed to activate because of its activation group.");

	// input tags
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Ability_Input_Blocked, "Ability.Input.Blocked", "Blocks input processing for this Ability System Component");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Ability_Input_Pressed, "Ability.Input.Pressed", "Input Pressed Event");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Ability_Input_Ongoing, "Ability.Input.Ongoing", "Input Ongoing Event");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Ability_Input_Released, "Ability.Input.Released", "Input Released Event");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Ability_ActivationGroup_Blocking, "Ability.ActivationGroup.Blocking", "Activation Group Blocking Tags");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Ability_ActivationGroup_Replacing, "Ability.ActivationGroup.Replacing", "Activation Group Replacing Tags");

	FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString)
	{
		// get the gameplay tags manager
		const UGameplayTagsManager& Manager = UGameplayTagsManager::Get();

		// search for the tag by string
		FGameplayTag Tag = Manager.RequestGameplayTag(FName(*TagString), false);

		// no direct result found, so look for partial matches
		if (!Tag.IsValid() && bMatchPartialString && !TagString.IsEmpty())
		{
			// get all the tags
			FGameplayTagContainer AllTags;
			Manager.RequestAllGameplayTags(AllTags, true);

			// check if any tag contains the partial string
			for (const FGameplayTag& TestTag : AllTags)
			{
				if (TestTag.ToString().Contains(TagString))
				{
					UE_LOG(LogSampleAbilitySystem, Display, TEXT("Could not find exact match for tag [%s] but found partial match on tag [%s]."), *TagString, *TestTag.ToString());
					Tag = TestTag;
					break;
				}
			}
		}

		return Tag;
	}
}