// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "InputCoreTypes.h"
#include "SimulatedInputWidget.generated.h"

class UEnhancedInputLocalPlayerSubsystem;
class UInputAction;
class UCommonHardwareVisibilityBorder;
class UEnhancedPlayerInput;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInputWidgetActivatedEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInputWidgetDeactivatedEvent);

/**
 *  USimulatedInputWidget
 *  Enables input injection into the Enhanced Input Subsystem
 *  More than one Input Action can be injected at the same time, to provide parity with 1-to-N mappings
 */
UCLASS(abstract, meta = (DisplayName = "Simulated Input Widget"))
class INPUTWIDGETS_API USimulatedInputWidget : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:

	/** Constructor */
	USimulatedInputWidget(const FObjectInitializer& ObjectInitializer);

	//~ Begin UWidget
#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif
	//~ End UWidget interface

	//~ Begin UUserWidget
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
	//~ End UUserWidget interface

	/** Get the enhanced input subsystem based on the owning local player of this widget. Will return null if there is no owning player */
	UFUNCTION(BlueprintCallable)
	UEnhancedInputLocalPlayerSubsystem* GetEnhancedInputSubsystem() const;

	/** Get the current player input from the current input subsystem */
	UEnhancedPlayerInput* GetPlayerInput() const;

	/** Returns the current key that will be used to input any values. */
	UFUNCTION(BlueprintCallable)
	FKey GetSimulatedKey() const { return KeyToSimulate; }

	/**
	 * Injects the given vector as an input to the current simulated key.
	 * This calls "InputKey" on the current player.
	 */
	UFUNCTION(BlueprintCallable)
	void InputKeyValue(const FVector& Value);

	/**
	 * Injects the given vector as an input to the current simulated key.
	 * This calls "InputKey" on the current player.
	 */
	UFUNCTION(BlueprintCallable)
	void InputKeyValue2D(const FVector2D& Value);

	/** Flushes the player inputs */
	UFUNCTION(BlueprintCallable)
	virtual void FlushSimulatedInput();

	/** Resets the control's state to not injecting input */
	UFUNCTION(BlueprintCallable)
	virtual void ResetControl();

public:

	/** Triggered when the touch input is activated */
	UPROPERTY(BlueprintAssignable)
	FOnInputWidgetActivatedEvent OnInputActivated;

	/** Triggered when the touch input is deactivated */
	UPROPERTY(BlueprintAssignable)
	FOnInputWidgetDeactivatedEvent OnInputDeactivated;

protected:

	/** Set the KeyToSimulate based on a query from enhanced input about what keys are mapped to the associated action */
	void QueryKeyToSimulate();

	/** Called whenever control mappings change, so we have a chance to adapt our own keys */
	UFUNCTION()
	void OnControlMappingsRebuilt();

	/** The common visibility border will allow you to specify UI for only specific platforms if desired */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UCommonHardwareVisibilityBorder> CommonVisibilityBorder = nullptr;

	/** The associated input actions that we should simulate the input for */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<const UInputAction*> AssociatedActions;

	/** The Key to simulate input for in the case where none are currently bound to the associated action */
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FKey FallbackBindingKey = EKeys::Gamepad_Right2D;

	/** The key that should be input via InputKey on the player input */
	FKey KeyToSimulate;

};
