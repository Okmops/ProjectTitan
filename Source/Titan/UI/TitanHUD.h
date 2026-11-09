// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "TitanHUD.generated.h"

class ATitanGameplayPlayerState;
class UMaterialInterface;
class ATitanGameplayCharacter;
class IPlayerInteractionInterface;

/**
 *  UTitanHUD
 *  Native class for the Titan Gameplay HUD
 */
UCLASS(abstract)
class TITAN_API UTitanHUD : public UCommonUserWidget
{
	GENERATED_BODY()
	
protected:

	/** Pointer to the owning Player State */
	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	TObjectPtr<ATitanGameplayPlayerState> PlayerState;

public:

	/** Initializes the HUD for the owning player */
	void InitializeForPlayer(ATitanGameplayPlayerState* OwningPlayerState);

	/** Initializes the HUD for the owning character */
	void InitializeForCharacter(ATitanGameplayCharacter* OwningCharacter);

	/** Resets all the HUD character delegates during unpossess flow */
	void ResetCharacterDelegates(ATitanGameplayCharacter* OwningCharacter);

protected:

	/** Allows BP sub-widgets to initializes for the owning character */
	UFUNCTION(BlueprintImplementableEvent, Category ="HUD", DisplayName = "Initialize for Character")
	void BP_InitializeForCharacter(ATitanGameplayCharacter* OwningCharacter);

protected:

	/** Last cached stamina value. */
	float OldStamina = 0.0f;

	/** Last cached health percentage value */
	float OldHealthPct = 0.0f;

	/** Last cached health value */
	float Health = 0.0f;

	/** Last cached max health value */
	float MaxHealth = 1.0f;

	/** Last cached gold value */
	float OldGold = 0.0f;

	/** Text format used when picking items */
	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	FText ItemsFoundText;

	/** Text format used when losing items */
	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	FText ItemsLostText;

protected:

	/** Movement mode changed delegate */
	UFUNCTION()
	void OnMovementModeChanged(const FName& PreviousMode, const FName& NewMode);

	/** Stamina changed delegate */
	UFUNCTION()
	void OnStaminaChanged(float NewPct, bool bExhausted);

	/** Health changed delegate */
	UFUNCTION()
	void OnHealthChanged(AActor* EffectInstigator, AActor* EffectCauser, const FGameplayEffectSpec& EffectSpec, float EffectMagnitude, float OldValue, float NewValue);

	/** Max health changed delegate */
	UFUNCTION()
	void OnMaxHealthChanged(float OldValue, float NewValue);

	/** Gold changed delegate */
	UFUNCTION()
	void OnGoldChanged(AActor* EffectInstigator, AActor* EffectCauser, const FGameplayEffectSpec& EffectSpec, float EffectMagnitude, float OldValue, float NewValue);

	/** Inventory item count changed delegate */
	UFUNCTION()
	void OnItemCountChanged(const FGameplayTag& ItemTag, int32 CountDelta, int32 TotalCount);

	/** Quest started delegate */
	UFUNCTION()
	void OnQuestStarted(const FGameplayTag& QuestTag);

	/** Quest completed delegate */
	UFUNCTION()
	void OnQuestCompleted(const FGameplayTag& QuestTag, bool bSucceeded);

	/** Quest updated delegate */
	UFUNCTION()
	void OnQuestUpdated(const FGameplayTag& QuestTag, const FGameplayTag& QuestStepTag, const FText& NotifyText, bool bCompleted);

protected:

	/** Allows sub-widgets to update based on character movement mode changes. */
	UFUNCTION(BlueprintImplementableEvent, Category ="HUD", DisplayName = "Set Movement Mode")
	void BP_SetMovementMode(const FName& NewMode);

	/** Updates the health percentage and calls the BP events if necessary */
	void UpdateHealth();

	/** Allows sub-widgets to update the stamina value */
	UFUNCTION(BlueprintImplementableEvent, Category ="HUD", DisplayName = "Set Stamina")
	void BP_SetStamina(float NewPct, bool bExhausted);

	/** Allows sub-widgets to update the health value */
	UFUNCTION(BlueprintImplementableEvent, Category ="HUD", DisplayName = "Set Health")
	void BP_SetHealth(float NewPct);

	/** Allows sub-widgets to update the max health value */
	UFUNCTION(BlueprintImplementableEvent, Category ="HUD", DisplayName = "Set Max Health")
	void BP_SetMaxHealth(float NewValue);

	/** Allows sub-widgets to update the gold value */
	UFUNCTION(BlueprintImplementableEvent, Category ="HUD", DisplayName = "Set Gold")
	void BP_SetGold(float NewValue, float Delta);

protected:

	/** Updates and shows the target interaction cue sub-widget when a new target is selected */
	UFUNCTION()
	void OnTargetChanged(TScriptInterface<IPlayerInteractionInterface> NewTarget);

	/** Hides the interaction cue sub-widget when there's no target */
	UFUNCTION()
	void OnTargetLost();

	/** Hides the interaction cue sub-widget when an interaction starts */
	UFUNCTION()
	void OnInteractionStarted(AActor* InteractedActor);

public:

	/** Shows or hides the action bar sub-widget */
	void SetActionBarVisibility(bool bVisible);

protected:

	/** Allows sub-widgets to update the interaction cue text */
	UFUNCTION(BlueprintImplementableEvent, Category ="HUD", DisplayName = "Show Interaction Cue")
	void BP_ShowInteractionCue(const FText& InteractionText);

	/** Hides the interaction cue sub-widget */
	UFUNCTION(BlueprintImplementableEvent, Category ="HUD", DisplayName = "Hide Interaction Cue")
	void BP_HideInteractionCue();

	/** Shows or hides the action bar sub-widget */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category ="HUD", DisplayName = "Set Action Bar Visibility")
	void BP_SetActionBarVisibility(bool bVisible);

	/** Allows BP sub-widgets to add a HUD notification */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category ="HUD", DisplayName = "Add HUD Notification")
	void BP_AddHUDNotification(const FText& Notification, const TSoftObjectPtr<UMaterialInterface>& Icon);

	/** Allows BP to display a quest status notification, such as Quest Started or Quest Completed */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category ="HUD", DisplayName = "Add Quest Status")
	void BP_AddQuestStatus(const FGameplayTag& QuestTag, bool bCompleted = false, bool bFailed = false);

	/** Allows BP to display a quest update notification, such as a step being added or updated */
	UFUNCTION(BlueprintImplementableEvent, Category ="HUD", DisplayName = "Add Quest Step Update")
	void BP_AddQuestStepUpdate(const FText& QuestUpdateText, bool bCompleted);

	/** Allows BP to display a landmark discovered notification */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category ="HUD", DisplayName = "Add Landmark Text")
	void BP_AddLandmarkNotification(const FText& LandmarkText, const FText& RegionText);
};