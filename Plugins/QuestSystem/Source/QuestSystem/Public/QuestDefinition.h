// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "QuestDefinition.generated.h"

class UStateTree;

USTRUCT(BlueprintType)
struct FQuestStep
{
	GENERATED_BODY()

	/** Identifies the specific quest step. Each step should have its own ID, usually as a sub-tag to the Quest ID. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Quest Step", meta = (Categories = "QuestSystem.Quest"))
	FGameplayTag QuestStepTag;

	/** Description text to show for the quest step. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Quest Step")
	FText Description;

	/** If this tag is present in quest status data, this step is considered completed. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Quest Step", meta = (Categories = "QuestSystem.Quest"))
	FGameplayTag StepCompletedTag;

	/** Optional quest quantity tag used to display step progress. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Quest Step", meta = (Categories = "QuestSystem.Quest"))
	FGameplayTag QuestStepProgressQuantityTag;

	/** Text pattern to format the progress quantity for display. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Quest Step")
	FText ProgressFormat;
};

/**
 *  UQuestDefinition
 *  Represents an individual Quest.
 *  Quest behavior is implemented in an associated StateTree
 *  Quest progression data is stored in a QuestComponent
 */
UCLASS(Blueprintable)
class QUESTSYSTEM_API UQuestDefinition : public UDataAsset
{
	GENERATED_BODY()
	
public:

	/** Constructor */
	UQuestDefinition(const FObjectInitializer& ObjectInitializer);

protected:

	/** Player-facing name for this quest, displayed in HUD notifications and menus. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Quest")
	FText GameplayName;

	/** Gameplay Tag that uniquely identifies this quest. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Quest", meta = (Categories = "QuestSystem.Quest"))
	FGameplayTag QuestTag;

	/** StateTree that executes the quest logic */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Quest")
	TObjectPtr<UStateTree> QuestStateTree;

	/** List of step descriptions for this quest. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Quest")
	TArray<FQuestStep> QuestSteps;

public:

	/** Returns the tag that identifies this quest */
	UFUNCTION(BlueprintPure, Category="Quest")
	const FGameplayTag& GetQuestTag() const;

	/** Returns the StateTree associated with this quest */
	UFUNCTION(BlueprintPure, Category="Quest")
	UStateTree* GetStateTree() const;

	/** Finds and returns a quest step identified by the provided GameplayTag, if it exists. */
	UFUNCTION(BlueprintCallable, Category="Quest")
	bool FindQuestStep(FGameplayTag QuestStepTag, FQuestStep& OutStep) const;

	/** Returns all quest step descriptions associated with this quest. */
	UFUNCTION(BlueprintPure, Category="Quest")
	const TArray<FQuestStep>& GetQuestSteps() const;

#if WITH_EDITOR

public:

	/** Validates the asset prior to saving */
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;

#endif // WITH_EDITOR
};