// Copyright Epic Games, Inc. All Rights Reserved.


#include "QuestFunctionLibrary.h"
#include "QuestSubsystem.h"
#include "QuestDefinition.h"
#include "QuestLogging.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

UQuestDefinition* UQuestFunctionLibrary::GetQuestDefinition(const UObject* WorldContextObject, FGameplayTag QuestTag)
{
	// get the Quest subsystem from the world
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (UQuestSubsystem* QuestSubsystem = World->GetSubsystem<UQuestSubsystem>())
		{
			// get the quest definition from the subsystem
			return QuestSubsystem->GetQuestDefinition(QuestTag);
		}
		else
		{
			UE_LOG(LogQuest, Error, TEXT("UQuestFunctionLibrary: Couldn't resolve the Quest World Subsystem."));
		}
	}
	else
	{
		UE_LOG(LogQuest, Error, TEXT("UQuestFunctionLibrary: Couldn't resolve the World from the WorldContextObject."));
	}

	return nullptr;
}
