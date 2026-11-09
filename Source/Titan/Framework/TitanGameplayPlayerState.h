// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/SamplePlayerState.h"
#include "GameplayTagContainer.h"
#include "TitanSkinDescription.h"
#include "TitanUserSettings.h"
#include "TitanGameplayPlayerState.generated.h"

class UQuestManagerComponent;
class UInventoryManagerComponent;
class UTitanSaveGame;
class UTitanHUD;
class USkeletalMesh;
class UAnimInstance;
class ATitanGameplayCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSkinLoadedDelegate, FGameplayTag, BodyPart, USkeletalMesh*, Mesh, TSubclassOf<UAnimInstance>, AnimationBlueprint);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSkinChangedDelegate, FGameplayTag, BodyPart, USkeletalMesh*, Mesh, TSubclassOf<UAnimInstance>, AnimationBlueprint);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSettingsUpdatedDelegate, const FTitanUserSettings&, Settings);

/**
 *  ATitanGameplayPlayerState
 *  PlayerState class used during standard Titan gameplay.
 *  Provides additional features to the standard framework PlayerState:
 *  - Inventory Management
 *  - Quest Management
 *  - HUD
 *  - Player Preferences
 *  - Skin Management
 *  - SaveGame Management
 */
UCLASS(abstract)
class TITAN_API ATitanGameplayPlayerState : public ASamplePlayerState
{
	GENERATED_BODY()
	
public:

	/** Constructor */
	ATitanGameplayPlayerState(const FObjectInitializer& ObjectInitializer);

	/////////////////////////////////////
	// Initialization

public:

	/** PlayerState init */
	virtual void BeginPlay() override;

	/** PlayerState cleanup */
	virtual void Destroyed() override;

	/** Pawn initialization */
	virtual void OnPawnChanged(APlayerState* PlayerState, APawn* NewPawn, APawn* OldPawn) override;

	/////////////////////////////////////
	// Components

protected:

	/** Inventory Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Titan", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInventoryManagerComponent> Inventory;

	/** Quest Manager Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Titan", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UQuestManagerComponent> QuestManager;

public:

	/** Returns the inventory manager component */
	UFUNCTION(BlueprintPure, Category="Components")
	UInventoryManagerComponent* GetInventory() { return Inventory; }

	/** Returns the quest manager component */
	UFUNCTION(BlueprintPure, Category="Components")
	UQuestManagerComponent* GetQuestManager() { return QuestManager; }

	/////////////////////////////////////
	// Pawn Initialization

protected:

	/** Initializes a possessed character */
	void InitializeCharacter(ATitanGameplayCharacter* GameplayCharacter);

	/** Initializes the inventory manager from SaveGame data */
	void InitializeInventory();

	/** Initializes the quest manager from SaveGame data */
	void InitializeQuests();

	/////////////////////////////////////
	// HUD

protected:

	/** Pointer to the HUD widget */
	UPROPERTY()
	TObjectPtr<UTitanHUD> HUD;

	/** Type of HUD to create */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HUD")
	TSubclassOf<UTitanHUD> HUDClass;

public:

	/** Creates the HUD */
	void CreateHUD();

	/** Destroys the HUD */
	void DestroyHUD();

	/** Makes the HUD visible or invisible */
	UFUNCTION(BlueprintCallable, Category="HUD")
	void SetHUDVisibility(bool bVisible);

	/** Returns the HUD */
	UFUNCTION(BlueprintPure, Category="HUD")
	UTitanHUD* GetHUD() const;

	/////////////////////////////////////
	// SaveGame

protected:

	/** Pointer to the most recently loaded SaveGame data */
	UPROPERTY()
	TObjectPtr<UTitanSaveGame> SaveFile;

	/** Name of the SaveGame slot to use */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SaveGame")
	FString SaveSlotName = "TitanSave";

public:

	/** Load the SaveGame data or create a new file if none exists */
	UFUNCTION(BlueprintCallable, Category="SaveGame")
	UTitanSaveGame* LoadOrCreateSaveFile();

	/** Save the game file to disk */
	UFUNCTION(BlueprintCallable, Category="SaveGame")
	void SaveGame();

	/////////////////////////////////////
	// Character Skin Management

protected:

	/** Data Table holding all character skin data */
	UPROPERTY(EditDefaultsOnly, Category="Character Skins")
	TObjectPtr<UDataTable> CharacterSkinsDataTable;

