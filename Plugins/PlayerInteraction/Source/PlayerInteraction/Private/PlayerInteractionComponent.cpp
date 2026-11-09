// Copyright Epic Games, Inc. All Rights Reserved.


#include "PlayerInteractionComponent.h"
#include "AbilitySystemInterface.h"
#include "InteractionInterface.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "AbilitySystemComponent.h"
#include "MoverComponent.h"
#include "MoverTypes.h"
#include "TitanMoverTypes.h"
#include "Curves/CurveFloat.h"
#include "Logging/LogMacros.h"

DEFINE_LOG_CATEGORY(LogInteractionComponent);

UPlayerInteractionComponent::UPlayerInteractionComponent()
{
	OnComponentBeginOverlap.AddDynamic(this, &UPlayerInteractionComponent::OnBeginOverlap);
	OnComponentEndOverlap.AddDynamic(this, &UPlayerInteractionComponent::OnEndOverlap);
}

void UPlayerInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ensure the owner has access to an ability system
	AbilityOwner = Cast<IAbilitySystemInterface>(GetOwner());

	// get the mover component
	MoverComp = Cast<UMoverComponent>(GetOwner()->FindComponentByClass(UMoverComponent::StaticClass()));

	// set the update targets timer
	GetWorld()->GetTimerManager().SetTimer(UpdateTargetsTimer, this, &UPlayerInteractionComponent::UpdateTargets, TimeBetweenTargetUpdates, true);
}

void UPlayerInteractionComponent::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	// clear the update targets timer
	GetWorld()->GetTimerManager().ClearTimer(UpdateTargetsTimer);

	Super::EndPlay(EndPlayReason);
}

void UPlayerInteractionComponent::RequestInteraction()
{
	// ensure we have a target and the targeting conditions are valid
	if (IsValid(CurrentTarget) && CheckTargetingConditions())
	{
		TScriptInterface<IPlayerInteractionInterface> InteractionActor(CurrentTarget);

		if (InteractionActor)
		{
			// request an interaction 
			if (InteractionActor->RequestInteraction(GetOwner()))
			{
				// interact with the owner
				InteractionActor->BeginInteraction(GetOwner());

				// call the delegate
				OnInteractionStarted.Broadcast(CurrentTarget);
			}
		}
	}
}

void UPlayerInteractionComponent::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// is this an interactive actor?
	TScriptInterface<IPlayerInteractionInterface> InteractionActor(OtherActor);

	if (InteractionActor)
	{
		// do we not currently have a target?
		if (!CurrentTarget)
		{
			UpdateTargets();
		}
		else
		{
			// check if we've overlapped with a pending removal current target
			if (OtherActor == CurrentTarget)
			{
				// reset the pending removal flag
				bCurrentTargetRemoved = false;
				return;
			}
			else
			{

				// if the current target has been selected for longer than the min time, reselect the target immediately
				if (GetWorld()->GetTimeSeconds() - CurrentTargetAddedTime > MinTargetingTime)
				{
					UpdateTargets();
				}

			}
		
		}
	}
}

void UPlayerInteractionComponent::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// have we stopped overlapping the current target?
	if (CurrentTarget && OtherActor == CurrentTarget)
	{
		// flag it for removal
		bCurrentTargetRemoved = true;

		// reselect targets immediately
		UpdateTargets();
	}
}

