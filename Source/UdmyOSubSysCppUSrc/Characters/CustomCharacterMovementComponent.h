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

	//All Saved Variables are placed here.
	//Boolean Flags
	bool bWantsToSprintMoveData = false;
};

class FCustomSavedMove_Character : public FSavedMove_Character
{
public:

	typedef FSavedMove_Character Super;

	//All Saved Variables are placed here.
	//Boolean Flags
	bool bWantsToSprintSaved = false;

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

	/////BEGIN Sprinting/////

	/*
	* Sprinting is different from other movement types. We don't need to add a whole new movement mode as we are simply adjusting the max movement speed.
	* If it requires additional logic in your game, you can certainly create a sprinting movement mode, but it isn't necessary for most use cases.
	* We will still be using the PhysWalking logic in this implementation.
	* Thus, this can be seen as a movement modifier, not a new movement type.
	*/

	/*
	* We create two variables here. This one tracks player intent, such as holding the sprint button down or toggling it.
	* While the player intends to sprint, the movement can use this information to trigger different logic. We can even re-use it to act as an indicator to start wall-running, for instance.
	* In this case, if the player intends to sprint but isn't sprinting, we trigger sprinting.
	* Similarly, if the sprinting is interrupted, we would want to resume sprinting as soon as possible.
	* The implementation of this logic may differ in a GAS (Gameplay Ability System) setup where this variable will be controlled by Gameplay Abilities (GA) and Gameplay Effects (GE).
	* We do not replicate the variable as it's only really relevant to the owning client and the server.
	* The network prediction setup will ensure sync between owning client and server.
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Sprinting")
	bool bWantsToSprint;

	/*
	* This variable controls the actual sprinting logic. If it's true, the character will be moving at a higher velocity.
	* It can be used as an internal CMC variable to track a gameplay tag that is applied/removed by GAS (e.g. State.Movement.Sprinting or State.Buff.Sprinting, depending on preference and design).
	* But in this basic tutorial, we will use it directly.
	* This is replicated as we want this variable to propagate down to simulated proxies (other clients).
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Sprinting")
	bool bIsSprinting;

	/*
	* The current maximum speed that the character can run.
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sprinting")
	float CustomMaxSpeed = 1000.0f;

	/*
	* A simple function to determine if the character is able to sprint in its current state.
	* This function does not factor in external state information at the moment.
	* If used with GAS, this could be a helper function to determine if the Sprint Ability can be applied or not, alongside the use of gameplay tags and other checks.
	*/
	UFUNCTION(BlueprintCallable, Category = "Sprinting")
	virtual bool CanSprint() const;

	/////END Sprinting/////

protected:

	/** Character movement component belongs to */
	UPROPERTY(Transient, DuplicateTransient)
	TObjectPtr<ACustomUdmyOSubSysCppUSrcCharacter> CustomCharacter;

public:

	//BEGIN UActorComponent Interface
	virtual void BeginPlay() override;
	//END UActorComponent Interface

	//BEGIN UMovementComponent Interface
	virtual float GetMaxSpeed() const override;

	/** Update the character state in PerformMovement right before doing the actual position change */
	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;

	/** Update the character state in PerformMovement after the position change. Some rotation updates happen after this. */
	virtual void UpdateCharacterStateAfterMovement(float DeltaSeconds) override;

	/** Consider this to be the final chance to change logic before the next tick. It can be useful to defer certain actions to the next tick. */
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;

	/** Override jump behaviour to help us create custom logic. */
	virtual bool CanAttemptJump() const override;
	virtual bool DoJump(bool bReplayingMoves) override;
	//END UMovementComponent Interface

	//Replication. Boilerplate function that handles replicated variables. 
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	//New move data container
	FCustomCharacterNetworkMoveDataContainer MoveDataContainer;

	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	virtual void MoveAutonomous(float ClientTimeStamp, float DeltaTime, uint8 CompressedFlags, const FVector& NewAccel) override;

	virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;
};
