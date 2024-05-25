// Blaster project. All rights reserved.


#include "HUD/OverheadWidget.h"

#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"

void UOverheadWidget::SetDisplayText(const FString& TextToDisplay) const
{
	if (!DisplayText) return;

	DisplayText->SetText(FText::FromString(TextToDisplay));
}

void UOverheadWidget::ShowPlayerNetRole(const APawn* InPawn) const
{
	const ENetRole RemoteRole = InPawn->GetRemoteRole();
	FString Role;

	switch (RemoteRole)
	{
	case ROLE_Authority:
		Role = FString("Authority");
		break;

	case ROLE_AutonomousProxy:
		Role = FString("Autonomous Proxy");
		break;

	case ROLE_SimulatedProxy:
		Role = FString("Simulated Proxy");
		break;

	default:
		Role = FString("None");
		break;
	}

	const FString RemoteRoleString = FString::Printf(TEXT("Remote Role: %s"), *Role);
	SetDisplayText(RemoteRoleString);
}

void UOverheadWidget::ShowPlayerName(const APlayerState* PlayerState) const
{
	if (PlayerState) SetDisplayText(PlayerState->GetPlayerName());
}

void UOverheadWidget::NativeDestruct()
{
	RemoveFromParent();

	Super::NativeDestruct();
}
