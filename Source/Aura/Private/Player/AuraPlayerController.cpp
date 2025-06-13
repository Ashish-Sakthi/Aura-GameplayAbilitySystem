// Copyright Ashish


#include "Player/AuraPlayerController.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Input/AuraInputComponent.h"
#include "Interaction/EnemyInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	CursorTrace();
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();

	check(AuraContext);// Engine Crashes if nullptr. i.e if not assigned to BP.

	//Should not use check() as only valid on locally controlled player.For others it will be 'null'
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(AuraContext,0);
	}
	
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	//Configure input mode to allow both game and UI interaction
	FInputModeGameAndUI InputModeData;
	//Allow mouse cursor to move freely outside the viewport to UI
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	// Keep cursor visible when capturing input. i.e,want the cursor to remain visible for UI interaction.
	InputModeData.SetHideCursorDuringCapture(false);
	// Apply the input mode settings
	SetInputMode(InputModeData);
}

void AAuraPlayerController::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility,false,CursorHit);
	if (!CursorHit.bBlockingHit) return;

	LastEnemy = ThisEnemy;
	ThisEnemy = CursorHit.GetActor(); // No need to cast the actor to EnemyInterface as we are using TScriptInterface in .h file.

	/**
		 Line trace from cursor. There are several scenarios:
		 * A. LastActor is null && ThisActor is null
		     - Do nothing
		 * B. LastActor is null && ThisActor is valid
		     - Highlight ThisActor
		 * C. LastActor is valid && ThisActor is null
		     - UnHighlight LastActor
		 * D. Both actors are valid, but LastActor != ThisActor
		     - UnHighlight LastActor, and Highlight ThisActor
		 * E. Both actors are valid, and are the same actor
		     - Do nothing
	 */

	if (LastEnemy == nullptr)
	{
		if (ThisEnemy != nullptr)
		{
			FName temp = FName(ThisEnemy.GetObject()->GetName());
			UE_LOG(LogTemp,Display,TEXT("%s"),*temp.ToString());
			ThisEnemy->HighlightActor(); // Case B
		}
		else
		{
			// Case A
		}
	}
	else
	{
		if (ThisEnemy == nullptr)
		{
			LastEnemy->UnHighlightActor(); // Case C
		}
		else
		{
			if (ThisEnemy != LastEnemy)
			{
				LastEnemy->UnHighlightActor(); // Case D
				ThisEnemy->HighlightActor();
			}
			else
			{
				// Case E
			}
		}
	}
}


/*
 * Ability related functions.Binded to input.
 */

//The tag of whatever input is pressed will be passed to this function.
void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	//GEngine->AddOnScreenDebugMessage(1, 3.f, FColor::Red, *InputTag.ToString());
}

//The tag of whatever input is pressed will be passed to this function.
void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	//All the Ability related functions will be handled in ASC so redirecting to ASC.
	if (GetASC() == nullptr) return;
	GetASC()->AbilityInputTagReleased(InputTag);
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	//All the Ability related functions will be handled in ASC so redirecting to ASC.
	if (GetASC() == nullptr) return;
	GetASC()->AbilityInputTagHeld(InputTag);
}

/*
 * End of Ability related functions.
 */


//Gets the ASC from the Pawn.
UAuraAbilitySystemComponent* AAuraPlayerController::GetASC()
{
	if (AuraAbilitySystemComponent == nullptr)
	{
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}
	return AuraAbilitySystemComponent;
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator ControllerRotation = GetControlRotation();
	const FRotator YawRotation = FRotator(0,ControllerRotation.Yaw,0);

	const FVector ForwardVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardVector,InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightVector,InputAxisVector.X);
	}
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	//Need to assign the InputComponent to the AuraInputComponent in project settings.
	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);
	AuraInputComponent->BindAction(MoveAction,ETriggerEvent::Triggered,this,&AAuraPlayerController::Move);

	//Gets the Input Config DA from the Blueprint and binds the actions to the function.
	AuraInputComponent->BindAbilityActions(InputConfig,this,&ThisClass::AbilityInputTagPressed,&ThisClass::AbilityInputTagReleased,&ThisClass::AbilityInputTagHeld);
}
