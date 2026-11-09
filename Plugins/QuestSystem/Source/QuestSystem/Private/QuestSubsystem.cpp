// Copyright Epic Games, Inc. All Rights Reserved.


#include "QuestSubsystem.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "QuestDefinition.h"
#include "QuestLogging.h"
#include "AssetRegistry/AssetData.h"

void UQuestSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// get the asset registry
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
    IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	// is it still loading assets?
	if (AssetRegistry.IsLoadingAssets())
	{
		AssetRegistry.OnFilesLoaded().AddUObject(this, &UQuestSubsystem::BuildQuestMap);
	}
	else
	{
		// asset load is done, so go ahead and build the Quest map immediately
		BuildQuestMap();
	}
}

void UQuestSubsystem::Deinitialize()
{
	// clear the Definitions map
	QuestDefinitions.Empty();

	// remove the asset callbacks
    if (FAssetRegistryModule* AssetRegistryModule = FModuleManager::GetModulePtr<FAssetRegistryModule>(TEXT("AssetRegistry")))
    {
		AssetRegistryModule->Get().OnFilesLoaded().RemoveAll(this);
    }

	Super::Deinitialize();
}

void UQuestSubsystem::BuildQuestMap()
{
	// get the asset registry
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
    IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	// get all Quest Definition asset data
    const FTopLevelAssetPath ClassPath(UQuestDefinition::StaticClass()->GetPathName());

	TArray<FAssetData> AssetDataList;
    AssetRegistry.GetAssetsByClass(ClassPath, AssetDataList, true);

	int32 LoggedErrors = 0;
	UE_LOG(LogQuest, Log, TEXT("UQuestSubsystem: Found [%d] Quest Definitions in Project"), AssetDataList.Num());

	// reset the quest map
	QuestDefinitions.Empty(AssetDataList.Num());

    // process each asset
    for (const FAssetData& AssetData : AssetDataList)
    {
		// load the asset
        if (UQuestDefinition* QuestDef = Cast<UQuestDefinition>(AssetData.GetAsset()))
        {
			const FGameplayTag& QuestTag = QuestDef->GetQuestTag();

			if (QuestTag.IsValid())
			{
				if(!QuestDefinitions.Contains(QuestTag))
				{
					QuestDefinitions.Add(QuestTag, QuestDef);
				}
				else
				{
					// duplicate Quest tags should be treated as an error
					UE_LOG(LogQuest, Error, TEXT("UQuestSubsystem: Found Quest Definition [%s] with duplicate QuestTag [%s]. This Quest Definition was skipped and will not be available."), *QuestDef->GetName(), *QuestTag.GetTagName().ToString());
					++LoggedErrors;
				}
			}
			else
			{
				// assets with invalid tags should be treated as an error
				UE_LOG(LogQuest, Error, TEXT("UQuestSubsystem: Found Quest Definition [%s] with invalid QuestTag. This Quest Definition was skipped and will not be available."), *QuestDef->GetName());
				++LoggedErrors;
			}
        }
    }

	if(LoggedErrors > 0)
	{
		UE_LOG(LogQuest, Warning, TEXT("UQuestSubsystem: Encountered [%d] errors while building the Quest Map."), LoggedErrors);
	}
}

UQuestDefinition* UQuestSubsystem::GetQuestDefinition(FGameplayTag QuestTag) const
{
	// find the Quest Definition and return it
	const TObjectPtr<UQuestDefinition>* QuestDefPtr = QuestDefinitions.Find(QuestTag);

	if(QuestDefPtr)
	{
		return *QuestDefPtr;
	}

	// Quest Definition not found
	return nullptr;
}
