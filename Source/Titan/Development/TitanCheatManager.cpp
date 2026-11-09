// Copyright Epic Games, Inc. All Rights Reserved.

#include "Development/TitanCheatManager.h"
#include "TitanLogChannels.h"
#include "TitanGameplayCharacter.h"
#include "Framework/TitanGameplayPlayerState.h"
#include "InventoryManagerComponent.h"
#include "QuestComponent.h"
#include "Components/CapsuleComponent.h"
#include "Framework/SamplePlayerController.h"
#include "TitanMoverComponent.h"
#include "Engine/World.h"
#include "Engine/Engine.h"


void UTitanCheatManager::TitanTeleportPlayer3D(float XCoord, float YCoord, float ZCoord)
{
	// get the player controller
	if (APlayerController* PC = GetOuterAPlayerController())
	{
		// get the pawn
		if(ATitanGameplayCharacter* Pawn = Cast<ATitanGameplayCharacter>(PC->GetPawn()))
		{
			// queue the pawn's teleport move
			Pawn->QueueTeleportMove(FVector(XCoord * 100.0f, YCoord * 100.0f, ZCoord * 100.0f));
		}
	}
}

void UTitanCheatManager::TitanTeleportPlayer2D(float XCoord, float YCoord)
{
	// get the player controller
	if (APlayerController* PC = GetOuterAPlayerController())
	{
		// get the pawn
		if (ATitanGameplayCharacter* Pawn = Cast<ATitanGameplayCharacter>(PC->GetPawn()))
		{
			// build the trace start and end locations
			FVector Start(XCoord * 100.0f, YCoord * 100.0f, TeleportMaxSweepZ);
			FVector End(XCoord * 100.0f, YCoord * 100.0f, TeleportMinSweepZ);

			UE_LOG(LogTitan, Log, TEXT("Start [%s] End [%s]"), *Start.ToCompactString(), *End.ToCompactString());

			// get the collision capsule size
			float Radius = 0.0f;
			float HalfHeight = 0.0f;

			Pawn->GetPlayerCapsule()->GetScaledCapsuleSize(Radius, HalfHeight);

			UE_LOG(LogTitan, Log, TEXT("Radius [%f] HalfHeight [%f]"), Radius, HalfHeight);
			
			FCollisionShape CollisionCapsule;
			CollisionCapsule.SetCapsule(Radius, HalfHeight);

			// initialize the collision params
			FCollisionQueryParams QueryParams;
			FCollisionResponseParams ResponseParams;
			//Pawn->GetPlayerCapsule()->InitSweepCollisionParams(QueryParams, ResponseParams);
			
			FHitResult Hit;

			// do a downwards sweep to find out the teleport location
			GetWorld()->SweepSingleByChannel(Hit,
				Start, End, Pawn->GetActorQuat(),
				Pawn->GetPlayerCapsule()->GetCollisionObjectType(), CollisionCapsule,
				QueryParams, ResponseParams);

			FVector TeleportLoc;

			// skip the teleport if the sweep started penetrating
			if (!Hit.bStartPenetrating)
			{
				// did we get a collision?
				if (Hit.bBlockingHit)
				{
					// teleport to the collision location
					TeleportLoc = Hit.Location;
				}
				else {

					// otherwise teleport to the sweep endpoint
					TeleportLoc = End;
				}

				UE_LOG(LogTitan, Log, TEXT("Resolved teleport to location [%s]"), *TeleportLoc.ToCompactString());

				// do the teleport
				TitanTeleportPlayer3D(TeleportLoc.X * 0.01f, TeleportLoc.Y * 0.01f, TeleportLoc.Z * 0.01f);
			}
			else {
				UE_LOG(LogTitan, Log, TEXT("Sweep started penetrating, skipping teleport"));
			}

		}
	}
}

void UTitanCheatManager::TitanTeleportPlayerToLandmark(int32 MarkerID)
{
	// TODO
}

void UTitanCheatManager::TitanGetMapCoords()
{
	// get the player controller
	if (APlayerController* PC = GetOuterAPlayerController())
	{
		FVector Loc;

		// get the pawn
		if (ATitanGameplayCharacter* Pawn = Cast<ATitanGameplayCharacter>(PC->GetPawn()))
		{
			// report the Pawn's current location
			Loc = Pawn->GetActorLocation() * 0.01f;

			UE_LOG(LogTitan, Warning, TEXT("Map Location [%s]"), *Loc.ToCompactString());
			
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, *Loc.ToCompactString());
			}
			
		}

		
	}
}

void UTitanCheatManager::TitanToggleStamina()
{
	// get the player controller
	if (APlayerController* PC = GetOuterAPlayerController())
	{
		// get the pawn
		if (ATitanGameplayCharacter* Pawn = Cast<ATitanGameplayCharacter>(PC->GetPawn()))
		{
			// report the Pawn's current location
			Pawn->GetMoverComponent()->ToggleStamina();

			if (Pawn->GetMoverComponent()->IsStaminaEnabled())
			{
				UE_LOG(LogTitan, Warning, TEXT("Stamina ENABLED"));

				if (GEngine)
				{

					GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Stamina ENABLED"));
				}
			}
			else {
				UE_LOG(LogTitan, Warning, TEXT("Stamina DISABLED"));

				if (GEngine)
				{

					GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Stamina DISABLED"));
				}
			}

		}
	}
}

