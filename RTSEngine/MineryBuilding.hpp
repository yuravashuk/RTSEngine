
#pragma once

#ifndef _MINERY_BUILDING_HPP_
#define _MINERY_BUILDING_HPP_

#include "Constants.hpp"
#include "RTEngine.hpp"

inline Resource CreateMineryBuilding(RTEngine *inEngine)
{
	Resource building;
	building.mLevel = 1; // current level
	building.mLevelLimit = 3; // maximum level
	building.mLimit = UINT32_MAX; // no building limit
	building.mLastResourceCreationTime = 0; //TODO: get current time + offset
	building.mResourceCreationTime = 10; // 
	building.mCreationTime = kDefaultBuildingCreateTime; // creation time
	building.mUpgradeTime = kDefaultBuildingUpgradeTime; // upgrade time
	building.mInputParameters = { }; // no input parameters
	building.mOutputParameters = // increase bank gold value with parameters depend on minery level
	{
		ResourceParameterValues(BuildTypes::kBank, BankParameters::kGold, {Value(1.0f), Value(1.5f), Value(2.0f)})
	};
	building.mUpgradeRules = // upgrade minery level rule, depend on level and bank gold value
	{
		ResourceParameterValues(BuildTypes::kBank, BankParameters::kGold, { Value(20.0f), Value(40.0f), Value(60.0f) })
	};
	building.mUpgrader = inEngine->GetDefaultLevelUpgrader();
	building.mIdentifier = sg_BuildCounter; // unique identifier
	sg_BuildCounter++;

	return building;
}

#endif //_MINERY_BUILDING_HPP_