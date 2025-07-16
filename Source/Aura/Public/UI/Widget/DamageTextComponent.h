// Copyright Ashish

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "DamageTextComponent.generated.h"

/**
 * UDamageTextComponent is a widget component used for displaying damage numbers
 * in the game. It provides functionality for setting and customizing the displayed
 * damage values. This component is expected to be attached to game objects such as
 * characters or enemies.
 */
UCLASS()
class AURA_API UDamageTextComponent : public UWidgetComponent
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	void SetDamageText(float Damage,bool bBlockedHit,bool bCriticalHit);
};
