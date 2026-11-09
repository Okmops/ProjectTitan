// Copyright Epic Games, Inc. All Rights Reserved.


#include "Framework/SamplePlayerController.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "GAS/SampleAbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"

ASamplePlayerController::ASamplePlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void ASamplePlayerController::BeginPlay()
{
	Super::BeginPlay();

	// only deal with input if we're the local player
	if (IsLocalPlayerController())
	{
		// get the input subsystem
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			// add the default input mapping contexts
			for (TObjectPtr<UInputMappingContext>& CurrentContext : DefaultMappingContexts)
			{
				if (CurrentContext)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}
}

void ASamplePlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	// get the ability system component
	if (IAbilitySystemInterface* AbilityInterface = Cast<IAbilitySystemInterface>(PlayerState))
	{
		if (USampleAbilitySystemComponent* SampleASC = Cast<USampleAbilitySystemComponent>(AbilityInterface->GetAbilitySystemComponent()))
		{
			SampleASC->ProcessAbilityInput(DeltaTime, bGamePaused);
		}
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
}

void ASamplePlayerController::SetMenuInputsEnabled(bool bEnabled)
{
	// only deal with input if we're the local player
	if (IsLocalPlayerController())
	{
		FModifyContextOptions ContextOptions;
		ContextOptions.bIgnoreAllPressedKeysUntilRelease = true;

		// get the input subsystem
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			// should we add the mappings?
			if (bEnabled && !bMenuMappingsActive)
			{
				// add the menu mapping contexts
				for (TObjectPtr<UInputMappingContext>& CurrentContext : MenuMappingContexts)
				{
					if (CurrentContext)
					{
						Subsystem->AddMappingContext(CurrentContext, 0, ContextOptions);
					}
				}
			
				// set the flag
				bMenuMappingsActive = true;
			}
			// should we remove the mappings?
			else if (!bEnabled && bMenuMappingsActive)
			{
				// remove the menu mapping contexts
				for (TObjectPtr<UInputMappingContext>& CurrentContext : MenuMappingContexts)
				{
					if (CurrentContext)
					{
						Subsystem->RemoveMappingContext(CurrentContext, ContextOptions);
					}
				}

				// set the flag
				bMenuMappingsActive = false;
			}
		}
	}
}