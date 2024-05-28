// Blaster project. All rights reserved.


#include "BlasterComponents/CombatComponent.h"

#include "Character/BlasterCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/Weapon.h"

UCombatComponent::UCombatComponent(): bAiming(false), Character(nullptr), EquippedWeapon(nullptr)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (Character)
	{
		if (const auto MovementComp = Character->GetCharacterMovement()) MovementComp->MaxWalkSpeed = BaseWalkSpeed;
	}
}

void UCombatComponent::SetAiming(const bool IsAiming)
{
	// Not necessarily but for better client's experience
	bAiming = IsAiming;

	if (!Character) return;

	if (const auto MovementComp = Character->GetCharacterMovement())
		MovementComp->MaxWalkSpeed = bAiming ? AimWalkSpeed : BaseWalkSpeed;

	// Always calls from the server(if client's call to), cause it's server rpc
	ServerSetAiming(IsAiming);
}

void UCombatComponent::ServerSetAiming_Implementation(const bool IsAiming)
{
	bAiming = IsAiming;

	if (!Character) return;

	if (const auto MovementComp = Character->GetCharacterMovement())
		MovementComp->MaxWalkSpeed = bAiming ? AimWalkSpeed : BaseWalkSpeed;
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, bAiming);
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

	if (const auto MovementComp = Character->GetCharacterMovement()) MovementComp->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;
}

void UCombatComponent::OnRep_EquippedWeapon() const
{
	if (!EquippedWeapon || !Character) return;

	if (const auto MovementComp = Character->GetCharacterMovement()) MovementComp->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;
}
