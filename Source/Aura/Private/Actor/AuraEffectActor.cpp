// Copyright Ashish


#include "Actor/AuraEffectActor.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Kismet/KismetMathLibrary.h"

AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent")));
}

void AAuraEffectActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Track time for sinusoidal movement
	RunningTime += DeltaTime;
	// Calculate the period of sine wave motion
	const float SinePeriod = 2 * PI / SinePeriodConstant; 
	// Reset running time when the period completes
	if (RunningTime > SinePeriod)
	{
		RunningTime = 0.f;
	}
	// Update actor position/rotation based on movement settings
	ItemMovement(DeltaTime);
}

void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
	// Store initial transforms when an actor spawns
	InitialLocation = GetActorLocation();
	CalculatedLocation = InitialLocation;
	CalculatedRotation = GetActorRotation();	
}

void AAuraEffectActor::StartSinusoidalMovement()
{
	// Enable sinusoidal up/down motion
	bSinusoidalMovement = true;
	// Reset location tracking
	InitialLocation = GetActorLocation();
	CalculatedLocation = InitialLocation;
}

void AAuraEffectActor::StartRotation()
{
	// Enable continuous rotation
	bRotates = true;
	// Store current rotation as starting point
	CalculatedRotation = GetActorRotation();
}

void AAuraEffectActor::ItemMovement(float DeltaTime)
{
	if (bRotates)
	{
		// Calculate rotation change based on rotation rate
		const FRotator DeltaRotation(0.f, DeltaTime * RotationRate, 0.f);
		// Compose new rotation
		CalculatedRotation = UKismetMathLibrary::ComposeRotators(CalculatedRotation, DeltaRotation);
	}
	if (bSinusoidalMovement)
	{
		// Calculate vertical offset using sine wave
		const float Sine = SineAmplitude * FMath::Sin(RunningTime * SinePeriodConstant);
		// Apply offset to the initial location
		CalculatedLocation = InitialLocation + FVector(0.f, 0.f, Sine);
	}
}

void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	if (TargetActor->ActorHasTag(FName("Enemy")) && !bApplyEffectsToEnemies) return;
	check(GameplayEffectClass);
	
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (TargetASC == nullptr) return;

	// Create an effect context handle which is needed for creating gameplay effect specs
	// This contains information about the effect's context such as the source and target
	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();//3 needed for 2
	
	// Set this actor as the source object that created/caused this effect
	EffectContextHandle.AddSourceObject(this);
	
	// Create the gameplay effect spec which defines the actual effect to apply
	const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(
		GameplayEffectClass, ActorLevel, EffectContextHandle);//2 needed for 1
	
	// This actually applies the effect (damage, buff, etc.) defined in the GameplayEffectClass to the TargetASC and store it.
	 FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());//1

	const bool bIsInfinite = EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;
	if (bIsInfinite && InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		ActiveEffectHandles.Add(ActiveEffectHandle,TargetASC);
	}

	if (!bIsInfinite)
	{
		Destroy();
	}
}

void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
	if (TargetActor->ActorHasTag(FName("Enemy")) && !bApplyEffectsToEnemies) return;
	if(InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor,InstantGameplayEffectClass);
	}
	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor,DurationGameplayEffectClass);
	}
	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor,InfiniteGameplayEffectClass);
	}
}

void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	if (TargetActor->ActorHasTag(FName("Enemy")) && !bApplyEffectsToEnemies) return;
	// Apply instant gameplay effect if policy is set to apply on end overlap
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor,InstantGameplayEffectClass);
	}
	
	// Apply duration gameplay effect if policy is set to apply on end overlap
	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor,DurationGameplayEffectClass);
	}
	
	// Handle removal of infinite effects if removal policy is set to remove on end overlap
	if (InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		// Get the ability system component from the target actor
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		if (!IsValid(TargetASC)) return;	

		// Store handles of effects that need to be removed
		TArray<FActiveGameplayEffectHandle> HandlesToRemove;
		
		// Iterate through all active effect handles
		for (TPair<FActiveGameplayEffectHandle, UAbilitySystemComponent*> HandlePair : ActiveEffectHandles)
		{
			// If the effect belongs to the target actor's ASC
			if (HandlePair.Value == TargetASC)
			{
				//Remove the gameplay effect from the target.
				TargetASC->RemoveActiveGameplayEffect(HandlePair.Key,1);
				HandlesToRemove.Add(HandlePair.Key);
			}
		}

		// Clean up the stored handles by removing them from the ActiveEffectHandles map.
		for (FActiveGameplayEffectHandle& Handle : HandlesToRemove)
		{
			ActiveEffectHandles.FindAndRemoveChecked(Handle);
		}
	}
}