// Blaster project. All rights reserved.


#include "Character/BlasterCharacter.h"

#include "BlasterComponents/CombatComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/Weapon.h"

ABlasterCharacter::ABlasterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = false;

	if (const auto MovementComp = Cast<UCharacterMovementComponent>(ACharacter::GetMovementComponent()))
	{
		MovementComp->bOrientRotationToMovement = true;
		MovementComp->NavAgentProps.bCanCrouch = true;
		MovementComp->bCanWalkOffLedges = true;
	}

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->TargetArmLength = 600.0f;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	CombatComponent->SetIsReplicated(true);

	if (const auto CapsuleComp = GetCapsuleComponent())
		CapsuleComp->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	if (const auto MeshComp = GetMesh())
		MeshComp->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	TurningPlace = ETurningPlace::ETIP_NotTurning;
}

void ABlasterCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	AimOffset(DeltaSeconds);
}

void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Basic move
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ABlasterCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ABlasterCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ABlasterCharacter::LookUp);
	PlayerInputComponent->BindAxis(TEXT("LookAround"), this, &ABlasterCharacter::LookAround);

	// Special move
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ThisClass::Jump);
	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Pressed, this, &ThisClass::CrouchButtonPressed);

	// Special
	PlayerInputComponent->BindAction(TEXT("Equip"), IE_Pressed, this, &ThisClass::EquipButtonPressed);

	// Combat
	PlayerInputComponent->BindAction(TEXT("Aim"), IE_Pressed, this, &ThisClass::AimButtonPressed);
	PlayerInputComponent->BindAction(TEXT("Aim"), IE_Released, this, &ThisClass::AimButtonReleased);
}

void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappingWeapon, COND_OwnerOnly);
}

void ABlasterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (CombatComponent) CombatComponent->Character = this;
}

void ABlasterCharacter::MoveForward(const float Value)
{
	if (!Controller || FMath::IsNearlyZero(Value)) return;;
	const FVector Direction = FRotationMatrix(FRotator(0.0f, Controller->GetControlRotation().Yaw, 0.0f)).
		GetUnitAxis(EAxis::X);

	AddMovementInput(Direction, Value);
}

void ABlasterCharacter::MoveRight(const float Value)
{
	if (!Controller || FMath::IsNearlyZero(Value)) return;;
	const FVector Direction = FRotationMatrix(FRotator(0.0f, Controller->GetControlRotation().Yaw, 0.0f)).
		GetUnitAxis(EAxis::Y);

	AddMovementInput(Direction, Value);
}

void ABlasterCharacter::LookUp(const float Value)
{
	AddControllerPitchInput(Value);
}

void ABlasterCharacter::LookAround(const float Value)
{
	AddControllerYawInput(Value);
}

void ABlasterCharacter::EquipButtonPressed()
{
	if (!CombatComponent) return;

	if (HasAuthority()) CombatComponent->EquipWeapon(OverlappingWeapon);
	else ServerEquipButtonPressed();
}

void ABlasterCharacter::ServerEquipButtonPressed_Implementation()
{
	if (!CombatComponent) return;

	CombatComponent->EquipWeapon(OverlappingWeapon);
}

void ABlasterCharacter::CrouchButtonPressed()
{
	if (bIsCrouched) UnCrouch();
	else Crouch();
}

void ABlasterCharacter::AimButtonPressed()
{
	if (CombatComponent) CombatComponent->SetAiming(true);
}

void ABlasterCharacter::AimButtonReleased()
{
	if (CombatComponent) CombatComponent->SetAiming(false);
}

void ABlasterCharacter::AimOffset(const float DeltaTime)
{
	if (!CombatComponent || !CombatComponent->EquippedWeapon) return;

	FVector Velocity = GetVelocity();
	Velocity.Z = 0.0f;
	const float Speed = Velocity.Size();

	bool IsInAir = false;
	if (const auto MovementComp = GetCharacterMovement()) IsInAir = MovementComp->IsFalling();

	if (FMath::IsNearlyZero(Speed) && !IsInAir) // Standing still, not jumping
	{
		const FRotator CurrentAimRotation = FRotator(0.0f, GetBaseAimRotation().Yaw, 0.0f);
		const FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(
			CurrentAimRotation, StartingAimRotation);

		AOYaw = DeltaAimRotation.Yaw;
		bUseControllerRotationYaw = false;
		TurnInPlace(DeltaTime);
	}
	else // Running or jumping
	{
		StartingAimRotation = FRotator(0.0f, GetBaseAimRotation().Yaw, 0.0f);
		AOYaw = 0.0f;
		bUseControllerRotationYaw = true;
		TurningPlace = ETurningPlace::ETIP_NotTurning;
	}

	AOPitch = GetBaseAimRotation().Pitch;

	// Map pitch from [270, 360) to [-90, 0]
	if (AOPitch > 90.0f && !IsLocallyControlled())
		AOPitch = FMath::GetMappedRangeValueClamped(FVector2D(270.0f, 360.0f), FVector2D(-90.0f, 0.0f), AOPitch);
}

void ABlasterCharacter::TurnInPlace(const float DeltaTime)
{
	if (AOYaw > 90.0f) TurningPlace = ETurningPlace::ETIP_Right;
	else
	{
		if (AOYaw < -90.0f) TurningPlace = ETurningPlace::ETIP_Left;
	}
}

void ABlasterCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if (IsLocallyControlled())
	{
		if (OverlappingWeapon) OverlappingWeapon->ShowPickupWidget(false);
	}

	OverlappingWeapon = Weapon;

	if (IsLocallyControlled())
	{
		if (OverlappingWeapon) OverlappingWeapon->ShowPickupWidget(true);
	}
}

void ABlasterCharacter::OnRep_OverlappingWeapon(const AWeapon* LastWeapon) const
{
	if (LastWeapon) LastWeapon->ShowPickupWidget(false);
	if (OverlappingWeapon) OverlappingWeapon->ShowPickupWidget(true);
}
