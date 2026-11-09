// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SimulatedInputWidget.h"
#include "TouchRegionWidget.generated.h"

class UObject;
struct FFrame;
struct FGeometry;
struct FPointerEvent;

/**
 *  UTouchRegionWidget
 *  Defines an area of the screen that should trigger an input when the user touches it.
 *  The input is injected every Tick as long as the widget is touched, and is ideal for press-and-hold actions.
 */
UCLASS(meta = (DisplayName = "Touch Region Widget"))
class INPUTWIDGETS_API UTouchRegionWidget : public USimulatedInputWidget
{
	GENERATED_BODY()
	
public:
	
	//~ Begin UUserWidget
	virtual FReply NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
	virtual FReply NativeOnTouchMoved(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
	virtual FReply NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	//~ End UUserWidget interface

	/** Returns whether the widget should simulate the input */
	UFUNCTION(BlueprintCallable)
	bool ShouldSimulateInput() const { return bShouldSimulateInput; }

	/** Implements the reset functionality */
	virtual void ResetControl() override;

	virtual void FlushSimulatedInput() override;

protected:

	/** True while this widget is being touched */
	bool bShouldSimulateInput = false;
};
