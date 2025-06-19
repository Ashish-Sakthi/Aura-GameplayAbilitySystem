// Copyright Ashish

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "AuraPlayerController.generated.h"

class UAuraAbilitySystemComponent;
class UAuraInputConfig;
class IEnemyInterface;
class UInputMappingContext;
class UInputAction;
class USplineComponent;
struct FInputActionValue;


UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AAuraPlayerController();
	virtual void PlayerTick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	
private:
	UPROPERTY(EditAnywhere , Category="Input")
	TObjectPtr<UInputMappingContext> AuraContext;

	UPROPERTY(EditAnywhere , Category="Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere , Category="Input")
	TObjectPtr<UInputAction> ShiftAction;

	void ShiftPressed() { bShiftPressed = true; }
	void ShiftReleased() { bShiftPressed = false; }
	bool bShiftPressed = false;

	void Move(const FInputActionValue& InputActionValue);

	void CursorTrace();

	TScriptInterface<IEnemyInterface> LastActor; // You can easily access both the object and interface using TScriptInterface.
	TScriptInterface<IEnemyInterface> ThisActor;
	FHitResult CursorHit;

	//Bind to all the inputs for abilities.
	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);

	//Gets DA for array of IA and tag to bind to abilities.
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UAuraInputConfig> InputConfig;

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;

	UAuraAbilitySystemComponent* GetASC();

	FVector CachedDestination = FVector::ZeroVector;
	float FollowTime = 0.0f;//How long to follow the cursor.
	float ShortPressThreshold = 0.3f;
	bool bAutoRunning = false;
	bool bTargeting = false;//hovering over enemy

	UPROPERTY(EditAnywhere)
	float AutoRunAcceptanceRadius = 50.0f;

	UPROPERTY(VisibleAnywhere	)
	TObjectPtr<USplineComponent> Spline;

	void AutoRun();
};
