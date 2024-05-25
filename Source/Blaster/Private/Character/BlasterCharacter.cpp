// Blaster project. All rights reserved.


#include "Character/BlasterCharacter.h"

#include "BlasterComponents/CombatComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/Weapon.h"

ABlasterCharacter::ABlasterCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bCanEverTick = false;

	bUseControllerRotationYaw = false;

	if (const auto MovementComp = Cast<
		UCharacterMovementComponent>(ACharacter::GetMovementComponent()))
		MovementComp->bOrientRotationToMovement = true;

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

	// Special
	PlayerInputComponent->BindAction(TEXT("Equip"), IE_Pressed, this, &ThisClass::EquipButtonPressed);
}

void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappingWeapon, COND_OwnerOnly);
}

void ABlasterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (CombatComponent)
	{
		CombatComponent->Character = this;
	}
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

void ABlasterCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon) const
{
	if (LastWeapon) LastWeapon->ShowPickupWidget(false);
	if (OverlappingWeapon) OverlappingWeapon->ShowPickupWidget(true);
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
