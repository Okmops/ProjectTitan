// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractionInterface.generated.h"

/**
  *  IPlayerInteractionInterface
  *  Interface for native actors to receive and process interaction requests.
  *  NOTE: this interface should be implemented in C++
  */
UINTERFACE(MinimalAPI, NotBlueprintable)
class UPlayerInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

class PLAYERINTERACTION_API IPlayerInteractionInterface
{
	GENERATED_BODY()

public:

	/** Notifies the actor that it's being targeted for interaction */
	virtual void OnBeginTargeted() = 0;

	/** Notifies the actor that it's stopped being targeted for interaction */
	virtual void OnEndTargeted() = 0;

	/** Requests an interaction with this actor. Should return true if the interaction was accepted. */
	UFUNCTION(BlueprintCallable, Category = "Interactive Actor")
	virtual bool RequestInteraction(AActor* RequestingActor) = 0;

	/** Starts the interaction behavior. Assumes RequestInteraction was called beforehand and returned true. */
	UFUNCTION(BlueprintCallable, Category = "Interactive Actor")
	virtual void BeginInteraction(AActor* Interactor) = 0;

	/** Scores this actor for target selection. Return a higher value to increase target priority. Return 0 or lower to prevent targeting. */
	virtual float ScoreTarget(const AActor* Interactor) const = 0;

	/** Returns the interaction verb text to display when this actor is targeted */
	UFUNCTION(BlueprintCallable, Category = "Interactive Actor")
	virtual const FText GetInteractionVerbText() const = 0;

};
