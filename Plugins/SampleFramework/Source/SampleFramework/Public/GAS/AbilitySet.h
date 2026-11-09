// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayAbilitySpecHandle.h"
#include "ActiveGameplayEffectHandle.h"
#include "AbilitySet.generated.h"

class USampleGameplayAbility;
class USampleGameplayEffect;
class UAttributeSet;
class USampleAbilitySystemComponent;

/**
 * FAbilitySet_GameplayAbility
 * Data used by the ability set to grant a gameplay ability.
 */
USTRUCT(BlueprintType)
struct SAMPLEFRAMEWORK_API FAbilitySet_GameplayAbility
{
	GENERATED_BODY()

public:

	// Class of the gameplay ability to grant
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<USampleGameplayAbility> Ability = nullptr;

	// Level of the ability to grant
	UPROPERTY(EditDefaultsOnly)
	int32 AbilityLevel = 1;
};

/**
 * FAbilitySet_GameplayEffect
 * Data used by the ability set to grant a gameplay effect.
 */
USTRUCT(BlueprintType)
struct SAMPLEFRAMEWORK_API FAbilitySet_GameplayEffect
{
	GENERATED_BODY()

public:

	// Class of the gameplay effect to grant
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<USampleGameplayEffect> GameplayEffect = nullptr;

	// Level of the gameplay effect to grant
	UPROPERTY(EditDefaultsOnly)
	float EffectLevel = 1.0f;
};

/**
 * FAbilitySet_AttributeSet
 * Data used by the ability set to grant an attribute set.
 */
USTRUCT(BlueprintType)
struct SAMPLEFRAMEWORK_API FAbilitySet_AttributeSet
{
	GENERATED_BODY()

public:

	// Class of the Attribute Set to grant
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAttributeSet> AttributeSet = nullptr;
};

/**
 * FAbilitySet_GrantedHandles
 * Data used to store ASC handles granted by the ability set.
 */
USTRUCT(BlueprintType)
struct SAMPLEFRAMEWORK_API FAbilitySet_GrantedHandles
{
	GENERATED_BODY()

public:

	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle);
	void AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle);
	void AddAttributeSet(UAttributeSet* Set);

	void TakeFromAbilitySystem(USampleAbilitySystemComponent* ASC);

protected:

	// Handles to the granted abilities.
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;

	// Handles to the granted gameplay effects.
	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> GameplayEffectHandles;

	// Pointers to the granted attribute sets
	UPROPERTY()
	TArray<TObjectPtr<UAttributeSet>> GrantedAttributeSets;
};

/**
 *  UAbilitySet
 *  
 *  Non-mutable data asset used to grant gameplay abilities, effects and attributes to an ASC
 */
UCLASS(BlueprintType, Const)
class SAMPLEFRAMEWORK_API UAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:

	UAbilitySet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/*
	 *  Grants the ability set to the specified ability system component.
	 *  The returned handles can be used later to take away anything that was granted.
	 */
	void GiveToAbilitySystem(USampleAbilitySystemComponent* ASC, FAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject = nullptr) const;

protected:

	// Gameplay Abilities to grant when this ability set is granted.
	UPROPERTY(EditDefaultsOnly, Category="Gameplay Abilities", meta=(TitleProperty=Ability))
	TArray<FAbilitySet_GameplayAbility> GrantedGameplayAbilities;

	// Gameplay Effects to grant when this ability set is granted.
	UPROPERTY(EditDefaultsOnly, Category="Gameplay Abilities", meta=(TitleProperty=GameplayEffect))
	TArray<FAbilitySet_GameplayEffect> GrantedGameplayEffects;

	// Attribute Sets to grant when this ability set is granted.
	UPROPERTY(EditDefaultsOnly, Category="Gameplay Abilities", meta=(TitleProperty=AttributeSet))
	TArray<FAbilitySet_AttributeSet> GrantedAttributeSets;
};
