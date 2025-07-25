// Copyright Ashish


#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

#include "AbilitySystem/AuraAttributeSet.h"

FString UAuraGameplayAbility::GetDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>%s, </><Level>%d</>"), L"Default Ability Name - Ashish Ashish Ashish Ashish Ashish Ashish Ashish Ashish Ashish Ashish Ashish Ashish Ashish Ashish Ashish Ashish Ashish Ashish Ashish Ashish Ashish Ashish ", Level);
}

FString UAuraGameplayAbility::GetNextLevelDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>Next Level: </><Level>%d</> \n<Default>Causes much more damage. </>"), Level);
}

FString UAuraGameplayAbility::GetLockedDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>Spell Locked Until Level: %d</>"), Level);
}

float UAuraGameplayAbility::GetManaCost(float InLevel) const
{
	float ManaCost = 0.f;

	//Used to directly get the cost effect from the ga.
	if (const UGameplayEffect* CostEffect = GetCostGameplayEffect())
	{
		//Loop through all the modifiers in the effect
		for (FGameplayModifierInfo Mod : CostEffect->Modifiers)
		{
			if (Mod.Attribute == UAuraAttributeSet::GetManaAttribute())
			{
				//It will only update the value of mana cost if it is not a curve table . i.e it is a static number.
				//It does not require spec.
				Mod.ModifierMagnitude.GetStaticMagnitudeIfPossible(InLevel, ManaCost);
				break;
			}
		}
	}
	return ManaCost;
}

float UAuraGameplayAbility::GetCooldown(float InLevel) const
{
	float Cooldown = 0.f;
	//for the cooldown we need to get the duration. No need to loop through modifiers.
	if (const UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect())
	{
		//Returns the magnitude of the cooldown duration if it is not a curve table.
		//It does not require spec.
		CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(InLevel, Cooldown);
	}
	return Cooldown;
}
