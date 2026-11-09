// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "QuestFunctionLibrary.generated.h"

class UQuestDefinition;

/**
 *  UQuestFunctionLibrary
 *  Utility functions to interact with the Quest System
 */
UCLASS()
class QUESTSYSTEM_API UQuestFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	/** Retrieves a Quest Definition from the Quest Subsystem if it exists */
	UFUNCTION(BlueprintPure, Category = "Inventory System", meta=(WorldContext="WorldContextObject"))
	static UQuestDefinition* GetQuestDefinition(const UObject* WorldContextObject, FGameplayTag QuestTag);
};
