// Copyright Epic Games, Inc. All Rights Reserved.


#include "Framework/SampleNPCController.h"
#include "Framework/SampleNPC.h"
#include "Components/StateTreeAIComponent.h"
#include "SampleLogging.h"

ASampleNPCController::ASampleNPCController(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	// ensure the controller follows the pawn so EQS works as expected
	bAttachToPawn = true;

	// create the StateTree AI Component
	StateTreeAI = CreateDefaultSubobject<UStateTreeAIComponent>(TEXT("StateTree AI"));
	check(StateTreeAI);

	// ensure we don't start logic automatically
	StateTreeAI->SetStartLogicAutomatically(false);
}

void ASampleNPCController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// cast the pawn
	if (ASampleNPC* NPCPawn = Cast<ASampleNPC>(InPawn))
	{
		// set the starting StateTree from the NPC
		if (UStateTree* StartingStateTree = NPCPawn->GetStartingStateTree())
		{
			StateTreeAI->SetStateTree(StartingStateTree);
			StateTreeAI->StartLogic();
		}
		else
		{
			UE_LOG(LogSampleFramework, Warning, TEXT("[%s]: No starting StateTree set for possessed Pawn [%s]"), *GetName(), *NPCPawn->GetName());
		}

		// ensure all abilities on the NPC are initialized
		NPCPawn->TryInitializeAbilities();

	}
	else
	{
		UE_LOG(LogSampleFramework, Warning, TEXT("[%s]: Not possessing a Pawn of type SampleNPC"), *GetName());
	}
}

void ASampleNPCController::OnUnPossess()
{
	// ensure StateTree is stopped on unpossess
	StateTreeAI->StopLogic("Pawn Unpossessed");

	Super::OnUnPossess();
}

UStateTreeAIComponent* ASampleNPCController::GetStateTreeComponent() const
{
	return StateTreeAI;
}
