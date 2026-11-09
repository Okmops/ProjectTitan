// Copyright Epic Games, Inc. All Rights Reserved.


#include "Framework/TitanGameplayPlayerState.h"
#include "InventoryManagerComponent.h"
#include "UI/TitanHUD.h"
#include "SaveGame/TitanSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "Character/TitanGameplayCharacter.h"
#include "QuestComponent.h"
#include "TitanLogChannels.h"
#include "Engine/DataTable.h"
#include "Engine/SkeletalMesh.h"
#include "Animation/AnimInstance.h"
#include "../Abilities/TitanAttributeSet_Health.h"
#include "../Abilities/TitanAttributeSet_Currency.h"

ATitanGameplayPlayerState::ATitanGameplayPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// create the inventory component
	Inventory = CreateDefaultSubobject<UInventoryManagerComponent>(TEXT("Inventory"));
	check(Inventory);

	QuestManager = CreateDefaultSubobject<UQuestManagerComponent>(TEXT("Quest Manager"));
	check(QuestManager);
}

void ATitanGameplayPlayerState::BeginPlay()
{
	Super::BeginPlay();

	// load the save file
	LoadOrCreateSaveFile();

	// create the HUD
	CreateHUD();

	// do we already have a possessed character?
	if (ATitanGameplayCharacter* GameplayCharacter = Cast<ATitanGameplayCharacter>(SamplePawn))
	{	
		// initialize the character
		InitializeCharacter(GameplayCharacter);
	}

	// initialize the character skins
	InitializeCharacterSkins();

	// initialize the inventory
	InitializeInventory();

	// initialize the quest manager
	InitializeQuests();
}

void ATitanGameplayPlayerState::Destroyed()
{
	// destroy the HUD
	DestroyHUD();

	Super::Destroyed();
}

void ATitanGameplayPlayerState::OnPawnChanged(APlayerState* PlayerState, APawn* NewPawn, APawn* OldPawn)
{
	Super::OnPawnChanged(PlayerState, NewPawn, OldPawn);

	// are we possessing a gameplay character?
	ATitanGameplayCharacter* GameplayCharacter = Cast<ATitanGameplayCharacter>(NewPawn);

	if (HUD)
	{
		if (ATitanGameplayCharacter* OldGameplayCharacter = Cast<ATitanGameplayCharacter>(OldPawn))
		{
			// allow the HUD to reset for the outgoing character
			HUD->ResetCharacterDelegates(OldGameplayCharacter);
		}

		if (GameplayCharacter)
		{
			// allow the HUD to init for the incoming character
			HUD->InitializeForCharacter(GameplayCharacter);

			// initialize the character
			InitializeCharacter(GameplayCharacter);
		}
	}	
}

void ATitanGameplayPlayerState::InitializeCharacter(ATitanGameplayCharacter* GameplayCharacter)
{
	if (!GameplayCharacter)
	{
		return;
	}

	// initialize the pawn
	GameplayCharacter->OnPawnInitialized();
}

void ATitanGameplayPlayerState::InitializeInventory()
{
	// ignore if we have no save file
	if (!SaveFile)
	{
		return;
	}

	// update the item stacks on the inventory comp
	Inventory->UpdateItemStacks(SaveFile->InventoryTags);
}

void ATitanGameplayPlayerState::InitializeQuests()
{
	// ignore if we have no save file
	if (!SaveFile)
	{
		return;
	}

	// update the quest status and quantity tags
	QuestManager->SetQuestStatusData(SaveFile->QuestStatusTags);
	QuestManager->SetQuestQuantityData(SaveFile->QuestQuantities);

	// update completed and failed quests
	QuestManager->SetCompletedQuests(SaveFile->CompletedQuests);
	QuestManager->SetFailedQuests(SaveFile->FailedQuests);

	// start the active quests
	QuestManager->SetAndInitializeActiveQuests(SaveFile->ActiveQuests);
}

void ATitanGameplayPlayerState::CreateHUD()
{
	// avoid overwriting an already created HUD
	if (HUD)
	{
		return;
	}

	// only create the HUD for local players
	if(GetPlayerController() && GetPlayerController()->IsLocalController())
	{
		// create the HUD widget
		if ((HUD = CreateWidget<UTitanHUD>(GetPlayerController(), HUDClass)))
		{
			// init the HUD
			HUD->InitializeForPlayer(this);

			// allow the HUD to init for the character
			if (ATitanGameplayCharacter* GameplayCharacter = Cast<ATitanGameplayCharacter>(SamplePawn))
			{	
				HUD->InitializeForCharacter(GameplayCharacter);
			}

			HUD->AddToPlayerScreen(0);
		}
	}
}

void ATitanGameplayPlayerState::DestroyHUD()
{
	if (HUD)
	{
		// remove the HUD and release the pointer
		HUD->RemoveFromParent();
		HUD = nullptr;
	}
}

