// Copyright Ashish

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "OverlayWidgetController.generated.h"

/*
 * Dynamic Multicast Delegate that broadcasts health changes to the UI.
 * Takes a single float parameter and can be bound to multiple functions.
 * To broadcast: OnHealthChanged.Broadcast(NewHealthValue);
 * To bind in C++: OnHealthChanged.AddDynamic(this, &ThisClass::FunctionName);
 * To bind in Blueprint: Bind Event to OnHealthChanged
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChangedSignature, float, NewHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxHealthChangedSignature, float, NewMaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnManaChangedSignature, float, NewMana);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxManaChangedSignature, float, NewMaxMana);
/**
 * 
 */
UCLASS(BlueprintType,Blueprintable)
class AURA_API UOverlayWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	//BlueprintAssignable specifies that this delegate can be assigned to in Blueprints 
	UPROPERTY(BlueprintAssignable,Category = "GAS|Attributes")
	FOnHealthChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable,Category = "GAS|Attributes")
	FOnMaxHealthChangedSignature OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable,Category = "GAS|Attributes")
	FOnManaChangedSignature OnManaChanged;

	UPROPERTY(BlueprintAssignable,Category = "GAS|Attributes")
	FOnMaxManaChangedSignature OnMaxManaChanged;

protected:
	/*The parameters should be "const FOnAttributeChangeData& Data" to bind to
	the delegate GetGameplayAttributeValueChangeDelegate(Attribute).AddObject(this,&class::Fncn).*/
	void HealthChanged(const FOnAttributeChangeData& Data) const;
	void MaxHealthChanged(const FOnAttributeChangeData& Data) const;
	void ManaChanged(const FOnAttributeChangeData& Data) const;
	void MaxManaChanged(const FOnAttributeChangeData& Data) const;
};
