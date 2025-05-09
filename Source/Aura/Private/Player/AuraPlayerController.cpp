// Copyright Ashish


#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
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

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	check(Subsystem);

	Subsystem->AddMappingContext(AuraContext,0);

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

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator controllerRotation = GetControlRotation();
	const FRotator YawRotation = FRotator(0,controllerRotation.Yaw,0);

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

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	EnhancedInputComponent->BindAction(MoveAction,ETriggerEvent::Triggered,this,&AAuraPlayerController::Move);
}
