// Copyright Epic Games, Inc. All Rights Reserved.

#include "RunewardGameMode.h"
#include "RunewardCharacter.h"
#include "UObject/ConstructorHelpers.h"

ARunewardGameMode::ARunewardGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
