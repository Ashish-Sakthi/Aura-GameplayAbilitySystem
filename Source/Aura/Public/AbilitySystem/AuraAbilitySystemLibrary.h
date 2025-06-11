// Copyright Ashish

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AuraAbilitySystemLibrary.generated.h"

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
	static UOverlayWidgetController* GetOverlayWidgetController(const UObject* GetWorldContextObject);

	UFUNCTION(BlueprintPure,Category = "AuraAbilitySystemLibrary|WidgetController")
	static UAttributeInfoWidgetController* GetAttributeInfoWidgetController(const UObject* GetWorldContextObject);
};
