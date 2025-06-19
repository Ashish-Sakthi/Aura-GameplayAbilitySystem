// Copyright Ashish


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"	
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
		//Used to complete spawning in deferred spawn. Else not spawned.

		//TODO: Give the projectile a gameplayspec to apply damage
		Projectile->FinishSpawning(SpawnTransform);
		
	}
}
