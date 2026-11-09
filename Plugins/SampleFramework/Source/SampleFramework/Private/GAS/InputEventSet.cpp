// Copyright Epic Games, Inc. All Rights Reserved.

#include "GAS/InputEventSet.h"
#include "GAS/SampleAbilitySystemComponent.h"
#include "SampleLogging.h"
#include "EnhancedInputComponent.h"

UInputEventSet::UInputEventSet(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

void UInputEventSet::GiveToAbilitySystem(USampleAbilitySystemComponent* AbilitySystem, UEnhancedInputComponent* InputComponent) const
{
	check(AbilitySystem);
	check(InputComponent);

	// iterate through all granted input events
	for (int32 idx = 0; idx < GrantedInputEvents.Num(); ++idx)
	{
		// get the input event
		const FInputEventSet_InputEvent& InputEvent = GrantedInputEvents[idx];

		// ensure the input action is valid
		if (!InputEvent.InputAction)
		{
			UE_LOG(LogSampleAbilitySystem, Warning, TEXT("GrantedInputEvents[%d] on Input Event Set [%s] is not valid. Skipping the binding."), idx, *GetNameSafe(this));
			continue;
		}

		// bind the input event
		AbilitySystem->BindInputEvent(InputEvent.InputAction, InputEvent.InputEventTag, InputComponent);
	}
}