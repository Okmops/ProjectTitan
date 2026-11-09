// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "TitanCheatManager.generated.h"

/**
 *  UTitanCheatManager
 *  Main cheat manager class for the Titan Player Controller
 */
UCLASS(Within=SamplePlayerController)
class TITAN_API UTitanCheatManager : public UCheatManager
{
	GENERATED_BODY()

protected:

	float TeleportMinSweepZ = -10000.0f;

	float TeleportMaxSweepZ = 40000.0f;
	
public:

	/** Teleports the player to the given map coords. */
	UFUNCTION(Exec)
	void TitanTeleportPlayer3D(float XCoord, float YCoord, float ZCoord);

	/** Teleports the player to the given map coords. Casts a ray down from the sky to find the Z coordinate */
	UFUNCTION(Exec)
	void TitanTeleportPlayer2D(float XCoord, float YCoord);

	/** Teleports the player to the landmark with the given ID */
	UFUNCTION(Exec)
	void TitanTeleportPlayerToLandmark(int32 MarkerID);

	/** Prints the current map coordinates to the log. */
	UFUNCTION(Exec)
	void TitanGetMapCoords();

	/** Enables or disables stamina on the player character. */
	UFUNCTION(Exec)
	void TitanToggleStamina();
	
	/* Adds an item to the inventory. */
	UFUNCTION(Exec)
	void TitanAddItem(FGameplayTag ItemTag, int32 Count);

	/* Removes an item from the inventory. */
	UFUNCTION(Exec)
	void TitanRemoveItem(FGameplayTag ItemTag, int32 Count);

	/* Adds a Quest Status Tag to the player to skip to specific steps or advance quests */
	UFUNCTION(Exec)
	void TitanAddQuestStatus(FGameplayTag QuestTag);

	/* Removes a Quest Status Tag from the player to clear or roll back on quests */
	UFUNCTION(Exec)
	void TitanRemoveQuestStatus(FGameplayTag QuestTag);

	/* Sends a Quest Event to advance quests */
	UFUNCTION(Exec)
	void TitanSendQuestEvent(FGameplayTag QuestEventTag);

	/* Adds quantity to a quest's Quantity Tag */
	UFUNCTION(Exec)
	void TitanAddQuestQuantity(FGameplayTag QuestQuantityTag, int32 Quantity);
};