void UPlayerInteractionComponent::UpdateTargets()
{
	// ensure we have valid target scoring curves
	if (!DistanceScoreCurve || !FacingScoreCurve)
	{
		UE_LOG(LogInteractionComponent, Error, TEXT("UPlayerInteractionComponent: Score curves not set on component [%s]"), *GetName());
		return;
	}

	// cache whether we had a target prior to retargeting
	const bool bHadTarget = CurrentTarget != nullptr;

	// has the current target been flagged for deferred removal?
	if (bCurrentTargetRemoved)
	{
		// has the sticky selection time ended?
		if (GetWorld()->GetTimeSeconds() - CurrentTargetAddedTime > MinTargetingTime)
		{
			DeselectCurrentTarget();

			// reset the removal flag
			bCurrentTargetRemoved = false;
		}
	}

	// are we being prevented from targeting by tags?
	if (!CheckTargetingConditions())
	{
		// if we had a current target, deselect it and notify we've lost the target
		DeselectCurrentTarget();

		if (bHadTarget)
		{
			OnTargetLost.Broadcast();
		}

		return;
	}

	// get our current facing, we'll use this as reference for cone checks when scoring targets
	const FVector CurrentFacing = GetForwardVector();
	const FVector OwnerLocation = GetOwner()->GetActorLocation();

	// initialize the occlusion check sweep parameters
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(InteractionBox), false);
	FCollisionShape CollisionSphere = FCollisionShape::MakeSphere(CollisionProbeRadius);
	FHitResult HitResult;

	// choose a new target
	AActor* ChosenTarget = nullptr;
	float HighestScore = 0.0f;

	// iterate through all overlapping actors
	TSet<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors);

	// iterate through the list
	for (AActor* Current : OverlappingActors)
	{
		// we only care about actors with interaction interfaces
		TScriptInterface<IPlayerInteractionInterface> InteractionCurrent(Current);

		if (InteractionCurrent)
		{
			// get the target's score
			float TargetScore = InteractionCurrent->ScoreTarget(GetOwner());

			// skip this target if its scoring is too low
			if (TargetScore <= 0.0f)
			{
				continue;
			}

			// calculate the facing vector so we can do distance and dot checks
			FVector TargetFacing = Current->GetActorLocation() - OwnerLocation;

			// score the distance
			const float Dist = TargetFacing.Size2D();
			const float DistScore = DistanceScoreCurve->GetFloatValue(Dist);

			// score the camera facing
			TargetFacing.Normalize();
			const float Dot = FVector::DotProduct(CurrentFacing, TargetFacing);
			const float FacingScore = FacingScoreCurve->GetFloatValue(Dot);

			// multiply the scores to get the final score
			TargetScore = TargetScore * DistScore * FacingScore;

			// do we have a new highest scored target?
			if (TargetScore > HighestScore)
			{
				// run a sweep to ensure the target is not occluded behind a wall or another actor
				HitResult.Reset();

				QueryParams.ClearIgnoredSourceObjects();
				QueryParams.AddIgnoredActor(Current);
				QueryParams.AddIgnoredActor(GetOwner());

				GetWorld()->SweepSingleByChannel(HitResult, OwnerLocation, Current->GetActorLocation(), FQuat::Identity, CollisionProbeChannel, CollisionSphere, QueryParams);

				// only override the result 
				if (!HitResult.bBlockingHit)
				{
					// save the new highest score
					HighestScore = TargetScore;

					// save the target
					ChosenTarget = Current;
				}
			}
		}
	}

	// were we unable to choose any targets?
	// this could be due to occlusion or target-side scoring biases
	if (!ChosenTarget)
	{
		// deselect the current target if we have it
		DeselectCurrentTarget();

		//  if we had a target, call the delegate
		if (bHadTarget)
		{
			OnTargetLost.Broadcast();
		}

		// reset the target removed flag
		bCurrentTargetRemoved = false;

		return;
	}

	// have we changed targets?
	if (CurrentTarget != ChosenTarget)
	{
		// deselect the old target
		DeselectCurrentTarget();

		// select the new target
		CurrentTarget = ChosenTarget;

		TScriptInterface<IPlayerInteractionInterface> InteractionActor(CurrentTarget);

		if (InteractionActor)
		{
			InteractionActor->OnBeginTargeted();
		}

		// call the target changed delegate
		OnTargetChanged.Broadcast(CurrentTarget);

		// save the added time
		CurrentTargetAddedTime = GetWorld()->GetTimeSeconds();

		// reset the target removed flag
		bCurrentTargetRemoved = false;
	}
}

bool UPlayerInteractionComponent::CheckTargetingConditions()
{
	// assume we pass by default
	bool bPass = true;

	// skip the ASC query if it's empty
	if (!AbilitySystemTagQuery.IsEmpty() && AbilityOwner)
	{
		// skip the ASC query if we do not have access to an ASC
		if (UAbilitySystemComponent* ASC = AbilityOwner->GetAbilitySystemComponent())
		{
			bPass &= ASC->MatchesGameplayTagQuery(AbilitySystemTagQuery);
		}	
	}

	// skip the Mover query if it's empty or if the Mover Component is not available
	if (!MoverTagQuery.IsEmpty() && MoverComp)
	{
		// get the tags sync state
		if (const FTitanTagsSyncState* SyncState = MoverComp->GetSyncState().SyncStateCollection.FindDataByType<FTitanTagsSyncState>())
		{
			bPass &= SyncState->GetMovementTags().MatchesQuery(MoverTagQuery);
		}

	}
	
	return bPass;
}

void UPlayerInteractionComponent::DeselectCurrentTarget()
{
	if (IsValid(CurrentTarget))
	{
		// notify the actor it's no longer targeted
		TScriptInterface<IPlayerInteractionInterface> InteractionActor(CurrentTarget);

		if (InteractionActor)
		{
			InteractionActor->OnEndTargeted();
		}
	}

	// clear the target
	CurrentTarget = nullptr;
}
