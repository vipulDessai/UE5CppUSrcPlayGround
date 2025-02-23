// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomCharacterMovementComponent.h"
#include "CustomUdmyOSubSysCppUSrcCharacter.h"

AMyUdmyOSubSysCppUSrcCharacter::AMyUdmyOSubSysCppUSrcCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCustomCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{

}

UCustomCharacterMovementComponent* AMyUdmyOSubSysCppUSrcCharacter::GetCustomCharacterMovement() const
{
	return GetCharacterMovement<UCustomCharacterMovementComponent>();
}
