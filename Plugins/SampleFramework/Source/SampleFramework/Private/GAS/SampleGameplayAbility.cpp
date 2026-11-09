// Copyright Epic Games, Inc. All Rights Reserved.


#include "GAS/SampleGameplayAbility.h"
#include "GAS/AbilityCost.h"
#include "SampleLogging.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemComponent.h"

USampleGameplayAbility::USampleGameplayAbility(FObjectInitializer const& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void USampleGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// call the BP initialization hook
	K2_InitAbilityForActivation();
	
	// handle the rest of the activation flow
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void USampleGameplayAbility::NativeOnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const
{
	// stub to allow derive abilities to handle activation failure
}

void USampleGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	// skip Super and only set the actorInfo
	// so that OnAvatarSet doesn't get called before the ability system is ready
	SetCurrentActorInfo(Spec.Handle, ActorInfo);

	// call the BP hook
	K2_OnAbilityAdded();
}

void USampleGameplayAbility::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	// call the BP hook
	K2_OnAbilityRemoved();

	// finish ability removal
	Super::OnRemoveAbility(ActorInfo, Spec);
}

bool USampleGameplayAbility::ValidateAbilityActorInfo(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& AbilitySpec)
{
	// Subclasses can override this to implement additional validations.
	
	// ensure the ActorInfo is valid
	if (!ActorInfo)
	{
		UE_LOG(LogSampleAbilitySystem, Error, TEXT("Ability [%s] ValidateAbilityActorInfo: ActorInfo is not valid. Something serious is wrong."), *GetName());
		return false;
	}

	// ensure the owner is valid
	if (!ActorInfo->OwnerActor.IsValid())
	{
		UE_LOG(LogSampleAbilitySystem, Error, TEXT("Ability [%s] ValidateAbilityActorInfo: Owner is not valid. Something serious is wrong."), *GetName());
		return false;
	}

	// check if we should validate the Owner's class
	if (ExpectedOwnerClass != nullptr)
	{
		if (!ActorInfo->OwnerActor->IsA(ExpectedOwnerClass))
		{
			UE_LOG(LogSampleAbilitySystem, Warning, TEXT("Ability [%s] ValidateAbilityActorInfo: Owner [%s] is not of the expected class [%s]"), *GetName(), *ActorInfo->OwnerActor->GetName(), *ExpectedOwnerClass->GetName());
			return false;
		}
	}

	// check if we should validate the avatar
	if (bValidationRequiresAvatar)
	{
		if (!ActorInfo->AvatarActor.IsValid())
		{
			UE_LOG(LogSampleAbilitySystem, Warning, TEXT("Ability [%s] ValidateAbilityActorInfo: Ability requires an Avatar but ActorInfo has none."), *GetName());
			return false;
		}

		if (ExpectedAvatarClass != nullptr && !ActorInfo->AvatarActor->IsA(ExpectedAvatarClass))
		{
			UE_LOG(LogSampleAbilitySystem, Warning, TEXT("Ability [%s] ValidateAbilityActorInfo: Avatar [%s] is not of the expected class [%s]"), *GetName(), *ActorInfo->AvatarActor->GetName(), *ExpectedAvatarClass->GetName());
			return false;
		}
	}

	return true;
}

void USampleGameplayAbility::SetAbilityValidated(bool bValidated)
{
	// save the value generated through ValidateAbilityActorInfo()
	bAbilityValidated = bValidated;

	UE_CLOG(!bAbilityValidated, LogSampleAbilitySystem, Warning, TEXT("Ability [%s] failed ActorInfo validation."), *GetName());
}

void USampleGameplayAbility::OnOwnerInitialized()
{
	// is this the first time we're passed an owner?
	if (!bOwnerInitialized)
	{
		// raise the initialized flag
		bOwnerInitialized = true;

		// call the native and BP hooks
		NativeOnOwnerInitialized();
		K2_OnOwnerInitialized();
	}
	
}

void USampleGameplayAbility::NativeOnOwnerInitialized()
{
	// Subclasses can implement further checks and initialization here
}

void USampleGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	// call the native and BP hooks
	NativeOnAvatarChanged();
	K2_OnAvatarChanged();
}

bool USampleGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	// is the ability validated?
	if (bAbilityValidated)
	{
		// continue with the normal flow
		return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
	}

	return false;
}

void USampleGameplayAbility::NativeOnAvatarChanged()
{
	// Subclasses can implement further checks and initialization here
}

