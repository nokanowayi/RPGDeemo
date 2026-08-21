// MyRPGDemo

#include "WarriorFunctionLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "WarriorFunctionLibrary.h"

#include "GenericTeamAgentInterface.h"
#include "WarriorGameplayTags.h"
#include "Interfaces/PawnCombatInterface.h"
#include "Kismet/KismetMathLibrary.h"

UWarriorAbilitySystemComponent* UWarriorFunctionLibrary::NativeGetWarriorASCFromActor(AActor* InActor)
{
	
	return  CastChecked<UWarriorAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InActor));
}

void UWarriorFunctionLibrary::AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag TagToAdd)
{
	UWarriorAbilitySystemComponent* ASC = NativeGetWarriorASCFromActor(InActor);

	if (!ASC->HasMatchingGameplayTag(TagToAdd))
	{
		ASC->AddLooseGameplayTag(TagToAdd);
	}
	
}

void UWarriorFunctionLibrary::RemoveGameplayTagFromActorIfFound(AActor* InActor, FGameplayTag TagToAdd)
{
	UWarriorAbilitySystemComponent* ASC = NativeGetWarriorASCFromActor(InActor);
	if (ASC->HasMatchingGameplayTag(TagToAdd))
	{
		ASC->RemoveLooseGameplayTag(TagToAdd);
	}
}

bool UWarriorFunctionLibrary::NativeDoesActorHaveTag(AActor* InActor,FGameplayTag TagToCheck)
{
	return NativeGetWarriorASCFromActor(InActor)->HasMatchingGameplayTag(TagToCheck);
}

void UWarriorFunctionLibrary::BP_DoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck,
	EWarriorConfirmType& OutConfirmType)
{
	OutConfirmType = NativeDoesActorHaveTag(InActor,TagToCheck) ? EWarriorConfirmType::Yes : EWarriorConfirmType::No;
}

UPawnCombatComponent* UWarriorFunctionLibrary::NativeGetPawnCombatComponentFromActor(AActor* InActor)
{
	check(InActor);

	if (Cast<IPawnCombatInterface>(InActor))
	{
		return Cast<IPawnCombatInterface>(InActor)->GetPawnCombatComponent();
	}
	return nullptr;
}

UPawnCombatComponent* UWarriorFunctionLibrary::BP_GetPawnCombatComponentFromActor(AActor* InActor,EWarriorValidType& OutValidType)
{
	UPawnCombatComponent* CombatComponent = NativeGetPawnCombatComponentFromActor(InActor);

	OutValidType = CombatComponent?EWarriorValidType::Valid:EWarriorValidType::Invalid;
	return CombatComponent;
}

bool UWarriorFunctionLibrary::IsTargetPawnHostile(APawn* QueryPawn, APawn* TargetPawn)
{
	check(QueryPawn && TargetPawn);
	IGenericTeamAgentInterface* QueryTeamAgent = Cast<IGenericTeamAgentInterface>(QueryPawn->GetController());
	IGenericTeamAgentInterface* TargetTeamAgent = Cast<IGenericTeamAgentInterface>(TargetPawn->GetController());

	if (QueryTeamAgent && TargetTeamAgent)
	{
		return  QueryTeamAgent->GetGenericTeamId() != TargetTeamAgent->GetGenericTeamId();
	}
	return false;
}

float UWarriorFunctionLibrary::GetScalableFloatValueAtLevel(const FScalableFloat& InScalableFloat, int32 InLevel)
{
	return InScalableFloat.GetValueAtLevel(InLevel);
}

FGameplayTag UWarriorFunctionLibrary::ComputeHitReactDirTag(AActor* InAttacker, AActor* InVictim,
	float& OutAngelDifference)
{
	check(InAttacker && InVictim);
	FVector VictimForward = InVictim->GetActorForwardVector();
	FVector VectorNormalized = (InAttacker->GetActorLocation() - InVictim->GetActorLocation()).GetSafeNormal();

	const float DotRes = FVector::DotProduct(VictimForward, VectorNormalized);
	OutAngelDifference = UKismetMathLibrary::DegAcos(DotRes);

	const FVector CrossRes = FVector::CrossProduct(VictimForward, VectorNormalized);

	if (CrossRes.Z < 0.f)
	{
		OutAngelDifference = -OutAngelDifference;
	}

	if (OutAngelDifference >= -45.f && OutAngelDifference < 45.f)
	{
		return WarriorGameplayTags::Shared_Status_HitReact_Front;
	}
	else if (OutAngelDifference >= 45.f && OutAngelDifference < 135.f)
	{
		return WarriorGameplayTags::Shared_Status_HitReact_Right;
	}
	else if (OutAngelDifference >= -135.f && OutAngelDifference < -45.f)
	{
		return WarriorGameplayTags::Shared_Status_HitReact_Left;
	}
	else if (OutAngelDifference >= 135.f || OutAngelDifference < -135.f)
	{
		return WarriorGameplayTags::Shared_Status_HitReact_Back;
	}
	
	return WarriorGameplayTags::Shared_Status_HitReact_Front;
}

bool UWarriorFunctionLibrary::IsValidBlock(AActor* InAttacker, AActor* InDefender)
{
	check(InAttacker && InDefender);
	const float DotRes = FVector::DotProduct(InDefender->GetActorForwardVector(), InAttacker->GetActorLocation());

	
	return DotRes < 0.1f;
}
