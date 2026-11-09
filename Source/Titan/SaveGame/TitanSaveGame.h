// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "GameplayTagContainer.h"
#include "Framework/TitanUserSettings.h"
#include "Character/TitanSkinDescription.h"
#include "TitanSaveGame.generated.h"

/**
 *  UTitanSaveGame
 *  SaveGame class for Titan Game
 *  
 */
UCLASS()
class TITAN_API UTitanSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:

	/** Saved Active character skins for each body part */
	UPROPERTY(SaveGame)
	TMap<FGameplayTag, FTitanSkinDescription> Skins;

	/** Saved User settings */
	UPROPERTY(SaveGame, BlueprintReadWrite)
	FTitanUserSettings UserSettings;

	/** Saved Tags of currently active quests */
	UPROPERTY(SaveGame)
	FGameplayTagContainer ActiveQuests;

	/** Saved Tags of completed quests */
	UPROPERTY(SaveGame)
	FGameplayTagContainer CompletedQuests;

	/** Saved Tags of failed quests */
	UPROPERTY(SaveGame)
	FGameplayTagContainer FailedQuests;

	/** Saved Quest status tags */
	UPROPERTY(SaveGame)
	FGameplayTagContainer QuestStatusTags;

	/** Saved Quest quantities */
	UPROPERTY(SaveGame)
	TMap<FGameplayTag, int32> QuestQuantities;

	/** Saved tags and quantities of items held in the player's inventory */
	UPROPERTY(SaveGame)
	TMap<FGameplayTag, int32> InventoryTags;

	/** Saved Max Health attribute value */
	UPROPERTY(SaveGame, BlueprintReadOnly)
	float MaxHealth = 0.0f;

	/** Saved Current Gold attribute value */
	UPROPERTY(SaveGame, BlueprintReadOnly)
	float Gold = 0.0f;
};
