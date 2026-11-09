// Copyright Epic Games, Inc. All Rights Reserved.


#include "InventorySubsystem.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "ItemDefinition.h"
#include "InventorySystemLogging.h"
#include "AssetRegistry/AssetData.h"

void UInventorySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// get the asset registry
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
    IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	// is it still loading assets?
	if (AssetRegistry.IsLoadingAssets())
	{
		AssetRegistry.OnFilesLoaded().AddUObject(this, &UInventorySubsystem::BuildItemMap);
	}
	else
	{
		// asset load is done, so go ahead and build the item map immediately
		BuildItemMap();
	}
}

void UInventorySubsystem::Deinitialize()
{
	// clear the Definitions map
	ItemDefinitions.Empty();

	// remove the asset callbacks
    if (FAssetRegistryModule* AssetRegistryModule = FModuleManager::GetModulePtr<FAssetRegistryModule>(TEXT("AssetRegistry")))
    {
		AssetRegistryModule->Get().OnFilesLoaded().RemoveAll(this);
    }

	Super::Deinitialize();
}

void UInventorySubsystem::BuildItemMap()
{
	// get the asset registry
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
    IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	// get all Item Definition asset data
    const FTopLevelAssetPath ClassPath(UItemDefinition::StaticClass()->GetPathName());

	TArray<FAssetData> AssetDataList;
    AssetRegistry.GetAssetsByClass(ClassPath, AssetDataList, true);

	int32 LoggedErrors = 0;
	UE_LOG(LogInventory, Log, TEXT("UInventorySubsystem: Found [%d] Item Definitions in Project"), AssetDataList.Num());

    // process each asset
    for (const FAssetData& AssetData : AssetDataList)
    {
		// load the asset
        if (UItemDefinition* ItemDef = Cast<UItemDefinition>(AssetData.GetAsset()))
        {
			const FGameplayTag& ItemTag = ItemDef->GetItemTag();

			if (ItemTag.IsValid())
			{
				if(!ItemDefinitions.Contains(ItemTag))
				{
					ItemDefinitions.Add(ItemTag, ItemDef);
				}
				else
				{
					// duplicate item tags should be treated as an error
					UE_LOG(LogInventory, Error, TEXT("UInventorySubsystem: Found Item Definition [%s] with duplicate ItemTag [%s]. This Item Definition was skipped and will not be available."), *ItemDef->GetName(), *ItemTag.GetTagName().ToString());
					++LoggedErrors;
				}
			}
			else
			{
				// assets with invalid tags should be treated as an error
				UE_LOG(LogInventory, Error, TEXT("UInventorySubsystem: Found Item Definition [%s] with invalid ItemTag. This Item Definition was skipped and will not be available."), *ItemDef->GetName());
				++LoggedErrors;
			}
        }
    }

	if(LoggedErrors > 0)
	{
		UE_LOG(LogInventory, Warning, TEXT("UInventorySubsystem: Encountered [%d] errors while building the Item Map."), LoggedErrors);
	}
}

UItemDefinition* UInventorySubsystem::GetItemDefinition(FGameplayTag ItemTag) const
{
	// find the item Definition and return it
	const TObjectPtr<UItemDefinition>* ItemDefPtr = ItemDefinitions.Find(ItemTag);

	if(ItemDefPtr)
	{
		return *ItemDefPtr;
	}

	// item Definition not found
	return nullptr;
}