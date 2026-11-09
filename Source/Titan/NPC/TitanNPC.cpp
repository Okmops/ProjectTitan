// Copyright Epic Games, Inc. All Rights Reserved.


#include "NPC/TitanNPC.h"
#include "Framework/SampleNPCController.h"
#include "Character/TitanGameplayCharacter.h"
#include "Curves/CurveFloat.h"
#include "StructUtils/InstancedStruct.h"

UE_DEFINE_GAMEPLAY_TAG(TAG_Titan_NPC_InteractionEvent, "Titan.NPC.Interaction.InteractionEvent");

ATitanNPC::ATitanNPC(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InteractionEventTag = TAG_Titan_NPC_InteractionEvent;
}

void ATitanNPC::OnBeginTargeted()
{
	// call the BP hook
	BP_OnBeginTargeted();
}

void ATitanNPC::OnEndTargeted()
{
	// call the BP hook
	BP_OnEndTargeted();
}

bool ATitanNPC::RequestInteraction(AActor* RequestingActor)
{
	// ensure we're interacting with a player character
	if (ATitanGameplayCharacter* TitanInteractor = Cast<ATitanGameplayCharacter>(RequestingActor))
	{
		// @TODO: query the character's ASC for targeting tags

		return true;
	}

	return false;
}

void ATitanNPC::BeginInteraction(AActor* Interactor)
{
	// ensure we're interacting with a player character
	if (ATitanGameplayCharacter* TitanInteractor = Cast<ATitanGameplayCharacter>(Interactor))
	{
		// build the payload
		FTitanNPCInteractionPayload Payload;
		Payload.InteractingCharacter = TitanInteractor;
		Payload.SimpleDialogue = InteractionSimpleDialogue;

		// send the event to the controller's StateTree
		SendStateTreeEvent(InteractionEventTag, FInstancedStruct::Make(Payload));
	}
}

float ATitanNPC::ScoreTarget(const AActor* Interactor) const
{
	// start with the base score
	float FinalScore = TargetingScore;

	// ensure we're interacting with a player character
	if (const ATitanGameplayCharacter* TitanInteractor = Cast<ATitanGameplayCharacter>(Interactor))
	{
		// @TODO: query the character's ASC for targeting tags

		// get the facing vector
		FVector FacingVector = TitanInteractor->GetActorLocation() - GetActorLocation();
		
		// should we apply distance scoring?
		if (bUseDistanceScoring && DistanceScoreCurve)
		{
			// apply the distance score multiplier
			const float NormalizedDist = 1.0f - (FacingVector.Size2D() / MaxInteractionDistance);

			FinalScore *= DistanceScoreCurve->GetFloatValue(NormalizedDist);
		}

	}

	return FinalScore;
}

const FText ATitanNPC::GetInteractionVerbText() const
{
	return InteractionVerbText;
}
