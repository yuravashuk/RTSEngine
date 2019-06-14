
#pragma once

#ifndef _CONSTANTS_HPP_
#define _CONSTANTS_HPP_

#include "RTEngine.hpp"

static constexpr u32 kDefaultBuildingCreateTime = 15;
static constexpr u32 kDefaultBuildingUpgradeTime = 10;

namespace BuildTypes
{
	static constexpr u32 kBank = 0;
	static constexpr u32 kMinery = 1;
	static constexpr u32 kMilitaryAcademy = 2;
	static constexpr u32 kPowerFactory = 3;
}

namespace BankParameters { static constexpr u32 kGold = 0; }
namespace MineryParameters { static constexpr u32 kRate = 1; }
namespace MilitaryParameters { static constexpr u32 kEffect = 2; }

static std::map<u32, std::string> sg_BuildingNames =
{
	{ BuildTypes::kBank, "Base Bank" },
	{ BuildTypes::kMinery, "Minery" },
	{ BuildTypes::kMilitaryAcademy, "Military Academy" },
	{ BuildTypes::kPowerFactory, "Power Factory" }
};

static u32 sg_BuildCounter = 0;
static u32 sg_UnitCounter = 0;



#endif //_CONSTANTS_HPP_