// Blaster project. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "TurningPlace.h"
#include "Animation/AnimInstance.h"
#include "BlasterAnimInstance.generated.h"

class AWeapon;
class ABlasterCharacter;

/**
 * 
 */
UCLASS()
class BLASTER_API UBlasterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UBlasterAnimInstance();

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UPROPERTY(BlueprintReadOnly, Category="Character", meta = (AllowPrivateAccess = "true"))
	ABlasterCharacter* BlasterCharacter;

	UPROPERTY(BlueprintReadOnly, Category="Movement", meta = (AllowPrivateAccess = "true"))
	float Speed = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="Movement", meta = (AllowPrivateAccess = "true"))
	uint8 bIsInAir : 1;

	UPROPERTY(BlueprintReadOnly, Category="Movement", meta = (AllowPrivateAccess = "true"))
	uint8 bIsAccelerating : 1;

	UPROPERTY(BlueprintReadOnly, Category="Movement", meta = (AllowPrivateAccess = "true"))
	uint8 bWeaponEquipped : 1;

	UPROPERTY()
	AWeapon* EquippedWeapon;

	UPROPERTY(BlueprintReadOnly, Category="Movement", meta = (AllowPrivateAccess = "true"))
	uint32 bIsCrouched : 1;

	UPROPERTY(BlueprintReadOnly, Category="Movement", meta = (AllowPrivateAccess = "true"))
	uint32 bAiming : 1;

	UPROPERTY(BlueprintReadOnly, Category="Movement", meta = (AllowPrivateAccess = "true"))
	float YawOffset = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="Movement", meta = (AllowPrivateAccess = "true"))
	float Lean = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="Movement", meta = (AllowPrivateAccess = "true"))
	float AOYaw = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="Movement", meta = (AllowPrivateAccess = "true"))
	float AOPitch = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="Movement", meta = (AllowPrivateAccess = "true"))
	FTransform LeftHandTransform;

	UPROPERTY(BlueprintReadOnly, Category="Movement", meta = (AllowPrivateAccess = "true"))
	ETurningPlace TurningPlace;

	FRotator RotationLastFrame;
	FRotator RotationCurrentFrame;
	FRotator DeltaRotation;
};
