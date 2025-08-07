#pragma once

#include "GameplayEffectTypes.h"  // Include base gameplay effect functionality from Unreal Engine
#include "AuraAbilityTypes.generated.h"  // Generated header for UE reflection system

class UGameplayEffect;

/**
 * Structure containing parameters for damage-based gameplay effects
 * Used to configure both immediate damage and damage-over-time (debuff) effects
 * Stores references to source and target ability components, damage values,
 * and debuff-specific parameters like chance, damage, duration, and frequency
 */
USTRUCT(BlueprintType)
struct FDamageEffectParams
{
    GENERATED_BODY()

    FDamageEffectParams(){}

    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<UObject> WorldContextObject = nullptr;

    UPROPERTY(BlueprintReadWrite)
    TSubclassOf<UGameplayEffect> DamageGameplayEffectClass = nullptr;

    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<UAbilitySystemComponent> SourceAbilitySystemComponent;

    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<UAbilitySystemComponent> TargetAbilitySystemComponent;

    UPROPERTY(BlueprintReadWrite)
    float BaseDamage = 0.f;

    UPROPERTY(BlueprintReadWrite)
    float AbilityLevel = 1.f;

    UPROPERTY(BlueprintReadWrite)
    FGameplayTag DamageType = FGameplayTag();

    UPROPERTY(BlueprintReadWrite)
    float DebuffChance = 0.f;

    UPROPERTY(BlueprintReadWrite)
    float DebuffDamage = 0.f;

    UPROPERTY(BlueprintReadWrite)
    float DebuffDuration = 0.f;

    UPROPERTY(BlueprintReadWrite)
    float DebuffFrequency = 0.f;

    UPROPERTY(BlueprintReadWrite)
    float DeathImpulseMagnitude = 0.f;

    UPROPERTY(BlueprintReadWrite)
    FVector DeathImpulse = FVector::ZeroVector;

    UPROPERTY(BlueprintReadWrite)
    float KnockbackForceMagnitude = 0.f;

    UPROPERTY(BlueprintReadWrite)
    float KnockbackChance = 0.f;

    UPROPERTY(BlueprintReadWrite)
    FVector KnockbackForce = FVector::ZeroVector;

    UPROPERTY(BlueprintReadWrite)
    bool bIsRadialDamage = false;

    UPROPERTY(BlueprintReadWrite)
    float RadialDamageInnerRadius = 0.f;

    UPROPERTY(BlueprintReadWrite)
    float RadialDamageOuterRadius = 0.f;

    UPROPERTY(BlueprintReadWrite)
    FVector RadialDamageOrigin = FVector::ZeroVector;
    
};


// Custom gameplay effect context structure that can be used in Blueprints
USTRUCT(BlueprintType)
struct FAuraGameplayEffectContext : public FGameplayEffectContext
{
    GENERATED_BODY()

    public:
        bool IsCriticalHit() const { return bIsCriticalHit; }
        bool IsBlockedHit() const { return bIsBlockedHit; }
        bool IsSuccessfulDebuff() const { return bIsSuccessfulDebuff; }
        float GetDebuffDamage() const { return DebuffDamage; }
        float GetDebuffDuration() const { return DebuffDuration; }
        float GetDebuffFrequency() const { return DebuffFrequency; }
        TSharedPtr<FGameplayTag> GetDamageType() const { return DamageType; }
        FVector GetDeathImpulse() const { return DeathImpulse; }
        FVector GetKnockbackForce() const { return KnockbackForce; }
        bool IsRadialDamage() const { return bIsRadialDamage; }
        float GetRadialDamageInnerRadius() const { return RadialDamageInnerRadius; }
        float GetRadialDamageOuterRadius() const { return RadialDamageOuterRadius; }
        FVector GetRadialDamageOrigin() const { return RadialDamageOrigin; }

