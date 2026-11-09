// Copyright Epic Games, Inc. All Rights Reserved.


#include "InteractiveActor.h"
#include "Curves/CurveFloat.h"

AInteractiveActor::AInteractiveActor()
{
 	PrimaryActorTick.bCanEverTick = true;

}

void AInteractiveActor::OnBeginTargeted()
{
	// call the BP hook
	BP_OnBeginTargeted();
}

void AInteractiveActor::OnEndTargeted()
{
	// call the BP hook
	BP_OnEndTargeted();
}

bool AInteractiveActor::RequestInteraction(AActor* RequestingActor)
{
	// return true by default
	// subclasses can implement more complex logic here
	return true;
}

void AInteractiveActor::BeginInteraction(AActor* Interactor)
{
	// call the BP hook
	BP_OnBeginInteraction(Interactor);
}

float AInteractiveActor::ScoreTarget(const AActor* Interactor) const
{
	// start with the base score
	float FinalScore = TargetingScore;

	if (Interactor)
	{
		// get the facing vector and distance to the interactor
		FVector FacingVector = Interactor->GetActorLocation() - GetActorLocation();
		
		const float DistToInteractor = FacingVector.Size2D();

		// return immediately if we're too far away
		if (DistToInteractor > MaxInteractionDistance)
		{
			return 0.0f;
		}

		// should we apply distance scoring?
		if (bUseDistanceScoring && DistanceScoreCurve)
		{
			// apply the distance score multiplier
			const float NormalizedDist = 1.0f - (DistToInteractor / FMath::Max(MaxInteractionDistance, 1.0f));

			FinalScore *= DistanceScoreCurve->GetFloatValue(NormalizedDist);
		}

		// should we apply positional facing scoring?
		if (bUsePositionalFacingScoring && PositionalFacingScoreCurve)
		{
			// ensure the facing vector is normalized, we only care about the angle
			FacingVector.Normalize();

			// apply the facing score multiplier based on the x-axis facing dot to the interactor's facing vector
			const float Dot = FVector::DotProduct(GetActorForwardVector(), FacingVector);

			FinalScore *= PositionalFacingScoreCurve->GetFloatValue(Dot);
		}

		// should we apply interactor facing scoring?
		if (bUseInteractorFacingScoring && InteractorFacingScoreCurve)
		{
			// apply the facing score multiplier based on the x-axis facing dot to the interactor's x-axis vector
			const float Dot = FVector::DotProduct(GetActorForwardVector(), Interactor->GetActorForwardVector());

			FinalScore *= InteractorFacingScoreCurve->GetFloatValue(Dot);
		}
	}

	return FinalScore;
}

const FText AInteractiveActor::GetInteractionVerbText() const
{
	return InteractionVerbText;
}
