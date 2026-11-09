// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "GameplayTagContainer.h"
#include "DialogueButton.generated.h"

class UDialogueWindow;

/**
 *  UDialogueResponseButton
 *	Button representing a user response message to a dialogue node.
 *  Expects to be owned by a UDialogueWindow.
 */
UCLASS(abstract)
class STATETREEDIALOGUE_API UDialogueResponseButton : public UCommonButtonBase
{
	GENERATED_BODY()
	
protected:

	/** Window that owns this response button */
	UPROPERTY(BlueprintReadOnly, Category = "Titan|Dialogue")
	TObjectPtr<UDialogueWindow> ParentWindow;

	/** Response tag to send when the button is activated */
	UPROPERTY(BlueprintReadOnly, Category = "Titan|Dialogue")
	FGameplayTag ResponseTag;

protected:

	/** Override to send the response when the button is clicked */
	virtual void HandleButtonClicked() override;

public:

	/** Initializes the response button */
	UFUNCTION(BlueprintCallable, Category = "Titan|Dialogue")
	void InitializeButton(UDialogueWindow* Window, const FGameplayTag& Response);

};
