// MyRPGDemo


#include "WarriorBaseGameMode.h"
#include "Controllers/WarriorHeroController.h"

AWarriorBaseGameMode::AWarriorBaseGameMode()
{
	PlayerControllerClass = AWarriorHeroController::StaticClass();
}

