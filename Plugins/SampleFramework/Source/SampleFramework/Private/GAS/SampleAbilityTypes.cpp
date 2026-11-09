// Copyright Epic Games, Inc. All Rights Reserved.


#include "GAS/SampleAbilityTypes.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/Pawn.h"
#include "AIController.h"

void FSampleGameplayAbilityActorInfo::InitFromActor(AActor* InOwnerActor, AActor* InAvatarActor, UAbilitySystemComponent* InAbilitySystemComponent)
{
	Super::InitFromActor(InOwnerActor, InAvatarActor, InAbilitySystemComponent);

	// Precast the avatar to Pawn
	PawnAvatar = Cast<APawn>(InAvatarActor);

	// Precast the owner to PlayerState
	PlayerState = Cast<APlayerState>(InOwnerActor);

	// Try to get the AIController from the Pawn
	if (PawnAvatar.IsValid())
	{
		AIController = Cast<AAIController>(PawnAvatar->GetController());
	
	}
	else if (PlayerState.IsValid())
	{

		AIController = Cast<AAIController>(PlayerState->GetOwningController());

	}
	else
	{

		// Skip searching for AIController if we already have a valid PlayerController
		if (!PlayerController.IsValid())
		{
			// Look for an AIController in the owner chain.
			AActor *TestActor = InOwnerActor;
			while (TestActor)
			{
				if (AAIController* CastPC = Cast<AAIController>(TestActor))
				{
					AIController = CastPC;
					break;
				}

				TestActor = TestActor->GetOwner();
			}
		}

	}

	
}

void FSampleGameplayAbilityActorInfo::ClearActorInfo()
{
	Super::ClearActorInfo();

	PawnAvatar = nullptr;
	PlayerState = nullptr;
	AIController = nullptr;
}