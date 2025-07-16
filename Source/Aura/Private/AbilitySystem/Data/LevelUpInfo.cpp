// Copyright Ashish


#include "AbilitySystem/Data/LevelUpInfo.h"

int32 ULevelUpInfo::FindLevelForXP(int32 XP) const
{
	// 0th element is a place holder in the DA.
	int32 Level = 1;
	while( Level < LevelUpInfos.Num())
	{
		if (LevelUpInfos[Level].LevelUpRequirement <= XP)
		{
			++Level;
		}
		else
		{
			break;
		}
	}
	return Level;
}
