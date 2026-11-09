// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "Engine/DataTable.h"
#include "TitanSkinDescription.generated.h"

class USkeletalMesh;
class UAnimInstance;

/** Root tag for character skin body parts */
TITAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Titan_Skin_BodyPart);

/** Root tag for character skin collections */
TITAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Titan_Skin_Collection);

USTRUCT(BlueprintType)
struct TITAN_API FTitanSkinDescription : public FTableRowBase
{
	GENERATED_BODY()

	/** Gameplay Tag identifying the body part this skin is for */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, SaveGame, Category="Skin", meta = (Categories = "Titan.Skin.BodyPart"))
	FGameplayTag BodyPart;

	/** Skeletal Mesh to use for this skin */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, SaveGame, Category="Skin")
	TSoftObjectPtr<USkeletalMesh> Mesh;

	/** Animation Blueprint to use for this skin */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, SaveGame, Category="Skin")
	TSoftClassPtr<UAnimInstance> AnimationBlueprint;

	/** Game-visible name for this skin */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, SaveGame, Category="Skin")
	FText Name;

	/** Gameplay Tag identifying all skins within the same collection */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, SaveGame, Category="Skin", meta = (Categories = "Titan.Skin.Collection"))
	FGameplayTag Collection;

	/** If true, this skin will be selected by default by the game */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, SaveGame, Category="Skin")
	bool bIsDefault = false;
};