// Blaster project. All rights reserved.


#include "Character/BlasterAnimInstance.h"

#include "Character/BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UBlasterAnimInstance::UBlasterAnimInstance(): BlasterCharacter(nullptr), bIsInAir(false), bIsAccelerating(false)
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
}
