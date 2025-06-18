// Copyright Ashish


#include "AbilitySystem/AbilityTask//TargetDataUnderMouse.h"
#include "AbilitySystemComponent.h"

/**
 * Implements functions for sending local player's mouse position data to the server.
 * CreateTargetDataUnderMouse - Creates a new task instance. Static function.
 */

UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	// Create a new instance of this ability task, passing in the owning ability
	UTargetDataUnderMouse* MyObj = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);
	return MyObj;
}

void UTargetDataUnderMouse::Activate()
{
	const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();

	// If locally controlled, send mouse cursor data to server
	if (bIsLocallyControlled)
	{
		SendMouseCursorData(); 
	}	
	else
	{
		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
		
		// Bind the target data replication callback when the data arrives to server.
		AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(
				SpecHandle, ActivationPredictionKey).AddUObject(this, &UTargetDataUnderMouse::OnTargetDataReplicatedCallback);

		// Check if we already have replicated data on server and can call the delegate immediately.
		const bool bCalledDelegate = AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey);
		if (!bCalledDelegate)
		{
			// No data yet, wait for it to arrive from the client.
			SetWaitingOnRemotePlayerData();
		}
	}
}


 /*
  *Called only on clients to send cursor data to server in multiplayer.
  *Target data is used to communicate where abilities should be activated. 
  */
void UTargetDataUnderMouse::SendMouseCursorData() 
{
	// Create scoped prediction window to handle client-side prediction. Everything within this scope will be predicted.
	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());
	
	// Get the player controller and perform line trace under the cursor
	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	FHitResult CursorHit;
	PC->GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);

	// Create target data handle and populate with the hit result
	FGameplayAbilityTargetDataHandle DataHandle;
	FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
	Data->HitResult = CursorHit;
	DataHandle.Add(Data);
	
	// Send the target data to the server to replicate the input validation. Here the target data to send is hit result.
	AbilitySystemComponent->ServerSetReplicatedTargetData(
		GetAbilitySpecHandle(),
		GetActivationPredictionKey(),//Prediction key when ability was activated is the original prediction key.
		DataHandle,
		FGameplayTag(),
		AbilitySystemComponent->ScopedPredictionKey); //ASC knows the current prediction key.

	// If we should broadcast, send the target data through the ValidData delegate
	if (ShouldBroadcastAbilityTaskDelegates())//Only broadcast if ability is active.
	{
		ValidData.Broadcast(DataHandle);
	}
}

//Receives the data handle that was broadcasted on the server side.Delegate is binded to this function.
void UTargetDataUnderMouse::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle,
	FGameplayTag ActivationTag)
{
	//Tells that the data is received.This prevents the same data from being processed multiple times.Clears the target data.
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
	
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		//Broadcasting is creating a node for the task.
		ValidData.Broadcast(DataHandle);
	}
}

