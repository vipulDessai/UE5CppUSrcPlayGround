// Copyright Epic Games, Inc. All Rights Reserved.

#include "UdmyOSubSysCppUSrcCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Online.h" 
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "CreateSessionCallbackProxyAdvanced.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AUdmyOSubSysCppUSrcCharacter

AUdmyOSubSysCppUSrcCharacter::AUdmyOSubSysCppUSrcCharacter(const FObjectInitializer& ObjectInitializer)
	//CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete))
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	// Set a high replication frequency for simulated proxies
	if (!IsLocallyControlled())
	{
		NetUpdateFrequency = 100.0f;
	}
	else
	{
		NetUpdateFrequency = 30.0f;
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AUdmyOSubSysCppUSrcCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AUdmyOSubSysCppUSrcCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AUdmyOSubSysCppUSrcCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AUdmyOSubSysCppUSrcCharacter::Look);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AUdmyOSubSysCppUSrcCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AUdmyOSubSysCppUSrcCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

// START - raw online subsystem create sesssion 
//void AUdmyOSubSysCppUSrcCharacter::CreateGameSession()
//{
//	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
//	if (OnlineSubsystem)
//	{
//		IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
//		if (SessionInterface.IsValid())
//		{
//			UE_LOG(LogTemp, Warning, TEXT("Found subsystem %s"), *OnlineSubsystem->GetSubsystemName().ToString());
//
//			auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
//
//			if (ExistingSession) {
//				SessionInterface->DestroySession(NAME_GameSession);
//			}
//
//			TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());
//			SessionSettings->NumPublicConnections = 4;
//			SessionSettings->bShouldAdvertise = true;
//			SessionSettings->bUseLobbiesIfAvailable = true;
//
//			const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
//			if (LocalPlayer)
//			{
//				FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &AUdmyOSubSysCppUSrcCharacter::OnCreateSessionComplete);
//				SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);
//
//				const FName SessionName = NAME_GameSession;
//				bool bWasSuccessful = SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), SessionName, *SessionSettings);
//
//				if (!bWasSuccessful)
//				{
//					UE_LOG(LogTemp, Warning, TEXT("CreateSession returned false!"));
//				}
//			}
//		}
//	}
//}
//
//void AUdmyOSubSysCppUSrcCharacter::OnCreateSessionComplete(FName SessionName, bool bWasSuccess)
//{
//	if (bWasSuccess)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Session created (callback)!"));
//		GetWorld()->ServerTravel("ThirdPersonMap?listen");
//	}
//	else
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Session creation failed (callback)!"));
//	}
//}
//
//void AUdmyOSubSysCppUSrcCharacter::CreateAdvGameSession()
//{
//	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
//	OnCreateSessionCompleteDelegate.BindUObject(this, &AUdmyOSubSysCppUSrcCharacter::OnCreateSessionComplete);
//
//	TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());
//	SessionSettings->NumPublicConnections = 4;
//	SessionSettings->bShouldAdvertise = true;
//	SessionSettings->bUseLobbiesIfAvailable = true;
//
//	// TODO: try to initialize the advanced sessions plugin 
//	//UCreateSessionCallbackProxyAdvanced::CreateAdvancedSession(
//	//	NAME_GameSession,
//	//	SessionSettings, // Make sure SessionSettings is configured
//	//	LocalPlayer,
//	//	4, // Public connections
//	//	true, // Use LAN
//	//	false, // Not a presence session
//	//	true, // Allow invites
//	//	true, // Allow join via presence
//	//	false, // Don't restrict presence joins to friends
//	//	OnCreateSessionCompleteDelegate
//	//);
//}
// END - raw online subsystem create sesssion