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
 bool bDocumentationOnly=false; Root->TryGetBoolField(TEXT("documentation_only"),bDocumentationOnly); if(bDocumentationOnly) return true;
 const TArray<TSharedPtr<FJsonValue>>* Entries; if(!Root->TryGetArrayField(TEXT("records"),Entries)){Errors.Add(Filename+TEXT(": missing records array"));return false;}
 for(const auto& Value:*Entries){auto Obj=Value->AsObject(); FString Id; if(!Obj.IsValid()||!Obj->TryGetStringField(TEXT("id"),Id)||Id.IsEmpty()){Errors.Add(Filename+TEXT(": record without id"));continue;} if(Records.Contains(Id)) Errors.Add(Filename+TEXT(": duplicate id ")+Id); else Records.Add(Id,Obj);}
 return true;
}
bool UPISJsonDataSubsystem::ReloadAndValidate(){ Records.Empty(); TArray<FString> Errors; FString Dir=FPaths::ProjectContentDir()/TEXT("Data/Json"); TArray<FString> Files; IFileManager::Get().FindFilesRecursive(Files,*Dir,TEXT("*.json"),true,false); for(const FString& F:Files) LoadFile(F,Errors);
 // Cross-file references are expressed as "*_id" string fields.
 for(const auto& Pair:Records) for(const auto& Field:Pair.Value->Values) if(Field.Key.EndsWith(TEXT("_id"))){FString Ref; if(Field.Value->TryGetString(Ref)&&!Ref.IsEmpty()&&!Records.Contains(Ref)) Errors.Add(Pair.Key+TEXT(": missing reference ")+Field.Key+TEXT("=")+Ref);}
 ValidationReport=Errors.Num()?FString::Join(Errors,TEXT("\n")):FString::Printf(TEXT("OK: %d records"),Records.Num()); for(const FString&E:Errors) UE_LOG(LogTemp,Error,TEXT("JSON data: %s"),*E); return Errors.Num()==0;
}