        // Setter functions to modify critical and blocked hit states
        void SetIsCriticalHit(bool bInIsCriticalHit) { bIsCriticalHit = bInIsCriticalHit; }
        void SetIsBlockedHit(bool bInIsBlockedHit) { bIsBlockedHit = bInIsBlockedHit; }
        void SetIsSuccessfulDebuff(bool bInIsSuccessfulDebuff) { bIsSuccessfulDebuff = bInIsSuccessfulDebuff; }
        void SetDebuffDamage(float InDamage) { DebuffDamage = InDamage; }
        void SetDebuffDuration(float InDuration) { DebuffDuration = InDuration; }
        void SetDebuffFrequency(float InFrequency) { DebuffFrequency = InFrequency; }
        void SetDamageType(TSharedPtr<FGameplayTag> InDamageType) { DamageType = InDamageType; }
        void SetDeathImpulse(const FVector& InImpulse) { DeathImpulse = InImpulse; }
        void SetKnockbackForce(const FVector& InForce) { KnockbackForce = InForce; }
        void SetIsRadialDamage(bool bInIsRadialDamage) { bIsRadialDamage = bInIsRadialDamage; }
        void SetRadialDamageInnerRadius(float InRadialDamageInnerRadius) { RadialDamageInnerRadius = InRadialDamageInnerRadius; }
        void SetRadialDamageOuterRadius(float InRadialDamageOuterRadius) { RadialDamageOuterRadius = InRadialDamageOuterRadius; }
        void SetRadialDamageOrigin(const FVector& InRadialDamageOrigin) { RadialDamageOrigin = InRadialDamageOrigin; }
        
        /**
         * Required override to provide the correct struct for serialization
         * This is used by Unreal Engine's reflection system to properly serialize the struct
         * @return The static struct information for this type
         */
        virtual UScriptStruct* GetScriptStruct() const //Copied from parent 
        {
            return StaticStruct();
        }

        /**
         * Creates a deep copy of this context
         * Important for gameplay effects that need to be modified without affecting the original
         * @return A new heap-allocated copy of this context
         */
        virtual FAuraGameplayEffectContext* Duplicate() const //Copied from parent
        {
            FAuraGameplayEffectContext* NewContext = new FAuraGameplayEffectContext();
            *NewContext = *this;
            if (GetHitResult())
            {
                // Ensures hit result data is properly deep copied
                NewContext->AddHitResult(*GetHitResult(), true);
            }
            return NewContext;
        }

        /**
         * Handles network serialization of the context
         * Must be implemented for proper network replication
         * @param Ar Archive to serialize to/from
         * @param Map Package map for networking
         * @param bOutSuccess Whether serialization was successful
         * @return True if serialization was successful
         */
        virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
        
    protected:
        // Properties marked for replication and visible to Blueprints
        UPROPERTY()
        bool bIsBlockedHit = false;  // Tracks if the hit was blocked
        
        UPROPERTY()
        bool bIsCriticalHit = false;  // Tracks if the hit was critical

        UPROPERTY()
        bool bIsSuccessfulDebuff = false;

        UPROPERTY()
        float DebuffDamage = 0.f;

        UPROPERTY()
        float DebuffDuration = 0.f;

        UPROPERTY()
        float DebuffFrequency = 0.f;

        TSharedPtr<FGameplayTag> DamageType;

        UPROPERTY()
        FVector DeathImpulse = FVector::ZeroVector;

        UPROPERTY()
        FVector KnockbackForce = FVector::ZeroVector;

        UPROPERTY()
        bool bIsRadialDamage = false;

        UPROPERTY()
        float RadialDamageInnerRadius = 0.f;

        UPROPERTY()
        float RadialDamageOuterRadius = 0.f;

        UPROPERTY()
        FVector RadialDamageOrigin = FVector::ZeroVector;
};

// Trait specification for the custom gameplay effect context
// This tells Unreal Engine how to handle this struct
template<> //Copied from parent
struct TStructOpsTypeTraits< FAuraGameplayEffectContext > : public TStructOpsTypeTraitsBase2< FAuraGameplayEffectContext >
{
    enum
    {
        WithNetSerializer = true,  // Enables network serialization
        WithCopy = true           // Enables proper copying of shared pointers (like FHitResult)
    };
};