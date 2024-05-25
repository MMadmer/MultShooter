// Blaster project. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OverheadWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class BLASTER_API UOverheadWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	UTextBlock* DisplayText;

	UFUNCTION(BlueprintCallable)
	void ShowPlayerNetRole(const APawn* InPawn) const;

	UFUNCTION(BlueprintCallable)
	void ShowPlayerName(const APlayerState* PlayerState) const;

	void SetDisplayText(const FString& TextToDisplay) const;

protected:
	virtual void NativeDestruct() override;
};
