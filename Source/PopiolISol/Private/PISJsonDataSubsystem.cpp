#include "PISJsonDataSubsystem.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonSerializer.h"
#include "HAL/FileManager.h"
void UPISJsonDataSubsystem::Initialize(FSubsystemCollectionBase& Collection){ Super::Initialize(Collection); ReloadAndValidate(); }
bool UPISJsonDataSubsystem::HasId(const FString& Id) const { return Records.Contains(Id); }
bool UPISJsonDataSubsystem::LoadFile(const FString& Filename,TArray<FString>& Errors){
 FString Text; if(!FFileHelper::LoadFileToString(Text,*Filename)){Errors.Add(Filename+TEXT(": cannot read"));return false;}
 TSharedPtr<FJsonObject> Root; auto Reader=TJsonReaderFactory<>::Create(Text);
 if(!FJsonSerializer::Deserialize(Reader,Root)||!Root.IsValid()){Errors.Add(Filename+TEXT(": invalid JSON"));return false;}
 const TArray<TSharedPtr<FJsonValue>>* Entries; if(!Root->TryGetArrayField(TEXT("records"),Entries)){Errors.Add(Filename+TEXT(": missing records array"));return false;}
 for(const auto& Value:*Entries){auto Obj=Value->AsObject(); FString Id; if(!Obj.IsValid()||!Obj->TryGetStringField(TEXT("id"),Id)||Id.IsEmpty()){Errors.Add(Filename+TEXT(": record without id"));continue;} if(Records.Contains(Id)) Errors.Add(Filename+TEXT(": duplicate id ")+Id); else Records.Add(Id,Obj);}
 return true;
}
static void CheckJsonValueForReferences(const FString& RecordId, const FString& KeyName, const TSharedPtr<FJsonValue>& Val, const TMap<FString,TSharedPtr<FJsonObject>>& Records, TArray<FString>& Errors)
{
 if (!Val.IsValid()) return;
 if (Val->Type == EJson::Object)
 {
  TSharedPtr<FJsonObject> Obj = Val->AsObject();
  if (!Obj.IsValid()) return;
  for (const auto& Field : Obj->Values)
  {
   CheckJsonValueForReferences(RecordId, Field.Key, Field.Value, Records, Errors);
  }
 }
 else if (Val->Type == EJson::Array)
 {
  const TArray<TSharedPtr<FJsonValue>>& Arr = Val->AsArray();
  for (const auto& Elem : Arr)
  {
   CheckJsonValueForReferences(RecordId, KeyName, Elem, Records, Errors);
  }
 }
 else if (Val->Type == EJson::String)
 {
  FString StrVal = Val->AsString();
  if (StrVal.IsEmpty()) return;
  if (KeyName.EndsWith(TEXT("_id")) || KeyName == TEXT("item") || KeyName == TEXT("speaker_id") || KeyName == TEXT("target_id") || KeyName == TEXT("schedule_id") || KeyName == TEXT("loot_table_id") || KeyName == TEXT("trophy_id"))
  {
   if (!Records.Contains(StrVal))
   {
    Errors.Add(FString::Printf(TEXT("%s: missing reference %s=%s"), *RecordId, *KeyName, *StrVal));
   }
  }
  else if (KeyName == TEXT("action") && StrVal.StartsWith(TEXT("start:")))
  {
   FString TargetQuest = StrVal.RightChop(6);
   if (!Records.Contains(TargetQuest))
   {
    Errors.Add(FString::Printf(TEXT("%s: missing quest reference in action %s"), *RecordId, *StrVal));
   }
  }
 }
}

bool UPISJsonDataSubsystem::ReloadAndValidate(){ Records.Empty(); TArray<FString> Errors; FString Dir=FPaths::ProjectContentDir()/TEXT("Data/Json"); TArray<FString> Files; IFileManager::Get().FindFilesRecursive(Files,*Dir,TEXT("*.json"),true,false); for(const FString& F:Files) LoadFile(F,Errors);
 // Check cross-file references across both top-level and nested structures.
 for(const auto& Pair:Records)
 {
  for(const auto& Field:Pair.Value->Values)
  {
   CheckJsonValueForReferences(Pair.Key, Field.Key, Field.Value, Records, Errors);
  }
 }
 ValidationReport=Errors.Num()?FString::Join(Errors,TEXT("\n")):FString::Printf(TEXT("OK: %d records"),Records.Num()); for(const FString&E:Errors) UE_LOG(LogTemp,Error,TEXT("JSON data: %s"),*E); return Errors.Num()==0;
}
