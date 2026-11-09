// Copyright Epic Games, Inc. All Rights Reserved.


#include "Framework/SampleNPC.h"
#include "Framework/SampleNPCController.h"
#include "GAS/SampleAbilitySystemComponent.h"
#include "Components/StateTreeAIComponent.h"
#include "StructUtils/InstancedStruct.h"
#include "StateTree.h"

ASampleNPC::ASampleNPC(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) 
{
 	PrimaryActorTick.bCanEverTick = true;

	// set the controller class
	AIControllerClass = ASampleNPCController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// create the ASC
	AbilitySystem = CreateDefaultSubobject<USampleAbilitySystemComponent>(TEXT("Ability System Component"));
	check(AbilitySystem);

}

void ASampleNPC::BeginPlay()
{
	Super::BeginPlay();

	// TODO: this is here as a workaround to Auto Possess AI initialization timing vs the ASC,
	// because by the time the NPC gets possessed the ASC has not yet had time to grant its ability sets
	TryInitializeAbilities();
}

void ASampleNPC::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// cast the npc controller
	NPCController = Cast<ASampleNPCController>(NewController);

	// initialize the abilities
	TryInitializeAbilities();
}

void ASampleNPC::UnPossessed()
{
	// clear the npc controller ptr
	NPCController = nullptr;

	Super::UnPossessed();
}

void ASampleNPC::TryInitializeAbilities()
{
	// initialize the ability system
	if (AController* MyController = GetController())
	{
		AbilitySystem->InitAbilityActorInfo(MyController, this);

		// ensure abilities that are activated on spawn are ready
		AbilitySystem->TryActivateAbilitiesOnSpawn();
	}
}

UAbilitySystemComponent* ASampleNPC::GetAbilitySystemComponent() const
{
	return AbilitySystem;
}

ASampleNPCController* ASampleNPC::GetNPCController() const
{
	return NPCController;
}

UStateTree* ASampleNPC::GetStartingStateTree() const
{
	return StartingStateTree;
}

void ASampleNPC::SendStateTreeEvent(const FGameplayTag& Tag, const FInstancedStruct& Payload)
{
	if (NPCController)
	{
		if (UStateTreeAIComponent* StateTree = NPCController->GetStateTreeComponent())
		{
			StateTree->SendStateTreeEvent(Tag, Payload);
		}
	}
}
