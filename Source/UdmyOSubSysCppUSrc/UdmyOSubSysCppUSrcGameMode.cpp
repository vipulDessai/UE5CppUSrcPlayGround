// Copyright Epic Games, Inc. All Rights Reserved.

#include "UdmyOSubSysCppUSrcGameMode.h"
#include "UdmyOSubSysCppUSrcCharacter.h"
#include "UObject/ConstructorHelpers.h"

AUdmyOSubSysCppUSrcGameMode::AUdmyOSubSysCppUSrcGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
