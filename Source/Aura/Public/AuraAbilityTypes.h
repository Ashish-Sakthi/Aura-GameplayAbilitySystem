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

    UPROPERTY()
    TObjectPtr<UObject> WorldContextObject = nullptr;

    UPROPERTY()
    TSubclassOf<UGameplayEffect> DamageGameplayEffectClass = nullptr;

    UPROPERTY()
    TObjectPtr<UAbilitySystemComponent> SourceAbilitySystemComponent;

    UPROPERTY()
    TObjectPtr<UAbilitySystemComponent> TargetAbilitySystemComponent;

    UPROPERTY()
    float BaseDamage = 0.f;

    UPROPERTY()
    float AbilityLevel = 1.f;

    UPROPERTY()
    FGameplayTag DamageType = FGameplayTag();

    UPROPERTY()
    float DebuffChance = 0.f;

    UPROPERTY()
    float DebuffDamage = 0.f;

    UPROPERTY()
    float DebuffDuration = 0.f;

    UPROPERTY()
    float DebuffFrequency = 0.f;

    UPROPERTY()
    float DeathImpulseMagnitude = 0.f;

    UPROPERTY()
    FVector DeathImpulse = FVector::ZeroVector;
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

        // Setter functions to modify critical and blocked hit states
        void SetIsCriticalHit(bool bInIsCriticalHit) { bIsCriticalHit = bInIsCriticalHit; }
        void SetIsBlockedHit(bool bInIsBlockedHit) { bIsBlockedHit = bInIsBlockedHit; }
        void SetIsSuccessfulDebuff(bool bInIsSuccessfulDebuff) { bIsSuccessfulDebuff = bInIsSuccessfulDebuff; }
        void SetDebuffDamage(float InDamage) { DebuffDamage = InDamage; }
        void SetDebuffDuration(float InDuration) { DebuffDuration = InDuration; }
        void SetDebuffFrequency(float InFrequency) { DebuffFrequency = InFrequency; }
        void SetDamageType(TSharedPtr<FGameplayTag> InDamageType) { DamageType = InDamageType; }
        void SetDeathImpulse(const FVector& InImpulse) { DeathImpulse = InImpulse; }
        
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