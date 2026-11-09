// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SamplePlayerController.generated.h"

class UInputMappingContext;

/**
 *  ASamplePlayerController
 *  Provides simplified Controller-level input mappings
 */
UCLASS()
class SAMPLEFRAMEWORK_API ASamplePlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ASamplePlayerController(const FObjectInitializer& ObjectInitializer);

protected:

	/** BeginPlay initialization */
	virtual void BeginPlay() override;

protected:

	/** Default Input Mapping Contexts to add to the player on initialization */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TArray< TObjectPtr<UInputMappingContext> > DefaultMappingContexts;

	/** Input Mapping Contexts to add to the player when menus are open */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TArray< TObjectPtr<UInputMappingContext> > MenuMappingContexts;

	/** If true, menu input mappings are active */
	bool bMenuMappingsActive = false;

public:

	/** Override to provide post process input support to the player's Ability System Component */
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;

	/** Enables or disables menu input mappings */
	UFUNCTION(BlueprintCallable, Category="Input")
	void SetMenuInputsEnabled(bool bEnabled);
};
