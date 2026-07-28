// MyRPGDemo


#include "AnimInstances/Hero/WarriorHeroAnimInstance.h"
#include "Characters/WarriorHeroCharacter.h"

void UWarriorHeroAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	OwningHeroCharacter = Cast<AWarriorHeroCharacter>(OwningCharacter);
}

void UWarriorHeroAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	if (bHasAcceleration)
	{
		IdleElpasedTime = 0.f;
		bShouldEnterRelaxState = false;
	}
	else
	{
		IdleElpasedTime += DeltaSeconds;
		if (IdleElpasedTime >= EnterRelaxStateThreshold)
		{
			bShouldEnterRelaxState = true;
		}
		else
		{
			bShouldEnterRelaxState = false;
		}
	}
}
