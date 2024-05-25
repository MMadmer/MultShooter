// Blaster project. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BlasterCharacter.generated.h"

class UCombatComponent;
class AWeapon;
class UWidgetComponent;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABlasterCharacter();

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;

	void SetOverlappingWeapon(AWeapon* Weapon);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USpringArmComponent* SpringArm;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UWidgetComponent* OverheadWidget;

	UPROPERTY(ReplicatedUsing=OnRep_OverlappingWeapon)
	AWeapon* OverlappingWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCombatComponent* CombatComponent;

	virtual void BeginPlay() override;
	void MoveForward(const float Value);
	void MoveRight(const float Value);
	void LookUp(const float Value);
	void LookAround(const float Value);
	void EquipButtonPressed();

	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

private:
	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon) const;
};
