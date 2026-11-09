// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "ItemDefinition.generated.h"

/**
 *  UItemFragment
 *  Represents a data fragment of an item definition.
 *  Subclass these to implement item traits or behaviors.
 */
UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, Abstract)
class INVENTORYSYSTEM_API UItemFragment : public UObject
{
	GENERATED_BODY()

public:

	/** Optional fragment tag. Can be used to identify a specific fragment when multiples of the same type are present. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fragment Tag")
	FGameplayTag FragmentTag;

	/** Called when the item owning this item is activated. */
	virtual bool ActivateItem(const UItemDefinition* OwningItem, AActor* OwningActor, AActor* TargetActor) const;
};

/**
 *  UItemDefinition
 *  Represents an inventory item definition.
 *  Item data and behaviors is stored as a collection of UItemFragments.
 */
UCLASS(Blueprintable)
class INVENTORYSYSTEM_API UItemDefinition : public UDataAsset
{
	GENERATED_BODY()
	
public:

	/** Constructor */
	UItemDefinition(const FObjectInitializer& ObjectInitializer);

protected:

	/** Human-readable development name for this item. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Item")
	FText DevelopmentName;

	/** Gameplay Tag that uniquely identifies this item. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Item", meta = (Categories = "Inventory.Item"))
	FGameplayTag ItemTag;

	/** List of fragments that specify this item's behavior. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, Category = "Fragments")
	TArray<TObjectPtr<UItemFragment>> Fragments;

public:

	/** Returns the item tag associated with this item */
	const FGameplayTag& GetItemTag() const;

	/** Attempts to find a specific type of fragment in this item. You can specify an optional Fragment Tag if you expect multiple fragments of the same type to be present. */
	UFUNCTION(BlueprintPure, Category="Item", meta = (DeterminesOutputType = "FragmentClass"))
	UItemFragment* FindFragmentByClass(TSubclassOf<UItemFragment> FragmentClass, FGameplayTag FragmentTag) const;

	/** Attempts to find all fragments of a specific type in this item. */
	UFUNCTION(BlueprintPure, Category="Item", meta = (DeterminesOutputType = "FragmentClass"))
	TArray<UItemFragment*> FindAllFragmentsByClass(TSubclassOf<UItemFragment> FragmentClass /*, TArray<UItemFragment*>& OutFragments*/) const;

	/** Attempts to activate this item. The activation behaviors are specified by the actual fragments. Fragments are activated in array order. */
	UFUNCTION(BlueprintCallable, Category="Item")
	bool ActivateItem(AActor* OwningActor, AActor* TargetActor);

#if WITH_EDITOR

public:

	/** Validates the asset prior to saving */
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;

#endif // WITH_EDITOR

};