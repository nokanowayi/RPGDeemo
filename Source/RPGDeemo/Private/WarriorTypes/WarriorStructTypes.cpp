// MyRPGDemo


#include "WarriorTypes/WarriorStructTypes.h"

bool FWarriorHeroAbilitySet::IsValid() const
{
	return InputTag.IsValid() && AbilityToGrant;
}
