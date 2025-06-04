// Copyright Ashish


#include "Actor/AuraEffectActor.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"

AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent")));
}

void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
}

void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
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
		GameplayEffectClass, 1.0f, EffectContextHandle);//2 needed for 1
	
	// This actually applies the effect (damage, buff etc) defined in the GameplayEffectClass to the TargetASC.
	TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());//1
}
