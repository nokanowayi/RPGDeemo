// MyRPGDemo


#include "Items/PickUps/WarriorDoorBase.h"

#include "WarriorDebugHelper.h"
#include "WarriorGameplayTags.h"
#include "Characters/WarriorHeroCharacter.h"

void AWarriorDoorBase::Interact(const FString& LevelName)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	
	if (!HasAuthority())
	{
		return;
	}

	Debug::Print(TEXT("Go to New Level"));
	World->ServerTravel(TEXT("/Game/Maps/GameModeTestMap?listen"));
}

void AWarriorDoorBase::OnPickUpCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnPickUpCollisionSphereBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep,
	                                           SweepResult);
	if (AWarriorHeroCharacter* WarriorHeroCharacter = Cast<AWarriorHeroCharacter>(OtherActor))
	{
		WarriorHeroCharacter->GetWarriorAbilitySystemComponent()->TryActivateAbilityByTag(WarriorGameplayTags::Player_Ability_Interact);
	}
}