void ATitanGameplayPlayerState::SetHUDVisibility(bool bVisible)
{
	// set the HUD visibility
	if (HUD)
	{
		HUD->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

UTitanHUD* ATitanGameplayPlayerState::GetHUD() const
{
	return HUD;
}

UTitanSaveGame* ATitanGameplayPlayerState::LoadOrCreateSaveFile()
{
	// if we already have a valid Save File, return it
	if (SaveFile)
	{
		return SaveFile;
	}

	// attempt to load an existing save
	SaveFile = Cast<UTitanSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));

	// if we don't have a valid save file, create a new one
	if (!SaveFile)
	{
		SaveFile = Cast<UTitanSaveGame>(UGameplayStatics::CreateSaveGameObject(UTitanSaveGame::StaticClass()));
	}

	return SaveFile;
}

void ATitanGameplayPlayerState::SaveGame()
{
	// ensure the save file exists
	if (SaveFile)
	{
		// update the inventory data
		SaveFile->InventoryTags = Inventory->GetItemStacks();

		// update the quest data
		SaveFile->QuestStatusTags = QuestManager->GetQuestStatusData();
		SaveFile->QuestQuantities = QuestManager->GetQuestQuantityData();
		SaveFile->ActiveQuests = QuestManager->GetActiveQuestTags();
		SaveFile->CompletedQuests = QuestManager->GetCompletedQuestTags();
		SaveFile->FailedQuests = QuestManager->GetFailedQuestTags();

		// update the MaxHP value
		if(const UTitanAttributeSet_Health* HealthAttributes = Cast<UTitanAttributeSet_Health>(GetAbilitySystemComponent()->GetAttributeSet(UTitanAttributeSet_Health::StaticClass())))
		{
			SaveFile->MaxHealth = HealthAttributes->GetMaxHealth();
		}

		// update the Gold value
		if(const UTitanAttributeSet_Currency* CurrencyAttributes = Cast<UTitanAttributeSet_Currency>(GetAbilitySystemComponent()->GetAttributeSet(UTitanAttributeSet_Currency::StaticClass())))
		{
			SaveFile->Gold = CurrencyAttributes->GetGold();
		}

		UGameplayStatics::SaveGameToSlot(SaveFile, SaveSlotName, 0);
	}
}

void ATitanGameplayPlayerState::InitializeCharacterSkins()
{
	// ensure the data table is valid
	if (!CharacterSkinsDataTable)
	{
		UE_LOG(LogTitanSkin, Error, TEXT("ATitanGameplayPlayerState: Character Skins Data Table is not set! Skins will not be available."));
		return;
	}

	// populate the skins array
	TArray<FTitanSkinDescription*> DTSkins;

	CharacterSkinsDataTable->GetAllRows<FTitanSkinDescription>(FString(), DTSkins);

	// find all default skins
	for (const FTitanSkinDescription* Current : DTSkins)
	{
		if (Current)
		{
			// add the current skin
			CharacterSkins.Add(*Current);

			// is this a default skin?
			if(Current->bIsDefault)
			{
				SetSkinAsDefault(*Current);
			}
		}
	}

	// update skin data in the save file
	UpdateSkins();
}

void ATitanGameplayPlayerState::GetAllSkinsForBodyPart(FGameplayTag BodyPart, TArray<FTitanSkinDescription>& OutDescriptions)
{
	// ensure the output array is empty
	OutDescriptions.Empty();

	// ignore if the queried tag is not a body part
	if (!IsSkinBodyPartTag(BodyPart))
	{
		return;
	}

	// iterate through the skins array
	for (const FTitanSkinDescription& Current : CharacterSkins)
	{
		// does this match the body part?
		if (Current.BodyPart == BodyPart)
		{
			// add it to the output
			OutDescriptions.Add(Current);
		}
	}
}

void ATitanGameplayPlayerState::GetAllSkinsInCollection(FGameplayTag Collection, TArray<FTitanSkinDescription>& OutDescriptions)
{
	// ensure the output array is empty
	OutDescriptions.Empty();

	// ignore if the queried tag is not a collection
	if (!IsSkinCollectionTag(Collection))
	{
		return;
	}

	// iterate through the skins array
	for (const FTitanSkinDescription& Current : CharacterSkins)
	{
		// does this match the collection?
		if (Current.Collection == Collection)
		{
			// add it to the output
			OutDescriptions.Add(Current);
		}
	}
}

