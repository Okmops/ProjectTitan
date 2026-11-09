// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ItemDefinition.h"
#include "ItemFragments_Animation.generated.h"

class UAnimMontage;

/**
 *  UItemFragment_AnimMontage
 *  Item data used to play an Animation Montage
 */
UCLASS()
class INVENTORYSYSTEM_API UItemFragment_AnimMontage : public UItemFragment
{
	GENERATED_BODY()
	
public:

	/** Pointer to the AnimMontage. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage Data")
	TSoftObjectPtr<UAnimMontage> Montage;

	/** Play Rate for this montage. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage Data", meta = (ClampMin = 0))
	float PlayRate = 1.0f;

	/** Start time for this montage, in seconds. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage Data", meta = (ClampMin = 0))
	float StartTime = 0.0f;

	/** Optional starting section for this montage. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage Data")
	FName StartSection;

};