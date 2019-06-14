
#include "pch.h"
#include "RTEngine.hpp"
#include <cassert>

// Level Upgrader
Error LevelUpgrader::Upgrade(Base *inBase, Resource *inBuilding)
{
	if (!inBuilding || !inBase)
	{
		return Error(3);
	}

	const auto nextLevel = inBuilding->mLevel + 1;

	if (nextLevel > inBuilding->mLevelLimit)
	{
		return Error(1);
	}

	for (const auto &rule : inBuilding->mUpgradeRules)
	{
		const auto resourceID = rule.mResourceID;
		const auto parameterID = rule.mParameterID;
		const auto coast = rule.mValues[inBuilding->mLevel];

		const auto foundedResources = inBase->mResources.find(rule.mResourceID);

		if (foundedResources != std::end(inBase->mResources))
		{
			auto &resources = foundedResources->second;
			if (resources.size() > 0)
			{
				auto foundedParameter = resources[0].mParameters.find(rule.mParameterID);

				if (foundedParameter != std::end(resources[0].mParameters))
				{
					const auto coast = rule.mValues[inBuilding->mLevel];
					if (foundedParameter->second >= coast)
						foundedParameter->second -= coast;
					else
						return Error(2);
				}
			}
			else
			{
				return Error(4);
			}
		}
	}

	inBuilding->mLevel++; // upgrade level
	return Error(0);
}



// RTEngine
RTEngine::RTEngine()
	: mInternalCounter(0)
{
	mLevelUpgrader = new LevelUpgrader();
}

RTEngine::~RTEngine()
{ }

void RTEngine::Destroy()
{
	if (mLevelUpgrader)
	{
		delete mLevelUpgrader;
		mLevelUpgrader = nullptr;
	}
}

void RTEngine::Upgrade(Base *inBase, u32 inResourceType, u32 inResourceID)
{
	assert(inBase);

	Resource resource;
	if (GetResource(inBase, inResourceType, inResourceID, resource))
	{
		Upgrade(inBase, &resource);
	}
}

void RTEngine::Upgrade(Base *inBase, Resource *inResource)
{
	assert(inResource);

	auto upgrader = inResource->mUpgrader;

	if (upgrader)
	{
		Error result = upgrader->Upgrade(inBase, inResource);
	}
}

void RTEngine::Tick(Base *inBase)
{
	assert(inBase);

	// go throught all resources of curtain type
	for (auto &[id, resources] : inBase->mResources)
	{
		for (auto &resource : resources)
		{
		JMP_TO_NEXT_RESOURCE:

			if (resource.mIsEnabled && !resource.mIsStableResource)
			{
				// is resource need mine some values?
				if (mInternalCounter >= (resource.mLastResourceCreationTime + resource.mResourceCreationTime))
				{
					if (!GetInputElementsFor(&resource, inBase))
					{
						goto JMP_TO_NEXT_RESOURCE;
					}

					ProduceNewElements(&resource, inBase);
					resource.mLastResourceCreationTime = mInternalCounter;
				}
			}
		}
	}

	mInternalCounter++;
}

bool RTEngine::GetInputElementsFor(Resource *inResource, Base *inBase)
{
	const auto inputParameters = inResource->mInputParameters;

	if (inputParameters.empty())
	{
		return true;
	}

	// TODO: - check all values >=, create table [ptr, parameter, value], if all values valid update them

	for (const auto& inputParameter : inputParameters)
	{
		const auto resourceID = inputParameter.mResourceID;
		const auto parameterID = inputParameter.mParameterID;
		const auto level = inResource->mLevel - 1;
		const auto coast = inputParameter.mValues[level];

		std::vector<Resource> foundedResources;
		if (GetResources(inBase, resourceID, foundedResources))
		{
			if (foundedResources.size() > 0)
			{
				Value parameterValue;
				if (GetResourceParameter(&foundedResources[0], parameterID, parameterValue))
				{
					if (parameterValue >= coast)
					{
						parameterValue -= coast;
						SetResourceParameter(&foundedResources[0], parameterID, parameterValue);
					}
					else
					{
						if (inResource->mIsAutoPurchaseEnabled)
						{
							// TODO: purchase input resources if needed
							return false;
						}
						else
						{
							return false;
						}
					}
				}
			}
		}
	}

	return true;
}

