// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PISHUDWidget.generated.h"

class UProgressBar;
class UTextBlock;
class UImage;
class UVerticalBox;
class UHorizontalBox;
class APISCharacter;

UCLASS()
class POPIOLISOL_API UPISHUDWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    /** Show a one-shot message for ~3 seconds. */
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ShowMessage(const FString& Text);

protected:
    virtual TSharedRef<SWidget> RebuildWidget() override;

    // Widget refs (created in RebuildWidget)
    UPROPERTY(Transient) TObjectPtr<UProgressBar> HealthBar;
    UPROPERTY(Transient) TObjectPtr<UProgressBar> ManaBar;
    UPROPERTY(Transient) TObjectPtr<UTextBlock> HealthText;
    UPROPERTY(Transient) TObjectPtr<UTextBlock> ManaText;
    UPROPERTY(Transient) TObjectPtr<UTextBlock> LevelText;
    UPROPERTY(Transient) TObjectPtr<UTextBlock> GoldText;
    UPROPERTY(Transient) TObjectPtr<UTextBlock> WeaponText;
    UPROPERTY(Transient) TObjectPtr<UTextBlock> MessageText;
    UPROPERTY(Transient) TObjectPtr<UTextBlock> LocationText;
    UPROPERTY(Transient) TObjectPtr<UTextBlock> ClockText;

    UPROPERTY(Transient) TObjectPtr<APISCharacter> CachedPlayer;

private:
    float MessageRemaining = 0.f;
    void RefreshFromPlayer();
    void BuildHudTree();
};
