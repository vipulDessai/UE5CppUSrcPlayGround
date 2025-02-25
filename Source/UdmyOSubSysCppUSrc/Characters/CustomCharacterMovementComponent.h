// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CustomCharacterMovementComponent.generated.h"

class FCustomCharacterNetworkMoveData : public FCharacterNetworkMoveData
{

public:

	typedef FCharacterNetworkMoveData Super;

	virtual bool Serialize(UCharacterMovementComponent& CharacterMovement, FArchive& Ar, UPackageMap* PackageMap, ENetworkMoveType MoveType) override;

	virtual void ClientFillNetworkMoveData(const FSavedMove_Character& ClientMove, ENetworkMoveType MoveType) override;
};

class FCustomSavedMove_Character : public FSavedMove_Character
{
public:

	typedef FSavedMove_Character Super;


	/** Returns a byte containing encoded special movement information (jumping, crouching, etc.)	 */
	virtual uint8 GetCompressedFlags() const override;

	/** Returns true if this move can be combined with NewMove for replication without changing any behaviour.
	* Just determines if any variables were modified between moves for optimisation purposes. If nothing changed, combine moves to save time.
	*/
	virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const override;

	/** Called to set up this saved move (when initially created) to make a predictive correction. */
	virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData) override;
	/** Called before ClientUpdatePosition uses this SavedMove to make a predictive correction	 */
	virtual void PrepMoveFor(class ACharacter* Character) override;

	/** Clear saved move properties, so it can be re-used. */
	virtual void Clear() override;
};

class FCustomCharacterNetworkMoveDataContainer : public FCharacterNetworkMoveDataContainer
{

public:

	FCustomCharacterNetworkMoveDataContainer();

	FCustomCharacterNetworkMoveData CustomDefaultMoveData[3];
};

//Class Prediction Data
class FCustomNetworkPredictionData_Client_Character : public FNetworkPredictionData_Client_Character
{
public:
	FCustomNetworkPredictionData_Client_Character(const UCharacterMovementComponent& ClientMovement);

	typedef FNetworkPredictionData_Client_Character Super;

	//Allocates a new copy of our custom saved move
	virtual FSavedMovePtr AllocateNewMove() override;
};

//formward declare or include the CustomUdmyOSubSysCppUSrcCharacter.h
class ACustomUdmyOSubSysCppUSrcCharacter;

/**
 * 
 */
UCLASS()
class UDMYOSUBSYSCPPUSRC_API UCustomCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	UCustomCharacterMovementComponent(const FObjectInitializer& ObjectInitializer);

	//Reference to the network predidtion buddy, the custom saved move class created above.
	friend class FCustomSavedMove;

protected:

	/** Character movement component belongs to */
	UPROPERTY(Transient, DuplicateTransient)
	TObjectPtr<ACustomUdmyOSubSysCppUSrcCharacter> CustomCharacter;

public:

	//BEGIN UActorComponent Interface
	virtual void BeginPlay() override;
	//END UActorComponent Interface

public:
	//New move data container
	FCustomCharacterNetworkMoveDataContainer MoveDataContainer;

	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	virtual void MoveAutonomous(float ClientTimeStamp, float DeltaTime, uint8 CompressedFlags, const FVector& NewAccel) override;

	virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;
};