void USampleGameplayAbility::TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const
{
	// try to activate if the activation policy is OnSpawn
	if (ActorInfo && !Spec.IsActive() && (ActivationPolicy == EAbilityActivationPolicy::OnSpawn))
	{
		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
		const AActor* AvatarActor = ActorInfo->AvatarActor.Get();

		// if the avatar actor is torn off or about to be destroyed, don't try to activate until we get the new one.
		if (ASC && AvatarActor && !AvatarActor->GetTearOff() && (AvatarActor->GetLifeSpan() <= 0.0f))
		{
			// ensure we respect client/server execution policies
			const bool bIsLocalExecution = (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalPredicted) || (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalOnly);
			const bool bIsServerExecution = (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerOnly) || (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerInitiated);

			const bool bClientShouldActivate = ActorInfo->IsLocallyControlled() && bIsLocalExecution;
			const bool bServerShouldActivate = ActorInfo->IsNetAuthority() && bIsServerExecution;

			if (bClientShouldActivate || bServerShouldActivate)
			{
				ASC->TryActivateAbility(Spec.Handle);
			}
		}
	}
}

bool USampleGameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr*/) const
{
	// check built-in costs
	if (!ActorInfo || !Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags))
	{
		return false;
	}

	// verify we can afford any additional costs
	for (const TObjectPtr<UAbilityCost>& AdditionalCost : AdditionalCosts)
	{
		if (AdditionalCost)
		{
			if (!AdditionalCost->CheckCost(this, Handle, ActorInfo, OptionalRelevantTags))
			{
				return false;
			}
		}
	}

	return true;
}

void USampleGameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	// ensure the ActorInfo is valid
	UE_CLOG(!ActorInfo, LogSampleAbilitySystem, Error, TEXT("ApplyCost: ActorInfo not valid on ability [%s]"), *GetName());
	check(ActorInfo);

	// apply built-in costs
	Super::ApplyCost(Handle, ActorInfo, ActivationInfo);

	// pay any additional costs
	for (const TObjectPtr<UAbilityCost>& AdditionalCost : AdditionalCosts)
	{
		if (AdditionalCost)
		{
			AdditionalCost->ApplyCost(this, Handle, ActorInfo, ActivationInfo);
		}
	}
}

FSampleGameplayAbilityActorInfo USampleGameplayAbility::GetSampleActorInfo() const
{
	// get the specialized struct
	if (const FSampleGameplayAbilityActorInfo* SampleActorInfo = GetSampleActorInfoPtr())
	{
		return *SampleActorInfo;
	}

	return FSampleGameplayAbilityActorInfo();
}

APlayerController* USampleGameplayAbility::GetPlayerControllerFromActorInfo() const
{
	// get the specialized struct
	if (const FSampleGameplayAbilityActorInfo* SampleActorInfo = GetSampleActorInfoPtr())
	{
		return SampleActorInfo->PlayerController.Get();
	}

	return nullptr;
}

AAIController* USampleGameplayAbility::GetAIControllerFromActorInfo() const
{
	// get the specialized struct
	if (const FSampleGameplayAbilityActorInfo* SampleActorInfo = GetSampleActorInfoPtr())
	{
		return SampleActorInfo->AIController.Get();
	}

	return nullptr;
}

APlayerState* USampleGameplayAbility::GetPlayerStateFromActorInfo() const
{
	// get the specialized struct
	if (const FSampleGameplayAbilityActorInfo* SampleActorInfo = GetSampleActorInfoPtr())
	{
		return SampleActorInfo->PlayerState.Get();
	}

	return nullptr;
}

APawn* USampleGameplayAbility::GetPawnAvatarFromActorInfo() const
{
	// get the specialized struct
	if (const FSampleGameplayAbilityActorInfo* SampleActorInfo = GetSampleActorInfoPtr())
	{
		return SampleActorInfo->PawnAvatar.Get();
	}

	return nullptr;
}

bool USampleGameplayAbility::IsAIControlled() const
{
	// get the specialized struct
	if (const FSampleGameplayAbilityActorInfo* SampleActorInfo = GetSampleActorInfoPtr())
	{
		return SampleActorInfo->AIController.IsValid();
	}

	return false;
}

bool USampleGameplayAbility::IsPlayerControlled() const
{
	// get the specialized struct
	if (const FSampleGameplayAbilityActorInfo* SampleActorInfo = GetSampleActorInfoPtr())
	{
		return SampleActorInfo->PlayerController.IsValid();
	}

	return false;
}

const FSampleGameplayAbilityActorInfo* USampleGameplayAbility::GetSampleActorInfoPtr() const
{
	// cast the ActorInfo struct to the specialized type
	const FSampleGameplayAbilityActorInfo* SampleActorInfo = static_cast<const FSampleGameplayAbilityActorInfo*>(CurrentActorInfo);

	// ensure the struct is valid
	ensure(SampleActorInfo);

	return SampleActorInfo;
}