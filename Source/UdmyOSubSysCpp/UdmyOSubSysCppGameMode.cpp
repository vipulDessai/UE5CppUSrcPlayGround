// Copyright Epic Games, Inc. All Rights Reserved.

#include "UdmyOSubSysCppGameMode.h"
#include "UdmyOSubSysCppCharacter.h"
#include "UObject/ConstructorHelpers.h"

AUdmyOSubSysCppGameMode::AUdmyOSubSysCppGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
