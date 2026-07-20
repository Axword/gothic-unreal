// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PISPauseWidget.generated.h"

class UVerticalBox;
class UTextBlock;
class UButton;
class UBorder;
class UEditableTextBox;
class USlider;
class UCheckBox;

UCLASS()
class POPIOLISOL_API UPISPauseWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    virtual TSharedRef<SWidget> RebuildWidget() override;

    UFUNCTION(BlueprintCallable, Category = "Pause") void Save();
    UFUNCTION(BlueprintCallable, Category = "Pause") void QuickLoad();
    UFUNCTION(BlueprintCallable, Category = "Pause") void Resume();
    UFUNCTION(BlueprintCallable, Category = "Pause") void QuitToMenu();

    UFUNCTION() void OnSaveClicked();
    UFUNCTION() void OnLoadClicked();
    UFUNCTION() void OnResumeClicked();
    UFUNCTION() void OnQuitClicked();

    UFUNCTION() void OnSlotTextChanged(const FText& T);
    UFUNCTION() void OnMasterChanged(float V);
    UFUNCTION() void OnMusicChanged(float V);
    UFUNCTION() void OnSfxChanged(float V);
    UFUNCTION() void OnInvertYChanged(bool V);

protected:
    UPROPERTY(Transient) TObjectPtr<UEditableTextBox> SlotInput;
    UPROPERTY(Transient) TObjectPtr<USlider> MasterSlider;
    UPROPERTY(Transient) TObjectPtr<USlider> MusicSlider;
    UPROPERTY(Transient) TObjectPtr<USlider> SfxSlider;
    UPROPERTY(Transient) TObjectPtr<UCheckBox> InvertY;
    UPROPERTY(Transient) TObjectPtr<UTextBlock> StatusText;
};
