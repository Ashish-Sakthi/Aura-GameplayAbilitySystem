// Copyright Ashish


#include "AbilitySystem/ModMagCalc/MMC_MaxHealth.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Interaction/CombatInterface.h"

/*
 * MMC are used to calculate the magnitude of modifiers, including variables that are not attributes like level.
 */

UMMC_MaxHealth::UMMC_MaxHealth()
{
	// Setup Vigor capture definition to get Vigor attribute value from the target.
	VigorDef.AttributeToCapture = UAuraAttributeSet::GetVigorAttribute();
	VigorDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	VigorDef.bSnapshot = false; // We want live value, not snapshot


	// Add Vigor definition to array of attributes this MMC will capture (Track the vigor attribute)
	RelevantAttributesToCapture.Add(VigorDef);
}

float UMMC_MaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// Gather gameplay tags from both source and target actors
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	// Setup evaluation parameters with source and target tags
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;
	
	float Vigor = 0.f;
	// Retrieves the captured (tracked) Vigor attribute value.
	// Updates the Vigor variable through the output parameter
	GetCapturedAttributeMagnitude(VigorDef, Spec, EvaluationParameters, Vigor);//Like GetVigor() but includes buffs and debuffs
	Vigor = FMath::Max<float>(Vigor, 0.f);

	// Get player level through the combat interface
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(Spec.GetContext().GetSourceObject());
	const int32 PlayerLevel = CombatInterface->GetPlayerLevel();

	// Calculate and return max health based on base health, Vigor bonus and level bonus.
	return 80.f + 2.5f * Vigor + 10.f * PlayerLevel;
}
