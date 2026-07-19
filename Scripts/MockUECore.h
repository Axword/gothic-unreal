#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cassert>
#include <cstdio>

using int32 = int;
using uint8 = unsigned char;
using TCHAR = char;

struct FVector {
    float x, y, z;
    FVector(float v = 0) : x(v), y(v), z(v) {}
    FVector(float a, float b, float c) : x(a), y(b), z(c) {}
};

struct FString {
    std::string s;
    FString() : s("") {}
    FString(const char* str) : s(str ? str : "") {}
    FString(const std::string& str) : s(str) {}
    bool IsEmpty() const { return s.empty(); }
    void Empty() { s.clear(); }
    size_t Len() const { return s.length(); }
    bool operator==(const FString& o) const { return s == o.s; }
    bool operator!=(const FString& o) const { return s != o.s; }
    char operator[](size_t i) const { return s[i]; }
    const char* operator*() const { return s.c_str(); }
    
    template<typename... Args>
    static FString Printf(const char* fmt, Args... args) {
        char buf[256];
        snprintf(buf, sizeof(buf), fmt, args...);
        return FString(buf);
    }
};

template <typename T>
struct TArray {
    std::vector<T> v;
    void Add(const T& item) { v.push_back(item); }
    void RemoveAt(size_t i) { if(i < v.size()) v.erase(v.begin() + i); }
    int32 Num() const { return static_cast<int32>(v.size()); }
    T& operator[](size_t i) { return v[i]; }
    const T& operator[](size_t i) const { return v[i]; }
    auto begin() { return v.begin(); }
    auto end() { return v.end(); }
    auto begin() const { return v.begin(); }
    auto end() const { return v.end(); }
};

struct FMath {
    static float Clamp(float val, float minV, float maxV) {
        if(val < minV) return minV;
        if(val > maxV) return maxV;
        return val;
    }
};

#define TEXT(x) x
#define UPROPERTY(...)
#define UFUNCTION(...)
#define UCLASS(...)
#define USTRUCT(...)
#define UENUM(...)
#define GENERATED_BODY()
#define POPIOLISOL_API
#define DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(...) struct FPISQuestChanged { void Broadcast(const auto&) {} };

struct UObject { virtual ~UObject() {} };
struct UActorComponent : public UObject {};
struct UBlueprintFunctionLibrary : public UObject {};
struct AActor : public UObject {
    struct USceneComponent* RootComponent = nullptr;
    bool bCanEverTick = false;
    struct { bool bCanEverTick = false; } PrimaryActorTick;
    virtual void Tick(float DeltaSeconds) {}
};
struct USceneComponent { void SetupAttachment(USceneComponent*) {} };
struct UBoxComponent : public USceneComponent { void SetBoxExtent(const FVector&) {} };
struct UStaticMeshComponent : public USceneComponent {};

template <typename T>
T* CreateDefaultSubobject(const char*) { return new T(); }
