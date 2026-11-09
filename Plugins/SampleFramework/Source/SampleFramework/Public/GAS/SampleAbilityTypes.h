// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "SampleAbilityTypes.generated.h"

class APawn;
class AAIController;
class AActor;
class APlayerState;

/**
 *  FSampleGameplayAbilityActorInfo
 *  Expanded ActorInfo struct
 *  Provides additional precast references to useful classes for the ability system
 */
USTRUCT(BlueprintType)
struct FSampleGameplayAbilityActorInfo : public FGameplayAbilityActorInfo
{
	GENERATED_USTRUCT_BODY()

	virtual ~FSampleGameplayAbilityActorInfo() {}

	/** Precast Pawn avatar. This will often be null! */
	UPROPERTY(BlueprintReadOnly, Category = "ActorInfo")
	TWeakObjectPtr<APawn> PawnAvatar;

	/** Precast PlayerState associated with the owning actor. This will often be null! */
	UPROPERTY(BlueprintReadOnly, Category = "ActorInfo")
	TWeakObjectPtr<APlayerState> PlayerState;

	/** Precast AIController associated with the owning actor. This will often be null! */
	UPROPERTY(BlueprintReadOnly, Category = "ActorInfo")
	TWeakObjectPtr<AAIController> AIController;

	/** Override to set the cast pointers */
	virtual void InitFromActor(AActor *OwnerActor, AActor *AvatarActor, UAbilitySystemComponent* InAbilitySystemComponent) override;

	/** Override to clean the cast pointers */
	virtual void ClearActorInfo() override;
};