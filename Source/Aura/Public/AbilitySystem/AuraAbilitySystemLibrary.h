// Copyright Ashish

#pragma once

#include "CoreMinimal.h"
#include "Data/CharacterClassInfo.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AuraAbilitySystemLibrary.generated.h"

class UAbilitySystemComponent;
class UAttributeInfoWidgetController;
class UOverlayWidgetController;
/**
 * UAuraAbilitySystemLibrary is a static function library designed to provide convenient access
 * to various gameplay-related widget controllers, such as the overlay widget controller and
 * attribute info widget controller. It simplifies the process of retrieving and managing
 * UI-related elements within the Aura game framework.
 */
UCLASS()
class AURA_API UAuraAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure,Category = "AuraAbilitySystemLibrary|WidgetController")
	static UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject	);

	UFUNCTION(BlueprintPure,Category = "AuraAbilitySystemLibrary|WidgetController")
	static UAttributeInfoWidgetController* GetAttributeInfoWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable,Category = "AuraAbilitySystemLibrary|CharacterClassDefaults")
	static void InitializeDefaultAttributes(const UObject* WorldContextObject,ECharacterClass CharacterClass,float level,UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable,Category = "AuraAbilitySystemLibrary|CharacterClassDefaults")
	static void GiveStartupAbilities(const UObject* WorldContextObject,UAbilitySystemComponent* ASC);
};
