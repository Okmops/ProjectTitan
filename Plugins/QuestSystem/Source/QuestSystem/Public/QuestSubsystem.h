// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameplayTagContainer.h"
#include "QuestSubsystem.generated.h"

class UQuestDefinition;

/**
 *  UQuestSubsystem
 *  Maintains an index of all available UQuestDefinition assets and their associated tags.
 */
UCLASS()
class QUESTSYSTEM_API UQuestSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
protected:

	/** Quest Definitions map, indexed by tag */
	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<UQuestDefinition>> QuestDefinitions;

public:

	/** Builds the Quest Definition Map on initialization */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Clears the Quest Definition Map on deinitialization */
	virtual void Deinitialize() override;

protected:

	UFUNCTION()
	void BuildQuestMap();

public:

	/** Finds an Quest definition with the given tag. Returns nullptr if no definition is found. */
	UFUNCTION(BlueprintCallable, Category="Inventory")
	UQuestDefinition* GetQuestDefinition(FGameplayTag QuestTag) const;
};
