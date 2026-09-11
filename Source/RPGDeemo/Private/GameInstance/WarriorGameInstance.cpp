// MyRPGDemo


#include "GameInstance/WarriorGameInstance.h"

TSoftObjectPtr<UWorld> UWarriorGameInstance::GetLevelByTag(FGameplayTag LevelTag)
{
	for (const FWarriorGameLevelSet& GameLevelSet : GameLevelSets)
	{
		if (!GameLevelSet.IsValid()) continue;

		if (GameLevelSet.LevelTag == LevelTag)
		{
			return GameLevelSet.Level;
		}
	}

	return nullptr;
}
