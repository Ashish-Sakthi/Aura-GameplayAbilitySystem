// Copyright Ashish

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "AttributeInfoWidgetController.generated.h"

struct FGameplayAttribute;
//Sends the struct data to the attribute menu.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributeInfoSignature,const FAuraAttributeInfo&,Info );

/**
 * UAttributeInfoWidgetController is responsible for managing and broadcasting attribute-related information
 * to the UI widgets. It extends the functionality of UAuraWidgetController and works closely with attribute
 * data and gameplay systems.
 */
UCLASS(BlueprintType,Blueprintable)
class AURA_API UAttributeInfoWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable,Category = "GAS|Attributes")
	FAttributeInfoSignature AttributeInfoDelegate;

	UPROPERTY(BlueprintAssignable,Category = "GAS|Attributes")
	FOnPlayerStatChangedSignature AttributePointsChangedDelegate;

	UFUNCTION(BlueprintCallable)
	void UpgradeAttribute(const FGameplayTag& AttributeTag);

protected:
	//Gets the DA in BP
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAttributeInfo> AttributeInfo;

private:
	void BroadcastAttributeInfo(FGameplayTag AttributeTag, const FGameplayAttribute& Attribute) const;
};
