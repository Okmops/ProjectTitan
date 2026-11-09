// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"
#include "DialogueNode.generated.h"

class UMaterialInterface;

/** Response event sent when a dialogue node is completed */
STATETREEDIALOGUE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Dialogue_DialogueComplete);

/** Response event sent when a dialogue node is cancelled */
STATETREEDIALOGUE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Dialogue_DialogueCancelled);

/** Base tag for dialogue node response events */
STATETREEDIALOGUE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Dialogue_DialogueResponse);

/**
 *  FDialoguePage
 *  Represents a single page of dialog.
 *  Supports optional title and portrait.
 */
 USTRUCT(BlueprintType)
struct STATETREEDIALOGUE_API FDialoguePage
{
	GENERATED_BODY()

	/** Optional title to display for this page */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue Page")
	FText Title;

	/** Main text for the page for this page */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue Page")
	FText PageText;

	/** Optional portrait to display for this page */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue Page")
	TObjectPtr<UMaterialInterface> Portrait = nullptr;
};

/**
 *  FDialogueResponse
 *  Represents a single dialogue tree response.
 *  These can be used by other systems to build procedural player response UI.
 *  ResponseTag can then be sent back through a StateTree event to implement dialogue branching.
 */
USTRUCT(BlueprintType)
struct STATETREEDIALOGUE_API FDialogueResponse
{
	GENERATED_BODY()

	/** Event Tag for the response event to send back to the owning StateTree */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue Response")
	FGameplayTag ResponseTag;

	/** Text to display on the response button */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue Response")
	FText ResponseText;
};

/**
 *  FDialogueResponsePayload
 *  Holds optional user-specified data to be included with the dialogue tree response.
 */
USTRUCT(BlueprintType)
struct STATETREEDIALOGUE_API FDialogueResponsePayload
{
	GENERATED_BODY()

	/** Optional text data to add to the response */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Response Data")
	FText TextData;

	/** Optional int data to add to the response */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Response Data")
	int32 IntData = 0;

	/** Optional float data to add to the response */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Response Data")
	float FloatData = 0.0f;

	/** Optional vector data to add to the response */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Response Data")
	FVector VectorData = FVector::ZeroVector;
};

/**
 *  FDialogueNode
 *  Represents a single node in the dialogue tree
 */
USTRUCT(BlueprintType)
struct STATETREEDIALOGUE_API FDialogueNode
{
	GENERATED_BODY()

	/** List of pages for this dialogue tree node */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pages")
	TArray<FDialoguePage> Pages;

	/** Optional list of responses that will move the dialogue tree forward. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Responses")
	TArray<FDialogueResponse> Responses;

	/** Optional list of Float values passed to this node. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	TArray<float> FloatValues;

	/** Optional list of Int values for this node */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	TArray<int32> IntValues;

	/** Optional list of Text values for this node */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	TArray<FText> TextValues;

	/** If true, this dialogue tree node can be cancelled out of by external actions, such as the player going out of range, etc. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Advanced")
	bool bIsCancellable = false;
};