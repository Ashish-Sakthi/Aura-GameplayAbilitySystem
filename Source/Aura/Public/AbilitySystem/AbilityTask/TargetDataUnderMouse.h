// Copyright Ashish

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "TargetDataUnderMouse.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMouseTargetDataSignature, const FGameplayAbilityTargetDataHandle&, DataHandle);

/**
 * Task for detecting and processing target data under the mouse cursor.
 * This ability task allows for gathering hit results under the mouse cursor
 * and sending the target data to the server for validation and processing.
 */
UCLASS()
class AURA_API UTargetDataUnderMouse : public UAbilityTask
{
	GENERATED_BODY()

public:

	/**
	 * Creates a new task instance for getting target data under the mouse cursor.
	 * @param OwningAbility The gameplay ability that owns this task
	 * @return New instance of UTargetDataUnderMouse task
	 */
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta = (DisplayName = "TargetDataUnderMouse", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UTargetDataUnderMouse* CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility);

	//Delegate will become as an output node in BP.
	UPROPERTY(BlueprintAssignable)
	FMouseTargetDataSignature ValidData;

private:
	//Override Activate
	virtual void Activate() override;
	void SendMouseCursorData();

	void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag);
};
