// Copyright Ashish


#include "AbilitySystem/AuraAbilitySystemLibrary.h"	

#include "AbilitySystemComponent.h"
#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "Player/AuraPlayerState.h"
#include "UI/Hud/AuraHUD.h"

//Creates a Function that is blueprint pure and static.
UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	//Getting the Local Player Controller
	if(APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject,0))
	{
		if (AAuraHUD* AuraHud = Cast<AAuraHUD>(PC->GetHUD()))
		{
			AAuraPlayerState* PS = Cast<AAuraPlayerState>(PC->PlayerState);
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			const FWidgetControllerParams WidgetControllerParams(PC,PS,ASC,AS);
			return AuraHud->GetOverlayWidgetController(WidgetControllerParams);//Creates the widget controller if not created.
		}
	}
	return nullptr;
}

UAttributeInfoWidgetController* UAuraAbilitySystemLibrary::GetAttributeInfoWidgetController(
	const UObject* WorldContextObject)
{
	//Getting the Local Player Controller
	if(APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject,0))
	{
		if (AAuraHUD* AuraHud = Cast<AAuraHUD>(PC->GetHUD()))
		{
			AAuraPlayerState* PS = Cast<AAuraPlayerState>(PC->PlayerState);
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			const FWidgetControllerParams WidgetControllerParams(PC,PS,ASC,AS);
			return AuraHud->GetAttributeInfoWidgetController(WidgetControllerParams);//Creates the widget controller if not created.
		}
	}
	return nullptr;
}

void UAuraAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject,
	ECharacterClass CharacterClass, float level,UAbilitySystemComponent* ASC)
{
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (AuraGameMode == nullptr) return;

	AActor* AvatarActor = ASC->GetAvatarActor();

	//Getting the default attributes for the character class from DA in the Game Mode using CharacterClass.
	UCharacterClassInfo* CharacterClassInfo = AuraGameMode->CharacterClassInfo;//Entire DA.
	FCharacterClassDefaultInfo ClassDefaultInfo =
		CharacterClassInfo->GetClassDefaultInfo(CharacterClass);//Struct in the Map in DA.

	FGameplayEffectContextHandle PrimaryAttributeContextHandle = ASC->MakeEffectContext();
	PrimaryAttributeContextHandle.AddSourceObject(AvatarActor);
	FGameplayEffectSpecHandle PrimaryAttributesSpecHandle = ASC->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttributes,level,PrimaryAttributeContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle SecondaryAttributeContextHandle = ASC->MakeEffectContext();
	SecondaryAttributeContextHandle.AddSourceObject(AvatarActor);
	FGameplayEffectSpecHandle SecondaryAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttributes,level,SecondaryAttributeContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle VitalAttributeContextHandle = ASC->MakeEffectContext();
	VitalAttributeContextHandle.AddSourceObject(AvatarActor);
	FGameplayEffectSpecHandle VitalAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->VitalAttributes,level,VitalAttributeContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalAttributesSpecHandle.Data.Get());
}

void UAuraAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC)
{
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (AuraGameMode == nullptr) return;

	//Gives the abilities to the enemy. Does not activate here.
	UCharacterClassInfo* CharacterClassInfo = AuraGameMode->CharacterClassInfo;
	for (const TSubclassOf<UGameplayAbility> AbilityClass : CharacterClassInfo->CommonAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		ASC->GiveAbility(AbilitySpec);
	}
}
