// Copyright Ashish


#include "AbilitySystem/Data/CharacterClassInfo.h"

FCharacterClassDefaultInfo UCharacterClassInfo::GetClassDefaultInfo(ECharacterClass Class) const
{
	return CharacterClassInformation.FindChecked(Class);
}
