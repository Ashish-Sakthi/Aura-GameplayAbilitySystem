// Copyright Ashish

#pragma once

#include "CoreMinimal.h"
#include "AuraInputConfig.h"
#include "EnhancedInputComponent.h"
#include "AuraInputComponent.generated.h"

//Need to assign the InputComponent to the AuraInputComponent in project settings.

/**
 * UAuraInputComponent extends the functionality of UEnhancedInputComponent to provide additional support for binding ability-related input actions.
 * - Allows binding of input actions defined in UAuraInputConfig to ability-related functions
 * - Supports three types of input events: Pressed, Released and Held
 * - Uses gameplay tags to identify and map specific input actions
 * - Integrates with the Enhanced Input System for more robust input handling
 */

UCLASS()
class AURA_API UAuraInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	// Single User-Defined Template function to bind all the input actions for abilities with pressed, released, and held functionality
	// Binds IA to pressed, released, and held functions simultaneously.
	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
	void BindAbilityActions(const UAuraInputConfig* InputConfig,UserClass* Object,PressedFuncType PressedFunc,ReleasedFuncType ReleasedFunc,HeldFuncType HeldFunc);
};


template <class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
void UAuraInputComponent::BindAbilityActions(const UAuraInputConfig* InputConfig,
 UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, HeldFuncType HeldFunc)
{
	check(InputConfig);//DA

	// Iterate through all ability input actions defined in the input config DA
	for (const FAuraInputAction& Action : InputConfig->AbilityInputActions)
	{
		// Check if both the input action and its associated gameplay tag are valid
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				// Binds IA to the PressedFunc with Action.InputTag as a parameter.
				BindAction(Action.InputAction , ETriggerEvent::Started, Object, PressedFunc, Action.InputTag);
			}
			
			if (ReleasedFunc)
			{
				// Binds IA to the Released with Action.InputTag as a parameter.
				BindAction(Action.InputAction , ETriggerEvent::Completed ,Object, ReleasedFunc, Action.InputTag);
			}
			
			if (HeldFunc)
			{
				// Binds IA to the HeldFunc with Action.InputTag as a parameter.
				BindAction(Action.InputAction , ETriggerEvent::Triggered ,Object, HeldFunc, Action.InputTag);
			}
		}
	}
}
