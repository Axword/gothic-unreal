// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PISMainMenuWidget.generated.h"

class UVerticalBox;
class UTextBlock;
class UButton;
class UBorder;
class UPISPauseWidget;

UCLASS()
class POPIOLISOL_API UPISMainMenuWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    virtual TSharedRef<SWidget> RebuildWidget() override;

    UFUNCTION() void OnNewGame();
    UFUNCTION() void OnLoadGame();
    UFUNCTION() void OnOptions();
    UFUNCTION() void OnQuit();
    UFUNCTION() void OnResumePause();

    UFUNCTION(BlueprintCallable, Category = "Menu") void ShowPauseOverlay();

protected:
    UPROPERTY(Transient) TObjectPtr<UVerticalBox> MenuBox;
    UPROPERTY(Transient) TObjectPtr<UTextBlock> StatusText;
    UPROPERTY(Transient) TObjectPtr<UPISPauseWidget> PauseOverlay;
    UPROPERTY(Transient) TObjectPtr<UButton> NewBtn;
    UPROPERTY(Transient) TObjectPtr<UButton> LoadBtn;
    UPROPERTY(Transient) TObjectPtr<UButton> OptBtn;
    UPROPERTY(Transient) TObjectPtr<UButton> QuitBtn;
};
