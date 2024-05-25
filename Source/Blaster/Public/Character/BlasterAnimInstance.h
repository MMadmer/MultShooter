// Blaster project. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BlasterAnimInstance.generated.h"

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
};
