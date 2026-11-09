// Copyright Epic Games, Inc. All Rights Reserved.


#include "TouchToggleWidget.h"

struct FGeometry;
struct FPointerEvent;

FReply UTouchToggleWidget::NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	// toggle the simulate input flag
	bShouldSimulateInput = !bShouldSimulateInput;

	// should we activate/deactivate?
	if (bShouldSimulateInput)
	{
		// broadcast the activated delegate
		OnInputActivated.Broadcast();
	}
	else
	{

		// broadcast the deactivated delegate
		OnInputDeactivated.Broadcast();

		//FlushSimulatedInput();
	}

	return Super::NativeOnTouchStarted(InGeometry, InGestureEvent);
}

void UTouchToggleWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// if the simulate flag is raised, inject the input
	if (bShouldSimulateInput)
	{
		InputKeyValue(FVector::OneVector);
	}
}

void UTouchToggleWidget::ResetControl()
{
	// reset the flag
	bShouldSimulateInput = false;

	//FlushSimulatedInput();

	// broadcast the deactivated delegate
	OnInputDeactivated.Broadcast();
}
