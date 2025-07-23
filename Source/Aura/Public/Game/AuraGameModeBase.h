// Copyright Ashish

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AuraGameModeBase.generated.h"

class UAbilityInfo;
class UCharacterClassInfo;

/**
 * @class AAuraGameModeBase
 * @brief The base game mode class for the Aura project.
 *
 * This class serves as the foundation for defining game rules, logic, and gameplay elements specific to the Aura project.
 * It extends the AGameModeBase and provides an interface to access character class information.
 */
UCLASS()
class AURA_API AAuraGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	//Used in ASC Library.
	UPROPERTY(EditDefaultsOnly, Category="Character Class Defaults")
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;

	UPROPERTY(EditDefaultsOnly, Category = "Ability Info")
	TObjectPtr<UAbilityInfo> AbilityInfo;
};
