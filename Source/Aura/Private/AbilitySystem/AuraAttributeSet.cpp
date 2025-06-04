// Copyright Ashish

#include "AbilitySystem/AuraAttributeSet.h"
#include "Net/UnrealNetwork.h" // Required for replication macros like DOREPLIFETIME 

// Constructor - Initialize default values if needed
UAuraAttributeSet::UAuraAttributeSet()
{
	/* InitHealth comes from ATTRIBUTE_ACCESSORS macro which creates:
	 * GAMEPLAYATTRIBUTE_VALUE_INITTER(Health) */
	InitHealth(50);
	InitMaxHealth(100);
	InitMana(25);
	InitMaxMana(100);
}

// Sets up property replication for network play
void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	// DOREPLIFETIME_CONDITION_NOTIFY macro sets up replication for properties:
	// Param1: Class name
	// Param2: Property name
	// Param3: Condition (COND_None = Always replicate)
	// Param4: Whether to trigger RepNotify (REPNOTIFY_Always = Always notify)
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,Health,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,MaxHealth,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,Mana,COND_None,REPNOTIFY_Always); 
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,MaxMana,COND_None,REPNOTIFY_Always);
}

// RepNotify functions - Called when replicated values change
// GAMEPLAYATTRIBUTE_REPNOTIFY macro handles the attribute replication notification: 
void UAuraAttributeSet::OnRep_Health(const FGameplayAttributeData OldHealth) const
{
	//inform GAS about the Change so it can do actions accordingly
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,Health,OldHealth); 
}

void UAuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,MaxHealth,OldMaxHealth);
}

void UAuraAttributeSet::OnRep_Mana(const FGameplayAttributeData OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,Mana,OldMana);
}

void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,MaxMana,OldMaxMana);
}