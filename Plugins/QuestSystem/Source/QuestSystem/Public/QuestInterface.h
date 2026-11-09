// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "QuestInterface.generated.h"

class UQuestManagerComponent;

/**
 *  IQuestInterface
 *  Helper interface to allow Actors such as Pawns to redirect to an Quest Manager Component
 */
UINTERFACE(MinimalAPI)
class UQuestInterface : public UInterface
{
	GENERATED_BODY()
};

class QUESTSYSTEM_API IQuestInterface
{
	GENERATED_BODY()

public:

	/** Returns the Quest Manager Component to use for this actor. It may live on another actor, such as a Pawn using the PlayerState's component */
	virtual UQuestManagerComponent* GetQuestManagerComponent() const = 0;
};
