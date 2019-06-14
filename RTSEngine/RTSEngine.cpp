
#include "pch.h"
#include <iostream>

#include "JapaneUnits.hpp"
#include "SlavUnits.hpp"
#include "MineryBuilding.hpp"
#include "MilitaryAcademyBuilding.hpp"
#include "BaseBuilding.hpp"
#include "Constants.hpp"
#include "RTEngine.hpp"

void DumpBases(Base *inBase)
{
	for (const auto&[id, buildings] : inBase->mResources)
	{
		for (const auto& building : buildings)
		{
			std::cout << "Building : " << sg_BuildingNames[id] << " ";
			std::cout << "Level : " << building.mLevel << " ";

			if (id == BuildTypes::kBank)
			{
				const auto goldValue = building.mParameters.at(BankParameters::kGold);
				std::cout << "Gold: " << goldValue.mData.AsFloat << " ";
			}
			else if (id == BuildTypes::kMinery)
			{
				std::cout << "Minery";
			}

			std::cout << "\n";
		}
	}
	std::cout << "\n";
}

int main()
{
    std::cout << "Hello World!\n"; 

	RTEngine engine;
	Base base = CreateBase(&engine);

	int i = 0;
	while (i < 20)
	{
		DumpBases(&base);
		engine.Tick(&base);
		i++;
	}

	engine.Upgrade(&base, BuildTypes::kMinery, 1);

	DumpBases(&base);
	engine.Tick(&base);
	DumpBases(&base);

	return 0;
}