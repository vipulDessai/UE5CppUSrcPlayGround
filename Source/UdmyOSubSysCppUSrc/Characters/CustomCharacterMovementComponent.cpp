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

bool UCustomCharacterMovementComponent::CanSprint() const
{
	if (CustomCharacter && IsMovingOnGround() && bWantsToSprint) //Only sprint if on ground 
	{
		//Check if moving forward
		FVector Forward = CharacterOwner->GetActorForwardVector();
		FVector MoveDirection = Velocity.GetSafeNormal();

		float VelocityDot = FVector::DotProduct(Forward, MoveDirection); //Confirm we are moving forward so the player can't sprint sideways or backwards.
		return VelocityDot > 0.7f; //Slight lenience so that small changes don't rapidly toggle sprinting. This should be a variable, but it is hard-coded here for simplicity.
	}
	return false;
}

void UCustomCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	CustomCharacter = Cast<ACustomUdmyOSubSysCppUSrcCharacter>(PawnOwner);
}

float UCustomCharacterMovementComponent::GetMaxSpeed() const
{
	return bIsSprinting ? CustomMaxSpeed : Super::GetMaxSpeed();
}

void UCustomCharacterMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
	// Proxies get replicated state. We don't need to run this logic for them.
	if (CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)
	{
		//Sprinting
		if (CanSprint())
		{
			bIsSprinting = true;
		}
		else
		{
			bIsSprinting = false;
		}
	}
}

void UCustomCharacterMovementComponent::UpdateCharacterStateAfterMovement(float DeltaSeconds)
{
}

void UCustomCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
}

bool UCustomCharacterMovementComponent::CanAttemptJump() const
{
	return false;
}

bool UCustomCharacterMovementComponent::DoJump(bool bReplayingMoves)
{
	return false;
}

void UCustomCharacterMovementComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, bIsSprinting, COND_SimulatedOnly);
}

void UCustomCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);
}

void UCustomCharacterMovementComponent::MoveAutonomous(float ClientTimeStamp, float DeltaTime, uint8 CompressedFlags, const FVector& NewAccel)
{
	FCustomCharacterNetworkMoveData* CurrentMoveData = static_cast<FCustomCharacterNetworkMoveData*>(GetCurrentNetworkMoveData());
	if (CurrentMoveData != nullptr)
	{
		bWantsToSprint = CurrentMoveData->bWantsToSprintMoveData;

		//If you still wanted to use bools AND bitflags, you could unpack movement flags like this.
		//It is similar to UpdateFromCompressedFlags in this sense. Check out that function in the parent to see how it's done.
		//EXAMPLE:
		//bWantsToFly = (CurrentMoveData->MovementFlagCustomMoveData & (uint8)EMovementFlag::CFLAG_WantsToFly) != 0;
	}
	Super::MoveAutonomous(ClientTimeStamp, DeltaTime, CompressedFlags, NewAccel);
}

FNetworkPredictionData_Client* UCustomCharacterMovementComponent::GetPredictionData_Client() const
{
	check(PawnOwner != NULL);

	if (!ClientPredictionData)
	{
		UCustomCharacterMovementComponent* MutableThis = const_cast<UCustomCharacterMovementComponent*>(this);

		MutableThis->ClientPredictionData = new FCustomNetworkPredictionData_Client_Character(*this);
	}

	return ClientPredictionData;
}

/** customized network orediction */
bool FCustomCharacterNetworkMoveData::Serialize(UCharacterMovementComponent& CharacterMovement, FArchive& Ar, UPackageMap* PackageMap, ENetworkMoveType MoveType)
{
	Super::Serialize(CharacterMovement, Ar, PackageMap, MoveType);

	SerializeOptionalValue<bool>(Ar.IsSaving(), Ar, bWantsToSprintMoveData, false);

	return !Ar.IsError();
}

void FCustomCharacterNetworkMoveData::ClientFillNetworkMoveData(const FSavedMove_Character& ClientMove, ENetworkMoveType MoveType)
{
	Super::ClientFillNetworkMoveData(ClientMove, MoveType);

	const FCustomSavedMove_Character& CurrentSavedMove = static_cast<const FCustomSavedMove_Character&>(ClientMove);

	bWantsToSprintMoveData = CurrentSavedMove.bWantsToSprintSaved;
}

uint8 FCustomSavedMove_Character::GetCompressedFlags() const
{
	return Super::GetCompressedFlags();
}

bool FCustomSavedMove_Character::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const
{
	FCustomSavedMove_Character* NewMovePtr = static_cast<FCustomSavedMove_Character*>(NewMove.Get());

	if (bWantsToSprintSaved != NewMovePtr->bWantsToSprintSaved) {
		return false;
	}

	return Super::CanCombineWith(NewMove, Character, MaxDelta);
}

void FCustomSavedMove_Character::SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);

	//This is where you set the saved move in case a packet is dropped containing this to minimize corrections
	UCustomCharacterMovementComponent* CharacterMovement = Cast<UCustomCharacterMovementComponent>(Character->GetCharacterMovement());
	if (CharacterMovement){
		bWantsToSprintSaved = CharacterMovement->bWantsToSprint;
	}
}

void FCustomSavedMove_Character::PrepMoveFor(ACharacter* Character)
{
	Super::PrepMoveFor(Character);

	UCustomCharacterMovementComponent* CharacterMovementComponent = Cast<UCustomCharacterMovementComponent>(Character->GetCharacterMovement());
	if (CharacterMovementComponent)
	{
		CharacterMovementComponent->bWantsToSprint = bWantsToSprintSaved;
	}
}

void FCustomSavedMove_Character::Clear()
{
	Super::Clear();

	bWantsToSprintSaved = false;
}

FCustomNetworkPredictionData_Client_Character::FCustomNetworkPredictionData_Client_Character(const UCharacterMovementComponent& ClientMovement)
	: Super(ClientMovement)
{
	
}

FSavedMovePtr FCustomNetworkPredictionData_Client_Character::AllocateNewMove()
{
	return FSavedMovePtr(new FCustomSavedMove_Character());
}

FCustomCharacterNetworkMoveDataContainer::FCustomCharacterNetworkMoveDataContainer()
{
	NewMoveData = &CustomDefaultMoveData[0];
	PendingMoveData = &CustomDefaultMoveData[1];
	OldMoveData = &CustomDefaultMoveData[2];
}