void ATitanGameplayPlayerState::GetDefaultMeshForPart(FGameplayTag BodyPart, USkeletalMesh*& OutMesh, TSubclassOf<UAnimInstance>& OutAnimBlueprint)
{
	// find the mesh asset for the body part
	if (USkeletalMesh** FoundMesh = DefaultMeshes.Find(BodyPart))
	{
		OutMesh = *FoundMesh;
	}
	else
	{
		OutMesh = nullptr;
	}

	// find the anim instance for the body part
	if (TSubclassOf<UAnimInstance>* FoundAnimInstance = DefaultAnimInstances.Find(BodyPart))
	{
		OutAnimBlueprint = *FoundAnimInstance;
	}
	else
	{
		// if not found, return the copy anim instance
		OutAnimBlueprint = CopyAnimInstance;
	}
}

FTitanSkinDescription ATitanGameplayPlayerState::GetDefaultSkinForPart(FGameplayTag BodyPart)
{
	// find the skin description for the part
	if (FTitanSkinDescription* FoundSkin = DefaultSkins.Find(BodyPart))
	{
		return *FoundSkin;
	}

	// not found, return an empty description
	return FTitanSkinDescription();
}

void ATitanGameplayPlayerState::GetAllDefaultSkinDescriptions(TArray<FTitanSkinDescription>& OutDescriptions)
{
	// ensure the output array is empty
	OutDescriptions.Empty();

	// populate the output array
	DefaultSkins.GenerateValueArray(OutDescriptions);
}

void ATitanGameplayPlayerState::SetSkinAsDefault(const FTitanSkinDescription& Description)
{
	// add the default skin to the map
	DefaultSkins.Add(Description.BodyPart, Description);

	// load the AnimInstance
	TSubclassOf<UAnimInstance> LoadedAnimInstance = TSubclassOf<UAnimInstance>(Description.AnimationBlueprint.LoadSynchronous());

	if (IsValid(LoadedAnimInstance))
	{
		DefaultAnimInstances.Add(Description.BodyPart, LoadedAnimInstance);
	}

	// load the skeletal mesh
	USkeletalMesh* LoadedMesh = Description.Mesh.LoadSynchronous();

	if (LoadedMesh)
	{
		DefaultMeshes.Add(Description.BodyPart, LoadedMesh);
	}

	// call the delegate
	OnSkinChanged.Broadcast(Description.BodyPart, LoadedMesh, LoadedAnimInstance);
}

void ATitanGameplayPlayerState::LoadSkin(const FTitanSkinDescription& Description)
{
	// load the AnimInstance
	TSubclassOf<UAnimInstance> LoadedAnimInstance = TSubclassOf<UAnimInstance>(Description.AnimationBlueprint.LoadSynchronous());

	if (IsValid(LoadedAnimInstance))
	{
		DefaultAnimInstances.Add(Description.BodyPart, LoadedAnimInstance);
	}
	else
	{
		// use the copy anim instance by default
		LoadedAnimInstance = CopyAnimInstance;
	}

	// load the skeletal mesh
	USkeletalMesh* LoadedMesh = Description.Mesh.LoadSynchronous();

	if (LoadedMesh)
	{
		DefaultMeshes.Add(Description.BodyPart, LoadedMesh);
	}

	OnSkinLoaded.Broadcast(Description.BodyPart, LoadedMesh, LoadedAnimInstance);
}

void ATitanGameplayPlayerState::SetSavedSkinForBodyPart(FGameplayTag BodyPart, const FTitanSkinDescription& Skin)
{
	if (!SaveFile)
	{
		return;
	}

	// update the Save File
	SaveFile->Skins.Add(BodyPart, Skin);
}

void ATitanGameplayPlayerState::UpdateSkins()
{
	// ensure we have a savefile
	if (!SaveFile)
	{
		return;
	}

	// get the default skin for each body part
	for (auto& Current : SaveFile->Skins)
	{
		// update the default skin and call the update delegates
		SetSkinAsDefault(Current.Value);
	}
}

bool ATitanGameplayPlayerState::IsSkinCollectionTag(const FGameplayTag& Tag)
{
	return Tag.IsValid() && Tag.MatchesTag(TAG_Titan_Skin_Collection);
}

bool ATitanGameplayPlayerState::IsSkinBodyPartTag(const FGameplayTag& Tag)
{
	return Tag.IsValid() && Tag.MatchesTag(TAG_Titan_Skin_BodyPart);
}

void ATitanGameplayPlayerState::UpdateUserSettings()
{
	// ensure we have a savefile
	if (!SaveFile)
	{
		return;
	}

	OnUserSettingsUpdated.Broadcast(SaveFile->UserSettings);
}

FTitanUserSettings ATitanGameplayPlayerState::GetUserSettings() const
{
	// ensure we have a savefile
	if (!SaveFile)
	{
		return FTitanUserSettings();
	}

	return SaveFile->UserSettings;
}

void ATitanGameplayPlayerState::SetUserSettings(const FTitanUserSettings& NewSettings)
{
	// ensure we have a savefile
	if (!SaveFile)
	{
		return;
	}

	// copy the settings
	SaveFile->UserSettings = NewSettings;
}