	/** Internal array of character skins extracted from the data table */
	UPROPERTY()
	TArray<FTitanSkinDescription> CharacterSkins;

	/** Internal array of default character skins */
	UPROPERTY()
	TMap<FGameplayTag, FTitanSkinDescription> DefaultSkins;

	/** Internal map of default meshes for each body part. */
	UPROPERTY()
	TMap<FGameplayTag, USkeletalMesh*> DefaultMeshes;

	/** Internal map of default AnimInstances for each body part */
	UPROPERTY()
	TMap<FGameplayTag, TSubclassOf<UAnimInstance>> DefaultAnimInstances;

	/** Animation Blueprint class used by default when no overrides are set from the character skin */
	UPROPERTY(EditDefaultsOnly, Category="Character Skins")
	TSubclassOf<UAnimInstance> CopyAnimInstance;

public:

	/** Delegate called when a character skin part is loaded and available for use */
	UPROPERTY(BlueprintAssignable, Category="Character Skins")
	FSkinLoadedDelegate OnSkinLoaded;

	/** Delegate called when a character skin part has been changed. */
	UPROPERTY(BlueprintAssignable, Category="Character Skins")
	FSkinChangedDelegate OnSkinChanged;

protected:	

	/** Initializes all character skins from the Data Table */
	void InitializeCharacterSkins();

public:

	/** Populates an array with all available skins for the given body part */
	UFUNCTION(BlueprintCallable, Category="Character Skins")
	void GetAllSkinsForBodyPart(FGameplayTag BodyPart, TArray<FTitanSkinDescription>& OutDescriptions);

	/** Populates an array with all available skins in the given collection */
	UFUNCTION(BlueprintCallable, Category="Character Skins")
	void GetAllSkinsInCollection(FGameplayTag Collection, TArray<FTitanSkinDescription>& OutDescriptions);

	/** Returns the currently set default mesh and AnimInstance assets for the given body part */
	UFUNCTION(BlueprintCallable, Category="Character Skins")
	void GetDefaultMeshForPart(FGameplayTag BodyPart, USkeletalMesh*& OutMesh, TSubclassOf<UAnimInstance>& OutAnimBlueprint);

	/** Returns the skin for the given body part */
	UFUNCTION(BlueprintCallable, Category="Character Skins")
	FTitanSkinDescription GetDefaultSkinForPart(FGameplayTag BodyPart);

	/** Returns all skin descriptions set as default for each body part */
	UFUNCTION(BlueprintCallable, Category="Character Skins")
	void GetAllDefaultSkinDescriptions(TArray<FTitanSkinDescription>& OutDescriptions);

	/** Updates the default skins with the new one and calls the skin updated delegate */
	UFUNCTION(BlueprintCallable, Category="Character Skins")
	void SetSkinAsDefault(const FTitanSkinDescription& Description);
	
	/** Loads the assets for the given skin description */
	UFUNCTION(BlueprintCallable, Category="Character Skins")
	void LoadSkin(const FTitanSkinDescription& Description);

	/** Updates the saved skin description for the specified body part */
	UFUNCTION(BlueprintCallable, Category="Character Skins")
	void SetSavedSkinForBodyPart(FGameplayTag BodyPart, const FTitanSkinDescription& Skin);

protected:

	/** Reads the SaveGame's default skins and updates the defaults for each body part */
	void UpdateSkins();

private:

	/** Validates a tag as a skin collection tag */
	bool IsSkinCollectionTag(const FGameplayTag& Tag);

	/** Validates a tag as a skin body part tag */
	bool IsSkinBodyPartTag(const FGameplayTag& Tag);

	/////////////////////////////////////
	// User Settings Management

public:

	/** Called when user settings are updated. */
	UPROPERTY(BlueprintAssignable)
	FSettingsUpdatedDelegate OnUserSettingsUpdated;

public:

	/** Calls the settings update delegate to allow subscribing systems to react to changes */
	UFUNCTION(BlueprintCallable, Category="User Settings")
	void UpdateUserSettings();

	/** Returns a copy of the user settings struct */
	UFUNCTION(BlueprintCallable, Category="User Settings")
	FTitanUserSettings GetUserSettings() const;

	/** Updates the internal user settings struct */
	UFUNCTION(BlueprintCallable, Category="User Settings")
	void SetUserSettings(const FTitanUserSettings& NewSettings);
};
