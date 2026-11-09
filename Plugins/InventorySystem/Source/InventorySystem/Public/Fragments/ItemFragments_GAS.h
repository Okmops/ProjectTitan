// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ItemDefinition.h"
#include "ItemFragments_GAS.generated.h"

class UGameplayAbility;
class UGameplayEffect;

///////////////////////////////////////////////////////

/**
 *  UItemFragment_ActivateAbility
 *  Attempts to grant and activates a Gameplay Ability on the item's owning actor when the item is activated.
 */
UCLASS()
class INVENTORYSYSTEM_API UItemFragment_ActivateAbility : public UItemFragment
{
	GENERATED_BODY()
	
public:

	/** Ability to grant and activate */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Activate Ability")
	TSoftClassPtr<UGameplayAbility> Ability;

	/** Level to grant the ability at */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Activate Ability")
	int32 Level = 1;

public:

	/** Grants and activates the Gameplay Ability when this item is activated. */
	virtual bool ActivateItem(const UItemDefinition* OwningItem, AActor* OwningActor, AActor* TargetActor) const override;

};

///////////////////////////////////////////////////////

/**
 *  UItemFragment_GameplayEvent
 *  Sends a Gameplay Event to the item's owning actor when the item is activated.
 *  Passes the following as part of the event payload:
 *  - Item Description: OptionalObject
 *  - TargetActor: TargetActor (may be nullptr)
 */
UCLASS()
class INVENTORYSYSTEM_API UItemFragment_GameplayEvent : public UItemFragment
{
	GENERATED_BODY()
	
public:

	/** Gameplay Event to send on activation */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay Event")
	FGameplayTag EventTag;

public:

	/** Sends the Gameplay Event when this item is activated. */
	virtual bool ActivateItem(const UItemDefinition* OwningItem, AActor* OwningActor, AActor* TargetActor) const override;

};

///////////////////////////////////////////////////////

/**
 *  Encapsulates data for a Gameplay Effect's Set by Caller Magnitude.
 */
USTRUCT(BlueprintType)
struct INVENTORYSYSTEM_API FItemSetByCallerMagnitude
{
	GENERATED_BODY()

	/** Tag to identify the Set by Caller Magnitude */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Magnitude")
	FGameplayTag Tag;

	/** Magnitude value */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Magnitude")
	float Magnitude = 0.0f;
};

/**
 *  Encapsulates data for a Gameplay Effect granted by an item.
 */
USTRUCT(BlueprintType)
struct INVENTORYSYSTEM_API FItemGameplayEffect
{
	GENERATED_BODY()

	/** Soft pointer to the Gameplay Effect class. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay Effect")
	TSoftClassPtr<UGameplayEffect> GameplayEffect;

	/** Level associated with the Gameplay Effect */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay Effect", meta = (ClampMin = 0))
	float Level = 1.0f;
};

/**
 *  UItemFragment_GameplayEffect
 *  Holds information about Gameplay Effects associated with the item.
 */
UCLASS(BlueprintType)
class INVENTORYSYSTEM_API UItemFragment_GameplayEffect : public UItemFragment
{
	GENERATED_BODY()
	
public:

	/** List of Gameplay Effects to grant. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay Effect")
	TArray<FItemGameplayEffect> GrantedGameplayEffects;

	/** List of set by caller magnitudes associated with the Gameplay Effect */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay Effect")
	TArray<FItemSetByCallerMagnitude> SetByCallerMagnitudes;
};

///////////////////////////////////////////////////////

/**
 *  UItemFragment_GameplayCue
 *  Triggers a Gameplay Cue on activation.
 *  Passes the item's owning actor as the cue's instigator.
 */
UCLASS()
class INVENTORYSYSTEM_API UItemFragment_GameplayCue : public UItemFragment
{
	GENERATED_BODY()
	
public:

	/** Gameplay Cue tag to trigger. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay Cue", meta = (Categories = "GameplayCue"))
	FGameplayTag GameplayCueTag;

public:

	/** Triggers the Gameplay Cue when this item is activated. */
	virtual bool ActivateItem(const UItemDefinition* OwningItem, AActor* OwningActor, AActor* TargetActor) const override;

};