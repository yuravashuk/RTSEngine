
#pragma once

#ifndef _BASE_BUILDING_HPP_
#define _BASE_BUILDING_HPP_

#include "RTEngine.hpp"
#include "Constants.hpp"
#include "MineryBuilding.hpp"

inline Resource CreateBankBuilding()
{
	Resource building;
	building.mLevel = 1;
	building.mLevelLimit = 1;
	building.mCreationTime = 0;
	building.mUpgradeTime = 0;
	building.mIsStableResource = true;
	building.mParameters =
	{
		{ BankParameters::kGold, Value(300.0f) }
	};
	building.mIdentifier = sg_BuildCounter;
	sg_BuildCounter++;
	return building;
}

inline Base CreateBase(RTEngine *inEngine)
{
	Base base;
	base.mResources =
	{
		{ BuildTypes::kBank, { CreateBankBuilding() } },
		{ BuildTypes::kMinery, { CreateMineryBuilding(inEngine) }}
	};
	return base;
}

#endif //_BASE_BUILDING_HPP_