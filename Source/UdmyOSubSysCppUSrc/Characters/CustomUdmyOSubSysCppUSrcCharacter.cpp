// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomCharacterMovementComponent.h"
#include "CustomUdmyOSubSysCppUSrcCharacter.h"

ACustomUdmyOSubSysCppUSrcCharacter::ACustomUdmyOSubSysCppUSrcCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCustomCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{

}

UCustomCharacterMovementComponent* ACustomUdmyOSubSysCppUSrcCharacter::GetCustomCharacterMovement() const
{
	return GetCharacterMovement<UCustomCharacterMovementComponent>();
}
