// Copyright Epic Games, Inc. All Rights Reserved.
#include "PISSaveGameSubsystem.h"
#include "PISStatsComponent.h"
#include "PISInventoryComponent.h"
#include "PISQuestComponent.h"
#include "GameFramework/Pawn.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFileManager.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonReader.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Engine/World.h"

namespace
{
    FString Slugify(const FString& In)
    {
        FString Out = In;
        for (TCHAR& C : Out)
        {
            if (!FChar::IsAlnum(C) && C != TEXT('_') && C != TEXT('-'))
            {
                C = TEXT('_');
            }
        }
        return Out;
    }
}

FString UPISSaveGameSubsystem::GetSaveDirectory() const
{
    return FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("SaveGames"));
}

bool UPISSaveGameSubsystem::SaveSlot(const FString& SlotName, APawn* Player)
{
    LastError.Empty();
    if (!Player) { LastError = TEXT("Brak postaci."); return false; }
    FString Json;
    FString Err;
    if (!SerializePlayerToJson(Player, Err))
    {
        LastError = Err;
        return false;
    }
    Json = Err; // populated

    const FString Dir = GetSaveDirectory();
    IPlatformFile& PF = FPlatformFileManager::Get().GetPlatformFile();
    if (!PF.DirectoryExists(*Dir)) { PF.CreateDirectoryTree(*Dir); }

    const FString Path = FPaths::Combine(Dir, FString::Printf(TEXT("pis_save_%s.json"), *Slugify(SlotName)));
    if (!FFileHelper::SaveStringToFile(Json, *Path))
    {
        LastError = FString::Printf(TEXT("Nie można zapisać pliku: %s"), *Path);
        return false;
    }
    return true;
}

