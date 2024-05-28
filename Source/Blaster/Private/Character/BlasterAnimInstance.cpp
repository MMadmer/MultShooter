// Blaster project. All rights reserved.


#include "Character/BlasterAnimInstance.h"

#include "Character/BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapon/Weapon.h"

UBlasterAnimInstance::UBlasterAnimInstance(): BlasterCharacter(nullptr), bIsInAir(false), bIsAccelerating(false),
                                              bWeaponEquipped(false), EquippedWeapon(nullptr), bIsCrouched(false),
                                              bAiming(false)
{
}

void UBlasterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
}

void UBlasterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!BlasterCharacter) BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
	if (!BlasterCharacter) return;;

	FVector Velocity = BlasterCharacter->GetVelocity();
	Velocity.Z = 0.0f;
	Speed = Velocity.Size();

	const auto MovementComp = BlasterCharacter->GetCharacterMovement();
	if (!MovementComp) return;

	bIsInAir = MovementComp->IsFalling();
	bIsAccelerating = MovementComp->GetCurrentAcceleration().Size() > 0.0f;

	bWeaponEquipped = BlasterCharacter->IsWeaponEquipped();
	EquippedWeapon = BlasterCharacter->GetEquippedWeapon();

	bIsCrouched = BlasterCharacter->bIsCrouched;

	bAiming = BlasterCharacter->IsAiming();

	TurningPlace = BlasterCharacter->GetTurningInPlace();

	// Yaw offset
	const FRotator AimRotation = BlasterCharacter->GetBaseAimRotation();
	const FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(BlasterCharacter->GetVelocity());

	const FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaSeconds, 5.0f);
	YawOffset = DeltaRotation.Yaw;

	// Lean
	RotationLastFrame = RotationCurrentFrame;
	RotationCurrentFrame = BlasterCharacter->GetActorRotation();

	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(RotationCurrentFrame, RotationLastFrame);
	const float Target = Delta.Yaw / DeltaSeconds;
	const float Interp = FMath::FInterpTo(Lean, Target, DeltaSeconds, 6.0f);
	Lean = FMath::Clamp(Interp, -90.0f, 90.0f);

	// Aim offset
	AOYaw = BlasterCharacter->GetAOYaw();
	AOPitch = BlasterCharacter->GetAOPitch();

	// Weapon FABRIK
	if (bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && BlasterCharacter->GetMesh())
	{
		LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(
			FName(TEXT("LeftHandSocket")), RTS_World);
		FVector OutPosition{};
		FRotator OutRotation{};

		BlasterCharacter->GetMesh()->TransformToBoneSpace(FName(TEXT("hand_r")), LeftHandTransform.GetLocation(),
		                                                  FRotator::ZeroRotator, OutPosition, OutRotation);

		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));
	}
}
