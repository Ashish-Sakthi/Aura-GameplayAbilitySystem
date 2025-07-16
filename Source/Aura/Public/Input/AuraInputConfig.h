// Copyright Ashish

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "AuraInputConfig.generated.h"

USTRUCT(BlueprintType)
struct FAuraInputAction
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	const class UInputAction* InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputTag = FGameplayTag();
};

/**
 * @class UAuraInputConfig
 * @brief A data asset class that contains input configurations for abilities, defined through a list of input actions and associated gameplay tags.
 *
 * This class is used to bridge gameplay actions with input bindings via the association of input actions and their corresponding gameplay tags.
 * It allows for querying and managing input configurations used in the AURA system.
 *
 * @details
 * The input configuration consists of an array of `FAuraInputAction` structs, each defining a unique mapping of input actions to gameplay tags.
 * These mappings are editable within the editor via the `AbilityInputActions` property. The class also includes utility functionality
 * for searching input actions based on their corresponding gameplay tags.
 */
UCLASS()
class AURA_API UAuraInputConfig : public UDataAsset
{
	GENERATED_BODY()
public:
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag,bool bLogNotFound = false) const;

	//Stores an array of structs(abilities) and makes it viewable in the editor.
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TArray<FAuraInputAction> AbilityInputActions;
};
