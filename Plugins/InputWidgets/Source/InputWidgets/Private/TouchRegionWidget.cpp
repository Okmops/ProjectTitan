// Copyright Epic Games, Inc. All Rights Reserved.


#include "TouchRegionWidget.h"

struct FGeometry;
struct FPointerEvent;

FReply UTouchRegionWidget::NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	// raise the simulate input flag
	bShouldSimulateInput = true;

	// broadcast the activated delegate
	OnInputActivated.Broadcast();

	return Super::NativeOnTouchStarted(InGeometry, InGestureEvent);
}

FReply UTouchRegionWidget::NativeOnTouchMoved(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	// raise the simulate input flag
	bShouldSimulateInput = true;

	return Super::NativeOnTouchMoved(InGeometry, InGestureEvent);
}

FReply UTouchRegionWidget::NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	// lower the simulate input flag
	bShouldSimulateInput = false;

	// broadcast the deactivated delegate
	OnInputDeactivated.Broadcast();

	return Super::NativeOnTouchEnded(InGeometry, InGestureEvent);
}

void UTouchRegionWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// if the simulate flag is raised, inject the input
	if (bShouldSimulateInput)
	{
		InputKeyValue(FVector::OneVector);
	}
}

void UTouchRegionWidget::ResetControl()
{
	// reset the flag
	bShouldSimulateInput = false;

	// broadcast the deactivated delegate
	OnInputDeactivated.Broadcast();
}

void UTouchRegionWidget::FlushSimulatedInput()
{
	bShouldSimulateInput = false;

	Super::FlushSimulatedInput();
}
