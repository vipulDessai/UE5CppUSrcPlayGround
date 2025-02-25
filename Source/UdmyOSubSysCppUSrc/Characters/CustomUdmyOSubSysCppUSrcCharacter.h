// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../UdmyOSubSysCppUSrcCharacter.h"
#include "MyUdmyOSubSysCppUSrcCharacter.generated.h"

// forward declare a class, instead of having them as header files which slows down compile time
class UCustomCharacterMovementComponent;
/**
 * 
 */
UCLASS()
class UDMYOSUBSYSCPPUSRC_API ACustomUdmyOSubSysCppUSrcCharacter : public AUdmyOSubSysCppUSrcCharacter
{
	GENERATED_BODY()
	
public:
	ACustomUdmyOSubSysCppUSrcCharacter(const FObjectInitializer& ObjectInitializer);

	/**
	* Returns CustomCharacterMovementComponent subobject
	*/
	UFUNCTION(BlueprintCallable)
	UCustomCharacterMovementComponent* GetCustomCharacterMovement() const;
};
