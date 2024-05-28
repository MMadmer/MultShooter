// Blaster project. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"


class AWeapon;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BLASTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatComponent();
	friend class ABlasterCharacter;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void EquipWeapon(AWeapon* WeaponToEquip);

protected:
	virtual void BeginPlay() override;
	void SetAiming(const bool IsAiming);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(const bool IsAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon() const;

	UPROPERTY(Replicated)
	uint8 bAiming : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BaseWalkSpeed = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AimWalkSpeed = 450.0f;

private:
	UPROPERTY()
	ABlasterCharacter* Character;

	UPROPERTY(ReplicatedUsing=OnRep_EquippedWeapon)
	AWeapon* EquippedWeapon;
};