void UTitanCheatManager::TitanAddItem(FGameplayTag ItemTag, int32 Count = 1)
{
	// get the player controller
	if (APlayerController* PC = GetOuterAPlayerController()) 
	{
		// get the player pawn
		if (ATitanGameplayCharacter* Pawn = Cast<ATitanGameplayCharacter>(PC->GetPawn()))
		{
			// get player state
			if (ATitanGameplayPlayerState* State = Cast<ATitanGameplayPlayerState>(Pawn->GetPlayerState()))
			{
				// get player inventory
				if (UInventoryManagerComponent* Inventory = Cast<UInventoryManagerComponent>(State->GetInventory()))
				{

					// attempts to add the item
					if (Inventory->AddItem(ItemTag, Count))
					{
						GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Item(s) added."));
					}
					else
					{
						GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Failed to add item: Check ItemTag, item count, or if the stack is not full."));
					}
				}
			}
		}
	}
}

void UTitanCheatManager::TitanRemoveItem(FGameplayTag ItemTag, int32 Count)
{
	// get the player controller
	if (APlayerController* PC = GetOuterAPlayerController())
	{
		// get the player pawn
		if (ATitanGameplayCharacter* Pawn = Cast<ATitanGameplayCharacter>(PC->GetPawn()))
		{
			// get player state
			if (ATitanGameplayPlayerState* State = Cast<ATitanGameplayPlayerState>(Pawn->GetPlayerState()))
			{
				// get player inventory
				if (UInventoryManagerComponent* Inventory = Cast<UInventoryManagerComponent>(State->GetInventory()))
				{
					// checks if the item is in the inventory, and attempts to remove it
					if (Inventory->HasItem(ItemTag) && Inventory->RemoveItem(ItemTag, Count))
					{
						GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Item(s) removed."));
					}
					else
					{
						GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Failed to remove item: Check ItemTag or item count."));
					}
				}
			}
		}
	}
}

void UTitanCheatManager::TitanAddQuestStatus(FGameplayTag QuestTag)
{
	// get the player controller
	if (APlayerController* PC = GetOuterAPlayerController())
	{
		// get the player pawn
		if (ATitanGameplayCharacter* Pawn = Cast<ATitanGameplayCharacter>(PC->GetPawn()))
		{
			// get pawn's quest list
			if (UQuestManagerComponent* Quests = Cast<UQuestManagerComponent>(Pawn->GetQuestManagerComponent()))
			{
				// attempts to add quest status tag
				if (Quests->AddQuestStatusTag(QuestTag))
				{
					GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Quest command completed."));
				}
			}
		}
	}
}

void UTitanCheatManager::TitanRemoveQuestStatus(FGameplayTag QuestTag)
{
	// get the player controller
	if (APlayerController* PC = GetOuterAPlayerController())
	{
		// get the player pawn
		if (ATitanGameplayCharacter* Pawn = Cast<ATitanGameplayCharacter>(PC->GetPawn()))
		{
			// get pawn's quest list
			if (UQuestManagerComponent* Quests = Cast<UQuestManagerComponent>(Pawn->GetQuestManagerComponent()))
			{
				// attempts to remove quest status tag
				if (Quests->RemoveQuestStatusTag(QuestTag))
				{
					GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Quest command completed."));
				}
			}
		}
	}
}

void UTitanCheatManager::TitanSendQuestEvent(FGameplayTag QuestEventTag)
{
	// get the player controller
	if (APlayerController* PC = GetOuterAPlayerController())
	{
		// get the player pawn
		if (ATitanGameplayCharacter* Pawn = Cast<ATitanGameplayCharacter>(PC->GetPawn()))
		{
			// get pawn's quest list
			if (UQuestManagerComponent* Quests = Cast<UQuestManagerComponent>(Pawn->GetQuestManagerComponent()))
			{
				FQuestEventPayload Payload;

				// sends the quest event
				Quests->SendQuestEvent(QuestEventTag, Payload);
			}
		}
	}
}

void UTitanCheatManager::TitanAddQuestQuantity(FGameplayTag QuestQuantityTag, int32 Quantity)
{
	// get the player controller
	if (APlayerController* PC = GetOuterAPlayerController())
	{
		// get the player pawn
		if (ATitanGameplayCharacter* Pawn = Cast<ATitanGameplayCharacter>(PC->GetPawn()))
		{
			// get pawn's quest list
			if (UQuestManagerComponent* Quests = Cast<UQuestManagerComponent>(Pawn->GetQuestManagerComponent()))
			{
				// adds the specified quantity to a quest's quantity tag
				if (Quests->AddQuestQuantity(QuestQuantityTag, Quantity))
				{
					GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Quest quantity succeeded."));
				}
			}
		}
	}
}

