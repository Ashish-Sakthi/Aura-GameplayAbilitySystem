// Copyright Ashish

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "AuraPlayerState.generated.h"

class ULevelUpInfo;
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChanged, int32 /*StatValue*/)

class UAttributeSet;
/**
 * @class AAuraPlayerState
 * @brief Represents the player state in the Aura game, extending functionality with an Ability System Component and Attribute Set.
 *
 * This class manages the player state functionality, integrating Unreal's Ability System
 * for gameplay attribute management and gameplay effect replication. It also handles
 * player-specific information such as levels and attribute data.
 */
UCLASS()
class AURA_API AAuraPlayerState : public APlayerState,public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AAuraPlayerState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const {return AttributeSet;};

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<ULevelUpInfo> LevelUpInfo;

	FORCEINLINE int32 GetPlayerLevel() const { return Level; };
	FORCEINLINE int32 GetXP() const { return XP; };
	FORCEINLINE int32 GetAttributePoints() const { return AttributePoints; }
	FORCEINLINE int32 GetSpellPoints() const { return SpellPoints; }

	void SetXP(int32 NewXP);
	void SetLevel(int32 NewLevel);
	
	void AddToXP(int32 InXP);
	void AddToLevel(int32 InLevel);
	void AddToAttributePoints(int32 InPoints);
	void AddToSpellPoints(int32 InPoints);

	FOnPlayerStatChanged OnLevelChangedDelegate;
	FOnPlayerStatChanged OnXPChangedDelegate;
	FOnPlayerStatChanged OnAttributePointsChangedDelegate;
	FOnPlayerStatChanged OnSpellPointsChangedDelegate;
	
protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

private:
	
	UPROPERTY(VisibleAnywhere,ReplicatedUsing=OnRep_Level)
	int32 Level = 1 ;
	
	UPROPERTY(VisibleAnywhere,ReplicatedUsing=OnRep_XP)
	int32 XP = 0;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_AttributePoints)
	int32 AttributePoints = 0;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_SpellPoints)
	int32 SpellPoints = 0;

	UFUNCTION()
	void OnRep_XP(int32 OldXP);
	
	UFUNCTION()
	void OnRep_Level(int32 OldLevel);

	UFUNCTION()
	void OnRep_AttributePoints(int32 OldAttributePoints);

	UFUNCTION()
	void OnRep_SpellPoints(int32 OldSpellPoints);

};
