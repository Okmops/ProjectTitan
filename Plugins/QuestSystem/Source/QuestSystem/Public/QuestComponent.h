// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"
#include "GameplayEffectTypes.h"
#include "QuestDefinition.h"
#include "QuestComponent.generated.h"

/** Root tag for Quest IDs */
QUESTSYSTEM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Quest_QuestID);

/** Root tag for Quest Events */
QUESTSYSTEM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Quest_QuestEvent);

/** Quest delegates */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FQuestStartedDelegate, const FGameplayTag&, QuestTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FQuestCompletedDelegate, const FGameplayTag&, QuestTag, bool, bSucceeded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FQuestUpdatedDelegate, const FGameplayTag&, QuestTag, const FGameplayTag&, QuestStepTag, const FText&, FormattedProgressText, bool, bCompleted);

class UStateTreeComponent;

/**
 *  FQuestEventPayload
 *  Holds optional user-specified data to be included with a Quest StateTree Event
 */
USTRUCT(BlueprintType)
struct QUESTSYSTEM_API FQuestEventPayload
{
	GENERATED_BODY()

	/** Optional text data to add to the event */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Event Data")
	FText TextData;

	/** Optional int data to add to the event */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Event Data")
	int32 IntData = 0;

	/** Optional float data to add to the event */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Event Data")
	float FloatData = 0.0f;

	/** Optional vector data to add to the event */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Event Data")
	FVector VectorData = FVector::ZeroVector;
};

/**
 *  FQuestUpdateData
 *  Optional data passed along a Quest Update HUD Notification for auto-formatting.
 */
USTRUCT(BlueprintType)
struct QUESTSYSTEM_API FQuestUpdateData
{
	GENERATED_BODY()

	/** Optional list of Float values passed to this quest update. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest Update Data")
	TArray<float> FloatValues;

	/** Optional list of Int values passed to this quest update. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest Update Data")
	TArray<int32> IntValues;

	/** Optional list of Text values passed to this quest update. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest Update Data")
	TArray<FText> TextValues;
};

/**
 *  UQuestManagerComponent
 *  Manages Quest data associated to a given Actor.
 *  Quests are identified through a GameplayTag and retrieved through the Quest Subsystem
 *  Quest status data is stored as Gameplay Tags and can be queried.
 *  Each quest sub-step should be represented by an individual Gameplay Tag stored in Quest Status Data.
 *  All active Quest logic is implemented through their associated StateTrees.
 *  Active quests spawn an individual UStateTreeComponent to run it.
 *  Active quests automatically complete or fail when their StateTree succeeds or fails.
 *  Active quests can also be finished manually with a function call.
 */
UCLASS(ClassGroup = ("Quest System"), meta = (BlueprintSpawnableComponent))
class QUESTSYSTEM_API UQuestManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	/** Constructor*/
	UQuestManagerComponent(const FObjectInitializer& ObjectInitializer);

protected:

	/** Holds tag IDs for all quests currently active */
	FGameplayTagContainer ActiveQuests;

	/** Holds tag IDs for all quests already completed */
	FGameplayTagContainer CompletedQuests;

	/** Holds tag IDs for all failed quests */
	FGameplayTagContainer FailedQuests;

	/** Holds Boolean quest progression data for all quests. E.g. "has this step been started or completed?" */
	FGameplayTagContainer QuestStatusData;

	/** Holds quantitative quest progression data for all quests. E.g. "how many MacGuffins have been collected?" */
	TMap<FGameplayTag, int32> QuestQuantityData;

	/** Holds all components actively */
	UPROPERTY(Transient)
	TMap<FGameplayTag, TObjectPtr<UStateTreeComponent>> ActiveQuestStateTreeComponents;

public:

	/** Called when a quest is started */
	UPROPERTY(BlueprintAssignable, Category="Quest")
	FQuestStartedDelegate OnQuestStarted;

	/** Called when a quest is completed or failed */
	UPROPERTY(BlueprintAssignable, Category="Quest")
	FQuestCompletedDelegate OnQuestCompleted;

	/** Called when a quest is updated to a new status step */
	UPROPERTY(BlueprintAssignable, Category="Quest")
	FQuestUpdatedDelegate OnQuestUpdated;

public:

	/** Attempts to start the Quest with the given ID. Returns true if it was successfully created and initialized. */
	UFUNCTION(BlueprintCallable, Category="Quest")
	bool StartQuest(FGameplayTag QuestTag, bool bSendQuestStarted = true);

	/** Attempts to finish the Quest with the given ID, and adds it to the Completed or Failed list depending on its completion status. Returns true if the quest was successfully finished. */
	UFUNCTION(BlueprintCallable, Category="Quest")
	bool FinishQuest(FGameplayTag QuestTag, bool bQuestSucceeded = true);

	/** Attempts to send a quest update event for the given quest and step. Returns true if the update was successfully broadcast, false otherwise. */
	UFUNCTION(BlueprintCallable, Category="Quest")
	bool UpdateQuest(FGameplayTag QuestTag, FGameplayTag QuestStepTag, const FQuestUpdateData& UpdateData);

	/** Adds a new Quest status tag to quest data. Returns true if the tag was not already present */
	UFUNCTION(BlueprintCallable, Category="Quest")
	bool AddQuestStatusTag(FGameplayTag QuestStatusTag);

	/** Removes a Quest status tag from quest data. Returns true if the tag was present and removed */
	UFUNCTION(BlueprintCallable, Category="Quest")
	bool RemoveQuestStatusTag(FGameplayTag QuestStatusTag);

