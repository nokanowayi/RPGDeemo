// MyRPGDemo


#include "AbilitySystem/GEExecCalc/GEExecCalc_DamageTaken.h"

#include "WarriorDebugHelper.h"
#include "WarriorGameplayTags.h"
#include "AbilitySystem/WarriorAttributeSet.h"

struct FWarriorDamageCapture
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPower)
	DECLARE_ATTRIBUTE_CAPTUREDEF(DefensePower)
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageTaken)

	FWarriorDamageCapture()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UWarriorAttributeSet,AttackPower,Source,false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UWarriorAttributeSet,DefensePower,Target,false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UWarriorAttributeSet,DamageTaken,Target,false)
	}
};	

static const FWarriorDamageCapture& GetWarriorDamageCapture()
{
	static FWarriorDamageCapture WarriorDamageCapture;
	return WarriorDamageCapture;
}
UGEExecCalc_DamageTaken::UGEExecCalc_DamageTaken()
{
// 	FProperty* AttackPowerProperty = FindFieldChecked<FProperty>(
// 		UWarriorAttributeSet::StaticClass(),
// 		GET_MEMBER_NAME_CHECKED(UWarriorAttributeSet, AttackPower)
// 		);
// 	FGameplayEffectAttributeCaptureDefinition AttackPowerCaptureDefinition(
// 		AttackPowerProperty,
// 		EGameplayEffectAttributeCaptureSource::Target,
// 		false
// 		);
// 	RelevantAttributesToCapture.Add(AttackPowerCaptureDefinition);
	
	RelevantAttributesToCapture.Add(GetWarriorDamageCapture().AttackPowerDef);
	RelevantAttributesToCapture.Add(GetWarriorDamageCapture().DefensePowerDef);
	RelevantAttributesToCapture.Add(GetWarriorDamageCapture().DamageTakenDef);
}

void UGEExecCalc_DamageTaken::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& EffectSpec = ExecutionParams.GetOwningSpec();
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = EffectSpec.CapturedSourceTags.GetAggregatedTags();
	EvaluationParameters.TargetTags = EffectSpec.CapturedTargetTags.GetAggregatedTags();

	float SourceAttackPower = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetWarriorDamageCapture().AttackPowerDef,EvaluationParameters,SourceAttackPower);
	//Debug::Print(TEXT("SourceAttackPower"),SourceAttackPower,FColor::Green,4.f);

	float BaseDamage = 0.f;
	int32 LightComboCount = 0;
	int32 HeavyComboCount = 0;
	for (const TPair<FGameplayTag, float>& TagMagnitudePair : EffectSpec.SetByCallerTagMagnitudes)
	{
		if (TagMagnitudePair.Key.MatchesTagExact(WarriorGameplayTags::Shared_SetByCaller_BaseDamage))
		{
			BaseDamage = TagMagnitudePair.Value;
			//Debug::Print(TEXT("BaseDamage"),BaseDamage,FColor::Green,4.f);
		}

		if (TagMagnitudePair.Key.MatchesTagExact(WarriorGameplayTags::Player_SetByCaller_AttackType_Light))
		{
			LightComboCount = TagMagnitudePair.Value;
			//Debug::Print(TEXT("LightComboCount"),LightComboCount,FColor::Green,4.f);
		}

		if (TagMagnitudePair.Key.MatchesTagExact(WarriorGameplayTags::Player_SetByCaller_AttackType_Heavy))
		{
			HeavyComboCount = TagMagnitudePair.Value;
			//Debug::Print(TEXT("HeavyComboCount"),HeavyComboCount,FColor::Green,4.f);
		}
	}
	float TargetDefensePower = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetWarriorDamageCapture().DefensePowerDef,EvaluationParameters,TargetDefensePower);
	//Debug::Print(TEXT("TargetDefensePower"),TargetDefensePower,FColor::Green,4.f,1);

	if (LightComboCount!=0)
	{
		const float DamageIncrease = (LightComboCount - 1) * 0.05 + 1;
		BaseDamage *= DamageIncrease;
	}

	if (HeavyComboCount!=0)
	{
		const float DamageIncrease = HeavyComboCount * 0.15 + 1;
		BaseDamage *= DamageIncrease;
	}

	const float FinalDamage = FMath::Max(0.f, (BaseDamage + SourceAttackPower) / TargetDefensePower);
	
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(GetWarriorDamageCapture().DamageTakenProperty,EGameplayModOp::Override,FinalDamage));
}
