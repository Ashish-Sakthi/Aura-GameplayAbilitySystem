// Copyright Ashish


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"	
#include "AuraGameplayTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                           const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
}

void UAuraProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation)
{
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsServer) return;
	
	/*
	 * Gets combat interface to get the spawn point socket with refencing actual player.
	 * SpawnActorDiffered helps to modify the actor before actually spawning with finish spawning.
	 */
	if(ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo()))
	{
		FTransform SpawnTransform;
		const FVector SocketLocation = CombatInterface->GetCombatSocketLocation();
		SpawnTransform.SetLocation(SocketLocation);

		FRotator SpawnRotation = (ProjectileTargetLocation - SocketLocation).Rotation();
		SpawnRotation.Pitch = 0.f;
		SpawnTransform.SetRotation(SpawnRotation.Quaternion());
		
		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
		);

		//Set the damage effect on the projectile.
		const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
		const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass,1,SourceASC->MakeEffectContext());
		const float ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Damage: %f"), ScaledDamage));
		//SetByCaller GE: Instead of hardcoding the value for Damage, setting the value the values in the ability.
		//Sends a key value pair of tag and magnitude to the GE.
		//Makes the damage modular and can be used by any ability.
		const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,GameplayTags.Damage,ScaledDamage);

		Projectile->DamageHandle = SpecHandle;
		Projectile->FinishSpawning(SpawnTransform);
		
	}
}