	/** Returns true if the Quest status data contains the exact tag queried. */
	UFUNCTION(BlueprintCallable, Category="Quest")
	bool HasQuestStatusTag(FGameplayTag QuestStatusTag) const;

	/** Runs the provided Gameplay Tag Query on Quest Status data. */
	UFUNCTION(BlueprintCallable, Category="Quest")
	bool DoQuestStatusQuery(const FGameplayTagQuery& QuestStatusQuery) const;

	/** Adds or removes the quantity identified by the Tag from quest data. */
	UFUNCTION(BlueprintCallable, Category="Quest")
	bool AddQuestQuantity(FGameplayTag Tag, int32 CountDelta = 0);

	/** Returns the current count for the quantity identified by the Tag from quest data. */
	UFUNCTION(BlueprintCallable, Category="Quest")
	int32 GetQuestQuantity(FGameplayTag Tag);

	/** Sends an event to all active Quest StateTrees */
	UFUNCTION(BlueprintCallable, Category="Quest")
	void SendQuestEvent(FGameplayTag QuestEventTag, const FQuestEventPayload& EventPayload);

	/** Returns true if the Quest with the given ID is currently active. */
	UFUNCTION(BlueprintCallable, Category="Quest")
	bool IsQuestActive(FGameplayTag QuestTag) const;

	/** Returns true if the Quest with the given ID has been completed. */
	UFUNCTION(BlueprintCallable, Category="Quest")
	bool IsQuestCompleted(FGameplayTag QuestTag) const;

	/** Returns true if the Quest with the given ID has failed. */
	UFUNCTION(BlueprintCallable, Category="Quest")
	bool IsQuestFailed(FGameplayTag QuestTag) const;

	/** Returns true if the Quest Step is considered completed. */
	UFUNCTION(BlueprintCallable, Category="Quest")
	bool IsQuestStepCompleted(const FQuestStep& QuestStep);

	/** Returns all active quests as a Gameplay Tag Container. Note that not all tags may have associated quests, e.g. non-leaf tags. */
	UFUNCTION(BlueprintPure, Category="Quest")
	const FGameplayTagContainer& GetActiveQuestTags() const;

	/** Returns all active quests as an array of Gameplay Tags. */
	UFUNCTION(BlueprintCallable, Category="Quest")
	void GetActiveQuestTagsArray(TArray<FGameplayTag>& OutTags);
	
	/** Returns all completed quests as a Gameplay Tag Container. Note that not all tags may have associated quests, e.g. non-leaf tags. */
	UFUNCTION(BlueprintPure, Category="Quest")
	const FGameplayTagContainer& GetCompletedQuestTags() const;

	/** Returns all completed quests as an array of Gameplay Tags. */
	UFUNCTION(BlueprintCallable, Category="Quest")
	void GetCompletedQuestTagsArray(TArray<FGameplayTag>& OutTags);

	/** Returns all failed quests as a Gameplay Tag Container. Note that not all tags may have associated quests, e.g. non-leaf tags. */
	UFUNCTION(BlueprintPure, Category="Quest")
	const FGameplayTagContainer& GetFailedQuestTags() const;

	/** Returns all failed quests as an array of Gameplay Tags. */
	UFUNCTION(BlueprintCallable, Category="Quest")
	void GetFailedQuestTagsArray(TArray<FGameplayTag>& OutTags);

	/** Returns currently held quest status data as read-only. If you want to edit the data, use the Add/Remove Quest Status Tag functions instead. */
	UFUNCTION(BlueprintPure, Category="Quest")
	const FGameplayTagContainer& GetQuestStatusData() const;

	/** Returns the currently held quest quantity data as read-only. If you want to edit the data, use the Add Quest Quantity function instead. NOTE: this is not supported in BP! */
	const TMap<FGameplayTag, int32>& GetQuestQuantityData() const;

protected:

	/** Handles run status change events for all quest StateTree comps. */
	UFUNCTION()
	void OnStateTreeStatusChanged(EStateTreeRunStatus RunStatus);

	// data sync setters, used to rebuild quest state, e.g. during savegame load
	// NOTE: use these with care! These can desync quest logic if called at the wrong time.

public:

	/** Copies the quest status data from an external container. Overwrites any existing data. */
	UFUNCTION(BlueprintCallable, Category="Quest")
	void SetQuestStatusData(const FGameplayTagContainer& QuestStatusTags);

	/** Copies the quest quantity data from an external container. Overwrites any existing data. NOTE: this is not supported in BP! */
	void SetQuestQuantityData(const TMap<FGameplayTag, int32>& QuestQuantities);

	/** Copies the completed quests from an external container. Overwrites any existing data. */
	UFUNCTION(BlueprintCallable, Category="Quest")
	void SetCompletedQuests(const FGameplayTagContainer& QuestTags);

	/** Copies the failed quests from an external container. Overwrites any existing data. */
	UFUNCTION(BlueprintCallable, Category="Quest")
	void SetFailedQuests(const FGameplayTagContainer& QuestTags);

	/** Copies the active quests from an external container and starts all their Quest StateTrees.
	 *  NOTE: Use with care! This is meant for active quest initialization after savegame load,
	 *  and will not end or reset any active quests or their status.
	 *  You should always call this LAST after all other quest data has been initialized.
	 */
	UFUNCTION(BlueprintCallable, Category="Quest")
	void SetAndInitializeActiveQuests(const FGameplayTagContainer& QuestTags);

private:

	/** Helper function to validate quest tags */
	bool IsQuestTag(const FGameplayTag& Tag) const;

	/** Helper function to map all quest ID gameplay tags in a container to an array */
	void MakeQuestTagsArray(const FGameplayTagContainer& TagsContainer, TArray<FGameplayTag>& OutTags);

};