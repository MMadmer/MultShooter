// Blaster project. All rights reserved.


#include "BlasterComponents/CombatComponent.h"

#include "Character/BlasterCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Weapon/Weapon.h"

UCombatComponent::UCombatComponent(): Character(nullptr), EquippedWeapon(nullptr)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (!Character || !WeaponToEquip) return;

	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);

	const auto CharMesh = Character->GetMesh();
	if (!CharMesh) return;

	const auto HandSocket = CharMesh->GetSocketByName(FName(TEXT("RightHandSocket")));
	if (!HandSocket) return;

	HandSocket->AttachActor(EquippedWeapon, CharMesh);

	EquippedWeapon->SetOwner(Character);
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
}
