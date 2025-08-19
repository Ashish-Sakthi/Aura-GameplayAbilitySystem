// Copyright Ashish

#include "Player/AuraPlayerState.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Net/UnrealNetwork.h"

AAuraPlayerState::AAuraPlayerState()
{
    // Set network update frequency for smooth replication
    NetUpdateFrequency = 100.f;

    // Create and set up the Ability System Component
    // This will automatically detect and work with any AttributeSet components
    AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
    
    // Create the AttributeSet
    // This is automatically registered with the ASC because they share the same owner
    AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");
}

void AAuraPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    //Replicate Level for the player (client).
    DOREPLIFETIME(AAuraPlayerState, Level);
    DOREPLIFETIME(AAuraPlayerState, XP);
    DOREPLIFETIME(AAuraPlayerState, AttributePoints);
    DOREPLIFETIME(AAuraPlayerState, SpellPoints);
}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

void AAuraPlayerState::SetXP(int32 NewXP)
{
    XP = NewXP;
    OnXPChangedDelegate.Broadcast(XP);
}

void AAuraPlayerState::SetLevel(int32 NewLevel)
{
    Level = NewLevel;
    OnLevelChangedDelegate.Broadcast(Level,false);
}

void AAuraPlayerState::SetAttributePoints(int32 NewAttributePoints)
{
    AttributePoints = NewAttributePoints;
    OnAttributePointsChangedDelegate.Broadcast(AttributePoints); 
}

void AAuraPlayerState::SetSpellPoints(int32 NewSpellPoints)
{
    SpellPoints = NewSpellPoints;
    OnSpellPointsChangedDelegate.Broadcast(SpellPoints);
}

void AAuraPlayerState::AddToXP(int32 InXP)
{
    XP += InXP;
    OnXPChangedDelegate.Broadcast(XP);
}

void AAuraPlayerState::AddToLevel(int32 InLevel)
{
    Level += InLevel;
    OnLevelChangedDelegate.Broadcast(Level,true);   
}

void AAuraPlayerState::AddToAttributePoints(int32 InPoints)
{
    AttributePoints += InPoints;
    OnAttributePointsChangedDelegate.Broadcast(AttributePoints);
}

void AAuraPlayerState::AddToSpellPoints(int32 InPoints)
{
    SpellPoints += InPoints;
    OnSpellPointsChangedDelegate.Broadcast(SpellPoints);
}

void AAuraPlayerState::OnRep_XP(int32 OldXP)
{
    OnXPChangedDelegate.Broadcast(XP);
}

void AAuraPlayerState::OnRep_Level(int32 OldLevel)
{
    OnLevelChangedDelegate.Broadcast(Level,true);  
}

void AAuraPlayerState::OnRep_AttributePoints(int32 OldAttributePoints)
{
    OnAttributePointsChangedDelegate.Broadcast(AttributePoints);
}

void AAuraPlayerState::OnRep_SpellPoints(int32 OldSpellPoints)
{
    OnSpellPointsChangedDelegate.Broadcast(SpellPoints);
}
