// Copyright Epic Games, Inc. All Rights Reserved.


#include "SimulatedInputWidget.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputSubsystems.h"

#define LOCTEXT_NAMESPACE "SimulatedInputWidget"

USimulatedInputWidget::USimulatedInputWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetConsumePointerInput(true);
	SetIsFocusable(false);
}

#if WITH_EDITOR
const FText USimulatedInputWidget::GetPaletteCategory()
{
	return LOCTEXT("PaletteCategory", "Input");
}
#endif // WITH_EDITOR

void USimulatedInputWidget::NativeConstruct()
{
	// find the initial key, then subscribe to any changes to control mappings
	QueryKeyToSimulate();

	if (UEnhancedInputLocalPlayerSubsystem* System = GetEnhancedInputSubsystem())
	{
		System->ControlMappingsRebuiltDelegate.AddUniqueDynamic(this, &USimulatedInputWidget::OnControlMappingsRebuilt);
	}

	Super::NativeConstruct();
}

void USimulatedInputWidget::NativeDestruct()
{
	// unsubscribe from changes to control mappings
	if (UEnhancedInputLocalPlayerSubsystem* System = GetEnhancedInputSubsystem())
	{
		System->ControlMappingsRebuiltDelegate.RemoveAll(this);
	}

	Super::NativeDestruct();
}

FReply USimulatedInputWidget::NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	// ensure we flush inputs on touch end
	FlushSimulatedInput();

	return Super::NativeOnTouchEnded(InGeometry, InGestureEvent);
}

UEnhancedInputLocalPlayerSubsystem* USimulatedInputWidget::GetEnhancedInputSubsystem() const
{
	// get the owning player's enhanced input subsystem
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (ULocalPlayer* LP = GetOwningLocalPlayer())
		{
			return LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		}
	}

	// no subsystem found
	return nullptr;
}

UEnhancedPlayerInput* USimulatedInputWidget::GetPlayerInput() const
{
	// get the enhanced input subsystem
	if (UEnhancedInputLocalPlayerSubsystem* System = GetEnhancedInputSubsystem())
	{
		// return the player input
		return System->GetPlayerInput();
	}

	// no player input found
	return nullptr;
}

void USimulatedInputWidget::InputKeyValue(const FVector& Value)
{
	// Use the associated input actions if they exist
	if (AssociatedActions.Num() > 0)
	{
		if (UEnhancedInputLocalPlayerSubsystem* System = GetEnhancedInputSubsystem())
		{
			// We don't want to apply any modifiers or triggers to this action, but they are required for the function signature
			TArray<UInputModifier*> Modifiers;
			TArray<UInputTrigger*> Triggers;

			for (const UInputAction* CurrentAction : AssociatedActions)
			{
				System->InjectInputVectorForAction(CurrentAction, Value, Modifiers, Triggers);
			}
		}
	}
	// In case there is no associated input action, we can attempt to simulate input on the fallback key
	else if (UEnhancedPlayerInput* Input = GetPlayerInput())
	{
		if (KeyToSimulate.IsValid())
		{
			const FInputKeyEventArgs KeyArgs = KeyArgs.CreateSimulated(KeyToSimulate, EInputEvent::IE_Pressed, Value.Length(), 1);			

			Input->InputKey(KeyArgs);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("'%s' is attempting to simulate input but has no player input!"), *GetNameSafe(this));
	}
}

void USimulatedInputWidget::InputKeyValue2D(const FVector2D& Value)
{
	InputKeyValue(FVector(Value.X, Value.Y, 0.0));
}

void USimulatedInputWidget::FlushSimulatedInput()
{
	// flush pressed keys from the player input
	if (UEnhancedPlayerInput* Input = GetPlayerInput())
	{
		Input->FlushPressedKeys();
	}
}

void USimulatedInputWidget::ResetControl()
{
	// stub
}

void USimulatedInputWidget::QueryKeyToSimulate()
{
	// get the enhanced input subsystem
	if (UEnhancedInputLocalPlayerSubsystem* System = GetEnhancedInputSubsystem())
	{
		bool bActionFound = false;

		if (AssociatedActions.Num() > 0)
		{
			// get the keys for the associated action
			TArray<FKey> Keys = System->QueryKeysMappedToAction(AssociatedActions[0]);

			// choose the first bound key
			if (!Keys.IsEmpty() && Keys[0].IsValid())
			{
				KeyToSimulate = Keys[0];
				bActionFound = true;
			}
		}
		
		if(!bActionFound)
		{
			// if no key is found, use the fallback key binding
			KeyToSimulate = FallbackBindingKey;
		}
	}
}

void USimulatedInputWidget::OnControlMappingsRebuilt()
{
	QueryKeyToSimulate();
}

#undef LOCTEXT_NAMESPACE