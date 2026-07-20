// Copyright Epic Games, Inc. All Rights Reserved.
#include "PISJsonDataSubsystem.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"

namespace
{
    // Returns true if the JSON file should be loaded as a record source.
    bool IsLoadableDataFile(const FString& Filename)
    {
        // savegame.json is a documentation-only schema sample; skip it at runtime.
        return !Filename.EndsWith(TEXT("savegame.json"), ESearchCase::IgnoreCase);
    }
}

void UPISJsonDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    ReloadAndValidate();
}

bool UPISJsonDataSubsystem::HasId(const FString& Id) const
{
    return Records.Contains(Id);
}

bool UPISJsonDataSubsystem::ReloadAndValidate()
{
    Records.Reset();
    RecordsView.Reset();
    TArray<FString> Errors;

    const FString DataDir = FPaths::Combine(FPaths::ProjectContentDir(), TEXT("Data"), TEXT("Json"));
    TArray<FString> Files;
    IFileManager::Get().FindFiles(Files, *(DataDir / TEXT("*.json")), true, false);
    for (const FString& File : Files)
    {
        const FString FullPath = FPaths::Combine(DataDir, File);
        if (!IsLoadableDataFile(FullPath)) { continue; }
        LoadFile(FullPath, Errors);
    }
    ValidationReport = FString::Printf(TEXT("Loaded %d records. %d error(s).\n%s"),
        Records.Num(), Errors.Num(), *FString::Join(Errors, TEXT("\n")));
    UE_LOG(LogTemp, Display, TEXT("[PISData] %s"), *ValidationReport);
    return Errors.Num() == 0;
}

bool UPISJsonDataSubsystem::LoadFile(const FString& Filename, TArray<FString>& Errors)
{
    FString Text;
    if (!FFileHelper::LoadFileToString(Text, *Filename))
    {
        Errors.Add(FString::Printf(TEXT("Could not read %s"), *Filename));
        return false;
    }
    TSharedPtr<FJsonObject> Root;
    auto Reader = TJsonReaderFactory<>::Create(Text);
    if (!FJsonSerializer::Deserialize(Reader, Root) || !Root.IsValid())
    {
        Errors.Add(FString::Printf(TEXT("%s: invalid JSON"), *Filename));
        return false;
    }
    int32 SchemaVersion = 0;
    if (!Root->TryGetNumberField(TEXT("schema_version"), SchemaVersion) || SchemaVersion != 1)
    {
        Errors.Add(FString::Printf(TEXT("%s: schema_version!=1"), *Filename));
        return false;
    }
    const TArray<TSharedPtr<FJsonValue>>* RecordsArr = nullptr;
    if (!Root->TryGetArrayField(TEXT("records"), RecordsArr) || !RecordsArr)
    {
        Errors.Add(FString::Printf(TEXT("%s: missing records[]"), *Filename));
        return false;
    }
    int32 Added = 0;
    for (const TSharedPtr<FJsonValue>& V : *RecordsArr)
    {
        const TSharedPtr<FJsonObject> O = V.IsValid() ? V->AsObject() : nullptr;
        if (!O.IsValid()) { continue; }
        FString Id;
        if (!O->TryGetStringField(TEXT("id"), Id) || Id.IsEmpty())
        {
            Errors.Add(FString::Printf(TEXT("%s: record without id"), *Filename));
            continue;
        }
        if (Records.Contains(Id))
        {
            Errors.Add(FString::Printf(TEXT("%s: duplicate id %s"), *Filename, *Id));
            continue;
        }
        Records.Add(Id, O);
        RecordsView.Add(MakeShared<FJsonValueObject>(O));
        ++Added;
    }
    return true;
}