bool RTEngine::ProduceNewElements(Resource *inResource, Base *inBase)
{
	const auto outputParameters = inResource->mOutputParameters;

	for (const auto &outputParameter : outputParameters)
	{
		const auto resourceID = outputParameter.mResourceID;
		const auto parameterID = outputParameter.mParameterID;
		const auto level = inResource->mLevel - 1;
		const auto produced = outputParameter.mValues[level];

		std::vector<Resource> foundedResources;
		if (GetResources(inBase, resourceID, foundedResources))
		{
			Value parameterValue;
			if (GetResourceParameter(&foundedResources[0], parameterID, parameterValue))
			{
				parameterValue += produced;
				SetResourceParameter(&foundedResources[0], parameterID, parameterValue);
			}
		}
	}

	return true;
}

void RTEngine::AddResource(Base *inBase, u32 inResourceType, const Resource &inResource)
{
	assert(inBase);

	auto list = inBase->mResources.find(inResourceType);
	if (list != std::end(inBase->mResources))
	{
		if (list->second.size() > inResource.mLimit)
		{
			list->second.push_back(inResource);
		}
	}
}

void RTEngine::AddUnit(Base *inBase, u32 inUnitType, const Unit &inUnit)
{
	assert(inBase);

	inBase->mUnits.insert({ inUnitType, inUnit });
}

bool RTEngine::GetResource(Base *inBase, u32 inResourceType, u32 inResourceID, Resource &outResource)
{
	assert(inBase);

	auto findedList = inBase->mResources.find(inResourceType);
	if (findedList != std::end(inBase->mResources))
	{
		auto resourceList = findedList->second;
		auto finded = std::find_if(std::begin(resourceList), std::end(resourceList), [&id = inResourceID](const Resource &inResource) {
			return inResource.mIdentifier == id;
		});
		if (finded != std::end(resourceList))
		{
			outResource = *finded;
			return true;
		}
	}

	return false;
}

bool RTEngine::GetResources(Base *inBase, u32 inResourceType, std::vector<Resource> &outResources)
{
	assert(inBase);

	std::vector<Resource> result;
	for (auto &[key, resources] : inBase->mResources)
	{
		if (key == inResourceType)
		{
			for (auto &resource : resources)
			{
				result.push_back(resource);
			}
		}
	}

	outResources = result;
	return result.empty();
}

bool RTEngine::GetResourceParameter(Resource *inResource, u32 inParameterID, Value &outValue)
{
	assert(inResource);

	const auto founded = inResource->mParameters.find(inParameterID);
	if (founded != std::end(inResource->mParameters))
	{
		outValue = founded->second;
		return true;
	}

	return false;
}

void RTEngine::SetResourceParameter(Resource *inResource, u32 inParameterID, const Value &inValue)
{
	assert(inResource);

	const auto founded = inResource->mParameters.find(inParameterID);
	if (founded != std::end(inResource->mParameters))
		inResource->mParameters[inParameterID] = inValue;
	else
		inResource->mParameters.insert({ inParameterID, inValue });
}

bool RTEngine::GetUnitByType(Base *inBase, u32 inUnitType, Unit &outUnit)
{
	assert(inBase);

	const auto finded = inBase->mUnits.find(inUnitType);
	if (finded != std::end(inBase->mUnits))
	{
		outUnit = finded->second;
		return true;
	}

	return false;
}

bool RTEngine::GetUnitByID(Base *inBase, u32 inUnitID, Unit &outUnit)
{
	assert(inBase);

	const auto finded = std::find_if(std::begin(inBase->mUnits), std::end(inBase->mUnits), [&id = inUnitID](const auto &u) {
		return u.second.mIdentifier == id;
	});
	
	if (finded != std::end(inBase->mUnits))
	{
		outUnit = finded->second;
		return true;
	}

	return false;
}

LevelUpgrader* RTEngine::GetDefaultLevelUpgrader()
{
	return mLevelUpgrader;
}