// Copyright Epic Games, Inc. All Rights Reserved.
#include "PISJsonDataSubsystem.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "HAL/FileManager.h"

void UPISJsonDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	ReloadAndValidate();
}

bool UPISJsonDataSubsystem::HasId(const FString& Id) const
{
	return Records.Contains(Id);
}

bool UPISJsonDataSubsystem::LoadFile(const FString& Filename, TArray<FString>& Errors)
{
	FString Text;
	if (!FFileHelper::LoadFileToString(Text, *Filename))
	{
		Errors.Add(Filename + TEXT(": cannot read"));
		return false;
	}

	TSharedPtr<FJsonObject> Root;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Text);
	if (!FJsonSerializer::Deserialize(Reader, Root) || !Root.IsValid())
	{
		Errors.Add(Filename + TEXT(": invalid JSON"));
		return false;
	}

	// Documentation-only fixtures (e.g. savegame schema samples) are skipped at runtime.
	bool bDocumentationOnly = false;
	if (Root->TryGetBoolField(TEXT("documentation_only"), bDocumentationOnly) && bDocumentationOnly)
	{
		return true;
	}

	const TArray<TSharedPtr<FJsonValue>>* Entries = nullptr;
	if (!Root->TryGetArrayField(TEXT("records"), Entries) || Entries == nullptr)
	{
		Errors.Add(Filename + TEXT(": missing records array"));
		return false;
	}

	for (const TSharedPtr<FJsonValue>& Value : *Entries)
	{
		if (!Value.IsValid())
		{
			Errors.Add(Filename + TEXT(": null record entry"));
			continue;
		}

		const TSharedPtr<FJsonObject> Obj = Value->AsObject();
		FString Id;
		if (!Obj.IsValid() || !Obj->TryGetStringField(TEXT("id"), Id) || Id.IsEmpty())
		{
			Errors.Add(Filename + TEXT(": record without id"));
			continue;
		}

		if (Records.Contains(Id))
		{
			Errors.Add(Filename + TEXT(": duplicate id ") + Id);
		}
		else
		{
			Records.Add(Id, Obj);
		}
	}

	return true;
}

bool UPISJsonDataSubsystem::ReloadAndValidate()
{
	Records.Empty();
	TArray<FString> Errors;

	const FString Dir = FPaths::ProjectContentDir() / TEXT("Data/Json");
	TArray<FString> Files;
	IFileManager::Get().FindFilesRecursive(Files, *Dir, TEXT("*.json"), true, false);

	for (const FString& File : Files)
	{
		LoadFile(File, Errors);
	}

	// Cross-file references are expressed as "*_id" string fields (and nested objects).
	for (const TPair<FString, TSharedPtr<FJsonObject>>& Pair : Records)
	{
		if (!Pair.Value.IsValid())
		{
			continue;
		}

		for (const TPair<FString, TSharedPtr<FJsonValue>>& Field : Pair.Value->Values)
		{
			if (!Field.Key.EndsWith(TEXT("_id")) || !Field.Value.IsValid())
			{
				continue;
			}

			FString Ref;
			if (Field.Value->TryGetString(Ref) && !Ref.IsEmpty() && !Records.Contains(Ref))
			{
				Errors.Add(Pair.Key + TEXT(": missing reference ") + Field.Key + TEXT("=") + Ref);
			}
		}
	}

	if (Errors.Num() > 0)
	{
		ValidationReport = FString::Join(Errors, TEXT("\n"));
		for (const FString& Error : Errors)
		{
			UE_LOG(LogTemp, Error, TEXT("JSON data: %s"), *Error);
		}
		return false;
	}

	ValidationReport = FString::Printf(TEXT("OK: %d records"), Records.Num());
	UE_LOG(LogTemp, Log, TEXT("PIS JSON data: %s"), *ValidationReport);
	return true;
}
