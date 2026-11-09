// Copyright Epic Games, Inc. All Rights Reserved.


#include "ItemDefinition.h"
#include "InventorySystemLogging.h"
#include "Misc/DataValidation.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/AssetData.h"
#include "GameFramework/Actor.h"

#define LOCTEXT_NAMESPACE "ItemDefinition"

bool UItemFragment::ActivateItem(const UItemDefinition* OwningItem, AActor* ItemOwner, AActor* Target) const
{
	// succeed by default
	return true;
}

UItemDefinition::UItemDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

const FGameplayTag& UItemDefinition::GetItemTag() const
{
	return ItemTag;
}

UItemFragment* UItemDefinition::FindFragmentByClass(TSubclassOf<UItemFragment> FragmentClass, FGameplayTag FragmentTag) const
{
	// ensure the fragment class is valid
	if (FragmentClass != nullptr)
	{
		// look for the fragment type in the fragments array
		for (UItemFragment* Current : Fragments)
		{
			if (Current && Current->IsA(FragmentClass))
			{
				// are we trying to match a specific fragment tag?
				if (FragmentTag.IsValid())
				{
					// does the fragment tag match?
					if (Current->FragmentTag == FragmentTag)
					{
						return Current;
					}
				}
				else
				{
					// not trying to match a tag, so just return the first hit
					return Current;
				}
			}
		}
	}

	// not found
	return nullptr;
}

TArray<UItemFragment*> UItemDefinition::FindAllFragmentsByClass(TSubclassOf<UItemFragment> FragmentClass/*, TArray<UItemFragment*>& OutFragments*/) const
{
	TArray<UItemFragment*> OutFragments;

	// ensure the return array is empty
	OutFragments.Empty();

	// ensure the fragment class is valid
	if (FragmentClass != nullptr)
	{
		// look for the fragment type in the fragments array
		for (UItemFragment* Current : Fragments)
		{
			if (Current && Current->IsA(FragmentClass))
			{
				// add the fragment to the return array
				OutFragments.Add(Current);
			}
		}
	}

	return OutFragments;
}

bool UItemDefinition::ActivateItem(AActor* OwningActor, AActor* TargetActor)
{
	// ensure the owning actor is valid so fragments are guaranteed to be working with a valid item and owner
	if (!IsValid(OwningActor))
	{
		return false;
	}

	bool bSucceeded = true;

	// activate each fragment in array order
	for (const UItemFragment* Current : Fragments)
	{
		if (Current)
		{
			bool bFragmentSuccess = Current->ActivateItem(this, OwningActor, TargetActor);

			if (!bFragmentSuccess)
			{
				UE_LOG(LogInventory, Error, TEXT("UItemDefinition: Failed to activate fragment [%s]"), *Current->GetName());
			}

			bSucceeded &= bFragmentSuccess;
		}
	}

	// return activation success status
	return bSucceeded;
}

#if WITH_EDITOR

EDataValidationResult UItemDefinition::IsDataValid(class FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	// check the item tag
	if (ItemTag.IsValid())
	{
		// get the asset registry
		if (FAssetRegistryModule* AssetRegistryModule = FModuleManager::GetModulePtr<FAssetRegistryModule>(TEXT("AssetRegistry")))
		{
			// get all of the item assets
			TArray<FAssetData> ItemAssets;
			FARFilter AssetFilter;
			AssetFilter.ClassPaths.Add(UItemDefinition::StaticClass()->GetClassPathName());

			AssetRegistryModule->Get().GetAssets(AssetFilter, ItemAssets);

			// go through all existing assets
			for (const FAssetData& CurrentAsset : ItemAssets)
			{
				// @TODO: this is slow and can potentially load a lot of assets into memory.
				// Look for a more efficient way, maybe using Asset Tags.
				if (UItemDefinition* CurrentDef = Cast<UItemDefinition>(CurrentAsset.GetAsset()))
				{
					// skip this item
					if (CurrentDef == this)
					{
						continue;
					}

					// check if there's a duplicate ItemTag being used.
					if (CurrentDef->GetItemTag() == ItemTag)
					{
						Context.AddError(FText::Format(LOCTEXT("ItemDefValidationErrorInvalidTag", "ItemTag is in use by ItemDefinition [{0}]."), FText::FromString(CurrentDef->GetName())));
						Result = EDataValidationResult::Invalid;
					}
				}
			}
		}
	}
	else
	{
		// invalid item tag
		Context.AddError(LOCTEXT("ItemDefValidationErrorInvalidTag", "ItemTag is invalid."));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

#endif // WITH_EDITOR

#undef LOCTEXT_NAMESPACE