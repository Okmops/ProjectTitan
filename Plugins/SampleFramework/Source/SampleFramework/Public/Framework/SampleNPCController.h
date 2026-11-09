// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SampleNPCController.generated.h"

class UStateTreeAIComponent;

/**
 *  ASampleNPCController
 *  AI Controller class meant to be used with ASampleNPC
 *  - Provides StateTree functionality to the possessed Pawn
 */
UCLASS()
class SAMPLEFRAMEWORK_API ASampleNPCController : public AAIController
{
	GENERATED_BODY()
	
public:

	/** Constructor */
	ASampleNPCController(const FObjectInitializer& ObjectInitializer);

protected:

	/** StateTree Component. We use the specialized class that handles AI Controller StateTree schemas and provides expanded support for tasks such as MoveTo. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStateTreeAIComponent> StateTreeAI;

protected:

	/** Initializes the NPC on possess */
	virtual void OnPossess(APawn* InPawn) override;

	/** NPC cleanup on unpossess */
	virtual void OnUnPossess() override;

public:

	/** Returns the StateTree component for this controller */
	UFUNCTION(BlueprintPure, Category = "AI")
	UStateTreeAIComponent* GetStateTreeComponent() const;
};
