// Copyright Epic Games, Inc. All Rights Reserved.


#include "UI/TitanHUD.h"
#include "Framework/TitanGameplayPlayerState.h"
#include "GAS/SampleAbilitySystemComponent.h"
#include "Abilities/TitanAttributeSet_Health.h"
#include "Abilities/TitanAttributeSet_Currency.h"
#include "Materials/MaterialInterface.h"
#include "Character/TitanGameplayCharacter.h"
#include "TitanMoverComponent.h"
#include "PlayerInteractionComponent.h"
#include "InteractionInterface.h"
#include "InventoryManagerComponent.h"
#include "InventoryFunctionLibrary.h"
#include "ItemDefinition.h"
#include "Fragments/ItemFragments_Pickup.h"
#include "Fragments/ItemFragments_UI.h"
#include "QuestComponent.h"

#define LOCTEXT_NAMESPACE "TitanHUD"

void UTitanHUD::InitializeForPlayer(ATitanGameplayPlayerState* OwningPlayerState)
{
	PlayerState = OwningPlayerState;

	if (PlayerState)
	{
		// get the ASC
		if (USampleAbilitySystemComponent* ASC = Cast<USampleAbilitySystemComponent>(PlayerState->GetAbilitySystemComponent()))
		{
			if(const UTitanAttributeSet_Health* HealthAttributes = Cast<UTitanAttributeSet_Health>(ASC->GetAttributeSet(UTitanAttributeSet_Health::StaticClass())))
			{
				// subscribe to health changes
				HealthAttributes->OnHealthChanged.AddDynamic(this, &UTitanHUD::OnHealthChanged);
				HealthAttributes->OnMaxHealthChanged.AddDynamic(this, &UTitanHUD::OnMaxHealthChanged);

				// init the health values
				Health = FMath::Max(HealthAttributes->GetHealth(), 0.0f);
				MaxHealth = FMath::Max(HealthAttributes->GetMaxHealth(), 1.0f);

				BP_SetMaxHealth(MaxHealth);

				OldHealthPct = Health / MaxHealth;

				BP_SetHealth(OldHealthPct);
			}

			if(const UTitanAttributeSet_Currency* CurrencyAttributes = Cast<UTitanAttributeSet_Currency>(ASC->GetAttributeSet(UTitanAttributeSet_Currency::StaticClass())))
			{
				// subscribe to currency changes
				CurrencyAttributes->OnGoldChanged.AddDynamic(this, &UTitanHUD::OnGoldChanged);

				// init the currency values
				OldGold = CurrencyAttributes->GetGold();

				BP_SetGold(OldGold, 0.0f);
			}
		}

		// get the inventory comp
		if (UInventoryManagerComponent* Inventory = PlayerState->GetInventory())
		{
			// subscribe to item changes
			Inventory->OnItemCountChanged.AddDynamic(this, &UTitanHUD::OnItemCountChanged);
		}

		// get the quest manager
		if (UQuestManagerComponent* QuestManager = PlayerState->GetQuestManager())
		{
			// subscribe to quest progression changes
			QuestManager->OnQuestStarted.AddDynamic(this, &UTitanHUD::OnQuestStarted);
			QuestManager->OnQuestCompleted.AddDynamic(this, &UTitanHUD::OnQuestCompleted);
			QuestManager->OnQuestUpdated.AddDynamic(this, &UTitanHUD::OnQuestUpdated);
		}
	}
}

void UTitanHUD::InitializeForCharacter(ATitanGameplayCharacter* OwningCharacter)
{
	if (IsValid(OwningCharacter))
	{
		// bind to the character delegates
		OwningCharacter->OnStaminaChanged.AddDynamic(this, &UTitanHUD::OnStaminaChanged);
		
		if (UTitanMoverComponent* MoverComp = OwningCharacter->GetMoverComponent())
		{
			MoverComp->OnMovementModeChanged.AddDynamic(this, &UTitanHUD::OnMovementModeChanged);
		}

		if (UPlayerInteractionComponent* InteractionBox = OwningCharacter->GetInteractionBox())
		{
			InteractionBox->OnTargetChanged.AddDynamic(this, &UTitanHUD::OnTargetChanged);
			InteractionBox->OnTargetLost.AddDynamic(this, &UTitanHUD::OnTargetLost);
			InteractionBox->OnInteractionStarted.AddDynamic(this, &UTitanHUD::OnInteractionStarted);
		}

		// pass to BP
		BP_InitializeForCharacter(OwningCharacter);
	}
	
}

void UTitanHUD::ResetCharacterDelegates(ATitanGameplayCharacter* OwningCharacter)
{
	if (IsValid(OwningCharacter))
	{
		// reset the character delegates
		OwningCharacter->OnStaminaChanged.RemoveAll(this);
		
		if (UTitanMoverComponent* MoverComp = OwningCharacter->GetMoverComponent())
		{
			MoverComp->OnMovementModeChanged.RemoveAll(this);
		}

		if (UPlayerInteractionComponent* InteractionBox = OwningCharacter->GetInteractionBox())
		{
			InteractionBox->OnTargetChanged.RemoveAll(this);
			InteractionBox->OnTargetLost.RemoveAll(this);
			InteractionBox->OnInteractionStarted.RemoveAll(this);
		}

		// pass to BP
		BP_InitializeForCharacter(OwningCharacter);
	}
}