FString UPISSaveGameSubsystem::SerializePlayerToJson(APawn* Player, FString& OutError)
{
    OutError.Empty();
    if (!Player) { OutError = TEXT("Brak postaci."); return FString(); }

    UPISStatsComponent* Stats = Player->FindComponentByClass<UPISStatsComponent>();
    UPISInventoryComponent* Inv = Player->FindComponentByClass<UPISInventoryComponent>();
    UPISQuestComponent* Quest = Player->FindComponentByClass<UPISQuestComponent>();

    TSharedRef<FJsonObject> Root = MakeShared<FJsonObject>();
    Root->SetNumberField(TEXT("schema_version"), SchemaVersion);
    Root->SetStringField(TEXT("saved_at"), FDateTime::Now().ToIso8601());
    Root->SetStringField(TEXT("slot_id"), TEXT("default"));

    // Player transform.
    const FVector Loc = Player->GetActorLocation();
    const FRotator Rot = Player->GetActorRotation();
    Root->SetNumberField(TEXT("pos_x"), Loc.X);
    Root->SetNumberField(TEXT("pos_y"), Loc.Y);
    Root->SetNumberField(TEXT("pos_z"), Loc.Z);
    Root->SetNumberField(TEXT("rot_pitch"), Rot.Pitch);
    Root->SetNumberField(TEXT("rot_yaw"), Rot.Yaw);
    Root->SetNumberField(TEXT("rot_roll"), Rot.Roll);

    // Stats.
    TSharedRef<FJsonObject> StatsObj = MakeShared<FJsonObject>();
    if (Stats)
    {
        StatsObj->SetNumberField(TEXT("level"), Stats->Level);
        StatsObj->SetNumberField(TEXT("xp"), Stats->Xp);
        StatsObj->SetNumberField(TEXT("learning_points"), Stats->LearningPoints);
        StatsObj->SetNumberField(TEXT("current_hp"), Stats->CurrentHp);
        StatsObj->SetNumberField(TEXT("current_mana"), Stats->CurrentMana);

        TSharedRef<FJsonObject> Prim = MakeShared<FJsonObject>();
        Prim->SetNumberField(TEXT("strength"), Stats->Primary.Strength);
        Prim->SetNumberField(TEXT("dexterity"), Stats->Primary.Dexterity);
        Prim->SetNumberField(TEXT("vitality"), Stats->Primary.Vitality);
        Prim->SetNumberField(TEXT("mana_stat"), Stats->Primary.ManaStat);
        StatsObj->SetObjectField(TEXT("primary"), Prim);

        TSharedRef<FJsonObject> Sk = MakeShared<FJsonObject>();
        Sk->SetNumberField(TEXT("sword"), Stats->Skills.Sword);
        Sk->SetNumberField(TEXT("bow"), Stats->Skills.Bow);
        Sk->SetNumberField(TEXT("lockpick"), Stats->Skills.Lockpick);
        Sk->SetNumberField(TEXT("steal"), Stats->Skills.Steal);
        Sk->SetNumberField(TEXT("skinning"), Stats->Skills.Skinning);
        Sk->SetNumberField(TEXT("fire"), Stats->Skills.FireMagic);
        Sk->SetNumberField(TEXT("ice"), Stats->Skills.IceMagic);
        StatsObj->SetObjectField(TEXT("skills"), Sk);

        TSharedRef<FJsonObject> Rep = MakeShared<FJsonObject>();
        for (const auto& Pair : Stats->Reputation) { Rep->SetNumberField(Pair.Key, Pair.Value); }
        StatsObj->SetObjectField(TEXT("reputation"), Rep);
        StatsObj->SetStringField(TEXT("faction_choice"), Stats->FactionChoice);
    }
    Root->SetObjectField(TEXT("stats"), StatsObj);

    // Inventory.
    TSharedRef<FJsonObject> InvObj = MakeShared<FJsonObject>();
    if (Inv)
    {
        TArray<TSharedPtr<FJsonValue>> Items;
        for (const FPISInventoryStack& S : Inv->Items)
        {
            TSharedRef<FJsonObject> IO = MakeShared<FJsonObject>();
            IO->SetStringField(TEXT("item_id"), S.ItemId);
            IO->SetNumberField(TEXT("count"), S.Count);
            Items.Add(MakeShared<FJsonValueObject>(IO));
        }
        InvObj->SetArrayField(TEXT("items"), Items);
        InvObj->SetStringField(TEXT("equipped_weapon"), Inv->EquippedWeaponId);
    }
    Root->SetObjectField(TEXT("inventory"), InvObj);

    // Quests.
    TArray<TSharedPtr<FJsonValue>> QuestArr;
    if (Quest)
    {
        for (const FPISQuestState& S : Quest->States)
        {
            TSharedRef<FJsonObject> QO = MakeShared<FJsonObject>();
            QO->SetStringField(TEXT("quest_id"), S.QuestId);
            QO->SetStringField(TEXT("stage_id"), S.StageId);
            QO->SetNumberField(TEXT("status"), static_cast<int32>(S.Status));
            QO->SetNumberField(TEXT("progress"), S.ProgressCount);
            QuestArr.Add(MakeShared<FJsonValueObject>(QO));
        }
    }
    Root->SetArrayField(TEXT("quests"), QuestArr);

    FString Out;
    auto Writer = TJsonWriterFactory<>::Create(&Out);
    FJsonSerializer::Serialize(Root, Writer);
    return Out;
}

bool UPISSaveGameSubsystem::LoadSlot(const FString& SlotName, APawn* Player)
{
    LastError.Empty();
    const FString Path = FPaths::Combine(GetSaveDirectory(), FString::Printf(TEXT("pis_save_%s.json"), *Slugify(SlotName)));
    FString Json;
    if (!FFileHelper::LoadFileToString(Json, *Path))
    {
        LastError = FString::Printf(TEXT("Nie znaleziono zapisu: %s"), *Path);
        return false;
    }
    FString Err;
    if (!ApplyJsonToPlayer(Player, Json, Err))
    {
        LastError = Err;
        return false;
    }
    return true;
}

