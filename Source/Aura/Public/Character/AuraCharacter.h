// Copyright Ashish

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "AuraCharacter.generated.h"

class AAuraPlayerState;

/**
 * @class AAuraCharacter
 * @brief Represents a character in the Aura game that extends the functionalities of AAuraCharacterBase.
 * This class overrides specific behavior for possession and player state replication while interfacing with combat systems.
 */
UCLASS()
class AURA_API AAuraCharacter : public AAuraCharacterBase
{
	GENERATED_BODY()
public:
	AAuraCharacter();
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	/* Combat Interface */
	virtual int32 GetPlayerLevel() override;

private:
	virtual void InitAbilityActorInfo() override;

};