void UTitanHUD::OnMovementModeChanged(const FName& PreviousMode, const FName& NewMode)
{
	// pass to BP
	BP_SetMovementMode(NewMode);
}

void UTitanHUD::OnStaminaChanged(float NewValue, bool bExhausted)
{
	// ignore if there's no significant change in value
	if (FMath::IsNearlyEqual(NewValue, OldStamina))
	{
		return;
	}

	// save the new stamina value
	OldStamina = NewValue;

	// pass the new value to BP
	BP_SetStamina(NewValue, bExhausted);
}

void UTitanHUD::OnHealthChanged(AActor* EffectInstigator, AActor* EffectCauser, const FGameplayEffectSpec& EffectSpec, float EffectMagnitude, float OldValue, float NewValue)
{
	// save the value
	Health = FMath::Max(NewValue, 0.0f);

	// recalculate the health bar
	UpdateHealth();
}

void UTitanHUD::OnMaxHealthChanged(float OldValue, float NewValue)
{
	// set the MaxHealth value but don't go below 1
	MaxHealth = FMath::Max(NewValue, 1.0f);

	// allow widgets to update the health bar size
	BP_SetMaxHealth(MaxHealth);

	// recalculate the health bar
	UpdateHealth();
}

void UTitanHUD::OnGoldChanged(AActor* EffectInstigator, AActor* EffectCauser, const FGameplayEffectSpec& EffectSpec, float EffectMagnitude, float OldValue, float NewValue)
{
	if (FMath::IsNearlyEqual(NewValue, OldGold))
	{
		return;
	}

	// clamp to 0
	const float ClampedGold = FMath::Max(NewValue, 0.0f);

	// calculate the change in gold value
	const float GoldDelta = ClampedGold - OldValue;

	// save the new gold value
	OldGold = ClampedGold;

	// pass the new value to BP
	BP_SetGold(OldGold, GoldDelta);
}

void UTitanHUD::OnItemCountChanged(const FGameplayTag& ItemTag, int32 CountDelta, int32 TotalCount)
{
	// get the item definition
	if (UItemDefinition* ItemDef = UInventoryFunctionLibrary::GetItemDefinition(this, ItemTag))
	{
		// get the pickup fragment
		UItemFragment_PickupData* Pickup = Cast<UItemFragment_PickupData>(ItemDef->FindFragmentByClass(UItemFragment_PickupData::StaticClass(), FGameplayTag::EmptyTag));

		if (Pickup)
		{
			// try to get the icon from the icons fragment
			TSoftObjectPtr<UMaterialInterface> ItemIcon = nullptr;

			if (UItemFragment_Icons* Icons = Cast<UItemFragment_Icons>(ItemDef->FindFragmentByClass(UItemFragment_Icons::StaticClass(), FGameplayTag::EmptyTag)))
			{
				ItemIcon = Icons->SmallIcon;
			}

			// choose the action text depending on whether we're picking up or losing items
			const FText& ActionText = CountDelta > 0 ? ItemsFoundText : ItemsLostText;

			// pick the singular or plural text for the item
			FText ItemText;
			const int32 AbsoluteDelta = FMath::Abs(CountDelta);

			if (AbsoluteDelta > 1)
			{
				ItemText = FText::Format(Pickup->PluralPickupText, AbsoluteDelta);
			}
			else
			{
				ItemText =  Pickup->SingularPickupText;
			}
			
			// build the notification string
			FText NotifyText = FText::Format(ActionText, ItemText);

			BP_AddHUDNotification(NotifyText, ItemIcon);
		}
	}
}

void UTitanHUD::OnQuestStarted(const FGameplayTag& QuestTag)
{
	// pass to BP to display the notification
	BP_AddQuestStatus(QuestTag, false, false);
}

void UTitanHUD::OnQuestCompleted(const FGameplayTag& QuestTag, bool bSucceeded)
{
	// pass to BP to display the notification
	BP_AddQuestStatus(QuestTag, true, !bSucceeded);
}

void UTitanHUD::OnQuestUpdated(const FGameplayTag& QuestTag, const FGameplayTag& QuestStepTag, const FText& NotifyText, bool bCompleted)
{
	// pass to BP to display the notification
	BP_AddQuestStepUpdate(NotifyText, bCompleted);
}

void UTitanHUD::UpdateHealth()
{
	const float NewRatio = Health / MaxHealth;

	if (FMath::IsNearlyEqual(NewRatio, OldHealthPct))
	{
		return;
	}

	// save the new health value
	OldHealthPct = NewRatio;

	// pass the new value to BP
	BP_SetHealth(NewRatio);
}

void UTitanHUD::OnTargetChanged(TScriptInterface<IPlayerInteractionInterface> NewTarget)
{
	if (NewTarget)
	{
		// pass to BP
		BP_ShowInteractionCue(NewTarget->GetInteractionVerbText());
	}
}

void UTitanHUD::OnTargetLost()
{
	// pass to BP
	BP_HideInteractionCue();
}

void UTitanHUD::OnInteractionStarted(AActor* InteractedActor)
{
	// pass to BP
	BP_HideInteractionCue();
}

void UTitanHUD::SetActionBarVisibility(bool bVisible)
{
	// pass to BP
	BP_SetActionBarVisibility(bVisible);
}

#undef LOCTEXT_NAMESPACE