bool UPISSaveGameSubsystem::ApplyJsonToPlayer(APawn* Player, const FString& Json, FString& OutError)
{
    if (!Player) { OutError = TEXT("Brak postaci."); return false; }
    TSharedPtr<FJsonObject> Root;
    auto Reader = TJsonReaderFactory<>::Create(Json);
    if (!FJsonSerializer::Deserialize(Reader, Root) || !Root.IsValid())
    {
        OutError = TEXT("Nieprawidłowy JSON zapisu.");
        return false;
    }
    int32 Version = 0;
    if (!Root->TryGetNumberField(TEXT("schema_version"), Version))
    {
        OutError = TEXT("Brak schema_version.");
        return false;
    }
    if (Version > SchemaVersion)
    {
        OutError = FString::Printf(TEXT("Zapis jest nowszy (v%d) niż obsługiwana wersja (v%d)."), Version, SchemaVersion);
        return false;
    }
    // Unknown version older than 0: still try to load but warn.

    const double X = Root->GetNumberField(TEXT("pos_x"));
    const double Y = Root->GetNumberField(TEXT("pos_y"));
    const double Z = Root->GetNumberField(TEXT("pos_z"));
    const double P = Root->GetNumberField(TEXT("rot_pitch"));
    const double Ya = Root->GetNumberField(TEXT("rot_yaw"));
    const double R = Root->GetNumberField(TEXT("rot_roll"));
    Player->SetActorLocationAndRotation(FVector(X, Y, Z), FRotator(P, Ya, R));

    UPISStatsComponent* Stats = Player->FindComponentByClass<UPISStatsComponent>();
    UPISInventoryComponent* Inv = Player->FindComponentByClass<UPISInventoryComponent>();
    UPISQuestComponent* Quest = Player->FindComponentByClass<UPISQuestComponent>();

    if (Stats)
    {
        const TSharedPtr<FJsonObject>* SObj = nullptr;
        if (Root->TryGetObjectField(TEXT("stats"), SObj) && SObj && SObj->IsValid())
        {
            Stats->Level = (*SObj)->GetIntegerField(TEXT("level"));
            Stats->Xp = (*SObj)->GetIntegerField(TEXT("xp"));
            Stats->LearningPoints = (*SObj)->GetIntegerField(TEXT("learning_points"));
            Stats->CurrentHp = (*SObj)->GetIntegerField(TEXT("current_hp"));
            Stats->CurrentMana = (*SObj)->GetIntegerField(TEXT("current_mana"));

            const TSharedPtr<FJsonObject>* PObj = nullptr;
            if ((*SObj)->TryGetObjectField(TEXT("primary"), PObj) && PObj && PObj->IsValid())
            {
                Stats->Primary.Strength  = (*PObj)->GetIntegerField(TEXT("strength"));
                Stats->Primary.Dexterity = (*PObj)->GetIntegerField(TEXT("dexterity"));
                Stats->Primary.Vitality  = (*PObj)->GetIntegerField(TEXT("vitality"));
                Stats->Primary.ManaStat  = (*PObj)->GetIntegerField(TEXT("mana_stat"));
            }
            const TSharedPtr<FJsonObject>* SkObj = nullptr;
            if ((*SObj)->TryGetObjectField(TEXT("skills"), SkObj) && SkObj && SkObj->IsValid())
            {
                Stats->Skills.Sword    = (*SkObj)->GetIntegerField(TEXT("sword"));
                Stats->Skills.Bow      = (*SkObj)->GetIntegerField(TEXT("bow"));
                Stats->Skills.Lockpick = (*SkObj)->GetIntegerField(TEXT("lockpick"));
                Stats->Skills.Steal    = (*SkObj)->GetIntegerField(TEXT("steal"));
                Stats->Skills.Skinning = (*SkObj)->GetIntegerField(TEXT("skinning"));
                Stats->Skills.FireMagic = (*SkObj)->GetIntegerField(TEXT("fire"));
                Stats->Skills.IceMagic  = (*SkObj)->GetIntegerField(TEXT("ice"));
            }
            Stats->Reputation.Reset();
            const TSharedPtr<FJsonObject>* RepObj = nullptr;
            if ((*SObj)->TryGetObjectField(TEXT("reputation"), RepObj) && RepObj && RepObj->IsValid())
            {
                for (const auto& Pair : (*RepObj)->Values)
                {
                    Stats->Reputation.Add(Pair.Key, Pair.Value.Get()->AsNumber());
                }
            }
            (*SObj)->TryGetStringField(TEXT("faction_choice"), Stats->FactionChoice);
        }
    }
    if (Inv)
    {
        Inv->Items.Reset();
        Inv->EquippedWeaponId.Empty();
        const TSharedPtr<FJsonObject>* IObj = nullptr;
        if (Root->TryGetObjectField(TEXT("inventory"), IObj) && IObj && IObj->IsValid())
        {
            const TArray<TSharedPtr<FJsonValue>>* Items = nullptr;
            if ((*IObj)->TryGetArrayField(TEXT("items"), Items) && Items)
            {
                for (const TSharedPtr<FJsonValue>& V : *Items)
                {
                    const TSharedPtr<FJsonObject> O = V.IsValid() ? V->AsObject() : nullptr;
                    if (!O.IsValid()) { continue; }
                    FPISInventoryStack S;
                    S.ItemId = O->GetStringField(TEXT("item_id"));
                    S.Count  = O->GetIntegerField(TEXT("count"));
                    if (!S.ItemId.IsEmpty() && S.Count > 0) { Inv->Items.Add(S); }
                }
            }
            FString Eq;
            if ((*IObj)->TryGetStringField(TEXT("equipped_weapon"), Eq)) { Inv->EquippedWeaponId = Eq; }
        }
    }
    if (Quest)
    {
        Quest->ResetAll();
        const TArray<TSharedPtr<FJsonValue>>* QArr = nullptr;
        if (Root->TryGetArrayField(TEXT("quests"), QArr) && QArr)
        {
            for (const TSharedPtr<FJsonValue>& V : *QArr)
            {
                const TSharedPtr<FJsonObject> O = V.IsValid() ? V->AsObject() : nullptr;
                if (!O.IsValid()) { continue; }
                FPISQuestState S;
                S.QuestId = O->GetStringField(TEXT("quest_id"));
                S.StageId = O->GetStringField(TEXT("stage_id"));
                S.Status  = static_cast<EPISQuestStatus>(O->GetIntegerField(TEXT("status")));
                S.ProgressCount = O->GetIntegerField(TEXT("progress"));
                if (!S.QuestId.IsEmpty()) { Quest->States.Add(S); }
            }
        }
    }
    return true;
}

