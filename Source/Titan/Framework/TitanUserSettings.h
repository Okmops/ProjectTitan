// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TitanUserSettings.generated.h"

/**
 *  FTitanUserSettings
 *  Holds user settings for Titan gameplay
 */
USTRUCT(BlueprintType)
struct TITAN_API FTitanUserSettings
{
	GENERATED_BODY()

	/** If true, the Action Bar should be visible in the HUD */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category="Character Customization")
	bool bShowHUDActionBar = true;
};
