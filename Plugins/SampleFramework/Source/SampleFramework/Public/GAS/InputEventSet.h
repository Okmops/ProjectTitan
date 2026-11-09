// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "InputEventSet.generated.h"

class UInputAction;
class UEnhancedInputComponent;
class USampleAbilitySystemComponent;

/**
 *  FInputEventSet_InputEvent
 *  An individual input event struct to translate an input action to a Gameplay Event
 */
USTRUCT(BlueprintType)
struct SAMPLEFRAMEWORK_API FInputEventSet_InputEvent
{
	GENERATED_BODY()

public:

	/** Input Action that triggers the event */
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> InputAction = nullptr;

	/** Tag of the event triggered in the ASC when the action is triggered */
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputEventTag;
};

/**
 *  UInputEventSet
 *  Non-mutable data asset used to bind Gameplay Event triggers to Input Actions
 */
UCLASS(BlueprintType, Const)
class SAMPLEFRAMEWORK_API UInputEventSet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	/** Constructor */
	UInputEventSet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Grants the event set to the given ASC */
	void GiveToAbilitySystem(USampleAbilitySystemComponent* AbilitySystem, UEnhancedInputComponent* InputComponent) const;

protected:

	/** Input Events to bind when this input event set is granted. */
	UPROPERTY(EditDefaultsOnly, Category="Input Events", meta=(TitleProperty=InputAction))
	TArray<FInputEventSet_InputEvent> GrantedInputEvents;
};
