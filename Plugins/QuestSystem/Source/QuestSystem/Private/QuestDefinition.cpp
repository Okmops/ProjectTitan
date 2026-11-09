// Copyright Epic Games, Inc. All Rights Reserved.


#include "QuestDefinition.h"
#include "Misc/DataValidation.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "StateTree.h"
#include "AssetRegistry/AssetData.h"

#define LOCTEXT_NAMESPACE "QuestDefinition"

UQuestDefinition::UQuestDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

const FGameplayTag& UQuestDefinition::GetQuestTag() const
{
	return QuestTag;
}

UStateTree* UQuestDefinition::GetStateTree() const
{
	return QuestStateTree;
}

bool UQuestDefinition::FindQuestStep(FGameplayTag QuestStepTag, FQuestStep& OutStep) const
{
	// look for a quest step with a matching tag
	for (const FQuestStep& Current : QuestSteps)
	{
		if (Current.QuestStepTag == QuestStepTag)
		{
			OutStep = Current;
			return true;
		}
	}

	// not found
	return false;
}

const TArray<FQuestStep>& UQuestDefinition::GetQuestSteps() const
{
	return QuestSteps;
}

#if WITH_EDITOR

EDataValidationResult UQuestDefinition::IsDataValid(class FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	// check the StateTree
	if (!QuestStateTree)
	{
		// invalid StateTree asset
		Context.AddError(LOCTEXT("QuestDefValidationErrorInvalidStateTree", "StateTree is invalid."));
		Result = EDataValidationResult::Invalid;
	}

	// check the Quest tag
	if (QuestTag.IsValid())
	{
		// get the asset registry
		if (FAssetRegistryModule* AssetRegistryModule = FModuleManager::GetModulePtr<FAssetRegistryModule>(TEXT("AssetRegistry")))
		{
			// get all of the quest assets
			TArray<FAssetData> QuestAssets;
			FARFilter AssetFilter;
			AssetFilter.ClassPaths.Add(UQuestDefinition::StaticClass()->GetClassPathName());

			AssetRegistryModule->Get().GetAssets(AssetFilter, QuestAssets);

			// go through all existing assets
			for (const FAssetData& CurrentAsset : QuestAssets)
			{
				// @TODO: this is slow and can potentially load a lot of assets into memory.
				// Look for a more efficient way, maybe using Asset Tags.
				if (UQuestDefinition* CurrentDef = Cast<UQuestDefinition>(CurrentAsset.GetAsset()))
				{
					// skip this Quest
					if (CurrentDef == this)
					{
						continue;
					}

					// check if there's a duplicate QuestTag being used.
					if (CurrentDef->GetQuestTag() == QuestTag)
					{
						Context.AddError(FText::Format(LOCTEXT("QuestDefValidationErrorInvalidTag", "QuestTag is in use by QuestDefinition [{0}]."), FText::FromString(CurrentDef->GetName())));
						Result = EDataValidationResult::Invalid;
					}
				}
			}
		}
	}
	else
	{
		// invalid Quest tag
		Context.AddError(LOCTEXT("QuestDefValidationErrorInvalidTag", "QuestTag is invalid."));
		Result = EDataValidationResult::Invalid;
	}

	// ensure the quest steps have no repeated tags
	for (int32 i = 0; i < QuestSteps.Num(); ++i)
	{
		for (int32 j = 0; j < QuestSteps.Num(); ++j)
		{
			// skip comparisons against self
			if (i == j)
			{
				continue;
			}

			if (QuestSteps[i].QuestStepTag == QuestSteps[j].QuestStepTag)
			{
				Context.AddError(FText::Format(LOCTEXT("QuestDefValidationErrorDuplicateStepTag", "Quest Step {0} has the same step tag as Quest Step {1}"), FText::AsNumber(i), FText::AsNumber(j)));
				Result = EDataValidationResult::Invalid;
			}
		}
	}

	return Result;
}

#endif // WITH_EDITOR

#undef LOCTEXT_NAMESPACE