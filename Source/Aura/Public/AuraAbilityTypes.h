#pragma once

#include "GameplayEffectTypes.h"  // Include base gameplay effect functionality from Unreal Engine
#include "AuraAbilityTypes.generated.h"  // Generated header for UE reflection system

// Custom gameplay effect context structure that can be used in Blueprints
USTRUCT(BlueprintType)
struct FAuraGameplayEffectContext : public FGameplayEffectContext
{
    GENERATED_BODY()

    public:
        bool IsCriticalHit() const { return bIsCriticalHit; }
        bool IsBlockedHit() const { return bIsBlockedHit; }

        // Setter functions to modify critical and blocked hit states
        void SetIsCriticalHit(bool bInIsCriticalHit) { bIsCriticalHit = bInIsCriticalHit; }
        void SetIsBlockedHit(bool bInIsBlockedHit) { bIsBlockedHit = bInIsBlockedHit; }
        
        /**
         * Required override to provide the correct struct for serialization
         * This is used by Unreal Engine's reflection system to properly serialize the struct
         * @return The static struct information for this type
         */
        virtual UScriptStruct* GetScriptStruct() const
        {
            return StaticStruct();
        }

        /**
         * Creates a deep copy of this context
         * Important for gameplay effects that need to be modified without affecting the original
         * @return A new heap-allocated copy of this context
         */
        virtual FAuraGameplayEffectContext* Duplicate() const
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
};

// Trait specification for the custom gameplay effect context
// This tells Unreal Engine how to handle this struct
template<>
struct TStructOpsTypeTraits< FAuraGameplayEffectContext > : public TStructOpsTypeTraitsBase2< FAuraGameplayEffectContext >
{
    enum
    {
        WithNetSerializer = true,  // Enables network serialization
        WithCopy = true           // Enables proper copying of shared pointers (like FHitResult)
    };
};