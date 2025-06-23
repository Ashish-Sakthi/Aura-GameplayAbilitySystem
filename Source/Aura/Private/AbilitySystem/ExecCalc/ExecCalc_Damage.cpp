// Copyright Ashish


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystem/AuraAttributeSet.h"

struct AuraDamageStatics
{
	//Helper macro for declaring attribute captures
	//Same as doing in mmc function for vigor def but using macros.
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor)
	AuraDamageStatics()
	{
		// Define attribute capture for Armor from target, not snapshotted
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,Armor,Target,false)	
	}
};

//Static function that returns the same struct variable every time.
static const AuraDamageStatics& DamageStatics()
{
	static AuraDamageStatics DStatics;
	return DStatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	// Add Armor attribute to list of attributes we want to capture
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
}

//Called when owning GE is executed.
void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
											  FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	// Get ability system components for source and target
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	// Get actor references, with null checks
	const AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	const AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	// Get gameplay effect spec 
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	// Get tags from source and target
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	
	float Armor = 0.f;
	//Retrive armor value from the capture.
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParameters, Armor);
	Armor = FMath::Max<float>(0.f, Armor); // Need to clamp because it does no process PreAttributeChange Clamping.
	++Armor;

	// Add armor modification to output
	const FGameplayModifierEvaluatedData EvaluatedData(DamageStatics().ArmorProperty, EGameplayModOp::Additive, Armor);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}