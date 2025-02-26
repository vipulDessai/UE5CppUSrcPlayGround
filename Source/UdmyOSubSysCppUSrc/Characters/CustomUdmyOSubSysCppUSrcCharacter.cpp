// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomUdmyOSubSysCppUSrcCharacter.h"
#include "CustomCharacterMovementComponent.h"

ACustomUdmyOSubSysCppUSrcCharacter::ACustomUdmyOSubSysCppUSrcCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCustomCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{

}

UCustomCharacterMovementComponent* ACustomUdmyOSubSysCppUSrcCharacter::GetCustomCharacterMovement() const
{
	return GetCharacterMovement<UCustomCharacterMovementComponent>();
}
