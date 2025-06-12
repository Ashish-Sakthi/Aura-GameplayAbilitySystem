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
 * 
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

protected:
	//Gets the DA in BP
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAttributeInfo> AttributeInfo;

private:
	void BroadcastAttributeInfo(FGameplayTag AttributeTag, const FGameplayAttribute& Attribute) const;
};