TArray<FPISSaveSlot> UPISSaveGameSubsystem::ListSlots() const
{
    TArray<FPISSaveSlot> Out;
    IPlatformFile& PF = FPlatformFileManager::Get().GetPlatformFile();
    const FString Dir = GetSaveDirectory();
    if (!PF.DirectoryExists(*Dir)) { return Out; }

    class FVisitor : public IPlatformFile::FDirectoryVisitor
    {
    public:
        TArray<FString>& Files;
        FVisitor(TArray<FString>& InFiles) : Files(InFiles) {}
        virtual bool Visit(const TCHAR* FilenameOrDirectory, bool bIsDirectory) override
        {
            if (!bIsDirectory) { Files.Add(FilenameOrDirectory); }
            return true;
        }
    };
    TArray<FString> Files;
    FVisitor V(Files);
    PF.IterateDirectory(*Dir, V);

    for (const FString& F : Files)
    {
        if (!F.EndsWith(TEXT(".json"))) { continue; }
        FString Json;
        if (!FFileHelper::LoadFileToString(Json, *F)) { continue; }
        TSharedPtr<FJsonObject> Root;
        auto Reader = TJsonReaderFactory<>::Create(Json);
        if (!FJsonSerializer::Deserialize(Reader, Root) || !Root.IsValid()) { continue; }
        FPISSaveSlot S;
        FString Base = FPaths::GetBaseFilename(F);
        Base.RemoveFromStart(TEXT("pis_save_"));
        S.SlotName = Base;
        Root->TryGetStringField(TEXT("saved_at"), S.Timestamp);
        const TSharedPtr<FJsonObject>* StatsObj = nullptr;
        if (Root->TryGetObjectField(TEXT("stats"), StatsObj) && StatsObj && StatsObj->IsValid())
        {
            S.PlayerLevel = (*StatsObj)->GetIntegerField(TEXT("level"));
            (*StatsObj)->TryGetStringField(TEXT("faction_choice"), S.FactionChoice);
        }
        S.SummaryLocation = FString::Printf(TEXT("x=%.0f y=%.0f z=%.0f"),
            Root->GetNumberField(TEXT("pos_x")),
            Root->GetNumberField(TEXT("pos_y")),
            Root->GetNumberField(TEXT("pos_z")));
        Out.Add(S);
    }
    return Out;
}

bool UPISSaveGameSubsystem::DeleteSlot(const FString& SlotName)
{
    LastError.Empty();
    const FString Path = FPaths::Combine(GetSaveDirectory(), FString::Printf(TEXT("pis_save_%s.json"), *Slugify(SlotName)));
    IPlatformFile& PF = FPlatformFileManager::Get().GetPlatformFile();
    if (!PF.FileExists(*Path)) { LastError = TEXT("Slot nie istnieje."); return false; }
    return PF.DeleteFile(*Path);
}
