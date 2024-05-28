// Blaster project. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "TurningPlace.h"
#include "BlasterComponents/CombatComponent.h"
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

	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;

	void SetOverlappingWeapon(AWeapon* Weapon);
	bool IsWeaponEquipped() const { return CombatComponent && CombatComponent->EquippedWeapon; }
	bool IsAiming() const { return CombatComponent && CombatComponent->bAiming; }
	float GetAOYaw() const { return AOYaw; }
	float GetAOPitch() const { return AOPitch; }
	AWeapon* GetEquippedWeapon() const { return CombatComponent ? CombatComponent->EquippedWeapon : nullptr; }
	ETurningPlace GetTurningInPlace() const { return TurningPlace; }

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
	void CrouchButtonPressed();
	void AimButtonPressed();
	void AimButtonReleased();
	void AimOffset(const float DeltaTime);
	void TurnInPlace(const float DeltaTime);

	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

private:
	UFUNCTION()
	void OnRep_OverlappingWeapon(const AWeapon* LastWeapon) const;

	float AOYaw = 0.0f;
	float AOPitch = 0.0f;
	FRotator StartingAimRotation;
	ETurningPlace TurningPlace;
};
