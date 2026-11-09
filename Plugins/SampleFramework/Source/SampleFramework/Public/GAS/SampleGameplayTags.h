// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "NativeGameplayTags.h"

/**
 *  SampleGameplayTags
 *  Important Gameplay Tags and utility used by the Sample Ability System
 */
namespace SampleGameplayTags
{
	/** Searches and returns a gameplay tag based on a query string. Supports partial matches (slow!) */
	SAMPLEFRAMEWORK_API	FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString = false);

	/** Native tags used by the ability system extension */
	SAMPLEFRAMEWORK_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_ActivateFail_Cooldown);
	SAMPLEFRAMEWORK_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_ActivateFail_Cost);
	SAMPLEFRAMEWORK_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_ActivateFail_TagsBlocked);
	SAMPLEFRAMEWORK_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_ActivateFail_TagsMissing);
	SAMPLEFRAMEWORK_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_ActivateFail_Networking);
	SAMPLEFRAMEWORK_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_ActivateFail_ActivationGroup);

	SAMPLEFRAMEWORK_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Input_Blocked);
	SAMPLEFRAMEWORK_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Input_Pressed);
	SAMPLEFRAMEWORK_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Input_Ongoing);
	SAMPLEFRAMEWORK_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Input_Released);

	SAMPLEFRAMEWORK_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_ActivationGroup_Blocking);
	SAMPLEFRAMEWORK_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_ActivationGroup_Replacing);
}