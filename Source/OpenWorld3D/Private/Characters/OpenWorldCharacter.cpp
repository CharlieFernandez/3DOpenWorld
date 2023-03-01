// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/OpenWorldCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimMontage.h"

#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Items/Weapons/Weapon.h"

AOpenWorldCharacter::AOpenWorldCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Ptr_SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	Ptr_SpringArm->SetupAttachment(GetRootComponent());
	Ptr_Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Ptr_Camera->SetupAttachment(Ptr_SpringArm);

	UCharacterMovementComponent* CharacterMovementComponent = GetCharacterMovement();

	CharacterMovementComponent -> bOrientRotationToMovement = true;
	CharacterMovementComponent -> RotationRate = FRotator(0.f, 400.f, 0.f);
}

void AOpenWorldCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator YawRotation(0.f, GetControlRotation().Yaw, 0.f);
	
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void AOpenWorldCharacter::LookAround(const FInputActionValue& Value)
{
	const FVector2D RotationValue = Value.Get<FVector2D>();

	if(Controller && !RotationValue.IsZero())
	{
		AddControllerPitchInput(RotationValue.Y);
		AddControllerYawInput(RotationValue.X);
	}
}

void AOpenWorldCharacter::EKeyPressed()
{	
	if(	AWeapon* Weapon = Cast<AWeapon>(OverlappedItem))
	{
		Weapon->EquipWeapon(GetMesh(), FName("handSocketWeapon"));
		CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	}
}

void AOpenWorldCharacter::Jump()
{
	Super::Jump();
}

void AOpenWorldCharacter::Attack()
{
	if(ActionState == EActionState::EAS_Unoccupied)
	{
		PlayActionMontage();
		ActionState = EActionState::EAS_Attacking;
	}
}

void AOpenWorldCharacter::PlayActionMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if(AnimInstance && AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage, 2.f);

		// AnimInstance->Montage_JumpToSection("Attack3", AttackMontage);
	}
}

void AOpenWorldCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = PlayerController->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			Subsystem->AddMappingContext(OpenWorldCharacterContext, 0);
		}
	}
}

void AOpenWorldCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AOpenWorldCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent -> BindAction(MovementAction, ETriggerEvent::Triggered, this, &AOpenWorldCharacter::Move);
		EnhancedInputComponent -> BindAction(LookAroundAction, ETriggerEvent::Triggered, this, &AOpenWorldCharacter::LookAround);
		EnhancedInputComponent -> BindAction(JumpAction, ETriggerEvent::Triggered, this, &AOpenWorldCharacter::Jump);
		EnhancedInputComponent -> BindAction(EKeyPressedAction, ETriggerEvent::Triggered, this, &AOpenWorldCharacter::EKeyPressed);
		EnhancedInputComponent -> BindAction(AttackPressedAction, ETriggerEvent::Triggered, this, &AOpenWorldCharacter::Attack);
	}
}

