// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomCharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PhysicsVolume.h"

//Let's include our custom character
#include "CustomUdmyOSubSysCppUSrcCharacter.h"
#include "Components/CapsuleComponent.h"

//Network types required for replication (we need this for GetLifetimeReplicatedProps)
#include "Net/UnrealNetwork.h"
#include "UObject/CoreNetTypes.h"

UCustomCharacterMovementComponent::UCustomCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);

	//Tells the system to use the new packed data system
	SetNetworkMoveDataContainer(MoveDataContainer);
}

void UCustomCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	CustomCharacter = Cast<ACustomUdmyOSubSysCppUSrcCharacter>(PawnOwner);
}

/** customized network orediction */
bool FCustomCharacterNetworkMoveData::Serialize(UCharacterMovementComponent& CharacterMovement, FArchive& Ar, UPackageMap* PackageMap, ENetworkMoveType MoveType)
{
	Super::Serialize(CharacterMovement, Ar, PackageMap, MoveType);

	return !Ar.IsError();
}

void FCustomCharacterNetworkMoveData::ClientFillNetworkMoveData(const FSavedMove_Character& ClientMove, ENetworkMoveType MoveType)
{
	Super::ClientFillNetworkMoveData(ClientMove, MoveType);

	const FCustomSavedMove_Character& CurrentSavedMove = static_cast<const FCustomSavedMove_Character&>(ClientMove);
}

bool FCustomSavedMove_Character::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const
{
	FCustomSavedMove_Character* NewMovePtr = static_cast<FCustomSavedMove_Character*>(NewMove.Get());

	return Super::CanCombineWith(NewMove, Character, MaxDelta);
}

void FCustomSavedMove_Character::SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);

	//This is where you set the saved move in case a packet is dropped containing this to minimize corrections
	UCustomCharacterMovementComponent* CharacterMovement = Cast<UCustomCharacterMovementComponent>(Character->GetCharacterMovement());
	if (CharacterMovement){

	}
}

void FCustomSavedMove_Character::PrepMoveFor(ACharacter* Character)
{
	Super::PrepMoveFor(Character);

	UCustomCharacterMovementComponent* CharacterMovementComponent = Cast<UCustomCharacterMovementComponent>(Character->GetCharacterMovement());
	if (CharacterMovementComponent)
	{
	}
}
