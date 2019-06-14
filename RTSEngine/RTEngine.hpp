
#pragma once

#ifndef _RTENGINE_HPP_
#define _RTENGINE_HPP_

#include <cinttypes>
#include <array>
#include <string>
#include <vector>
#include <algorithm>
#include <map>

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

struct Error
{
	enum class ParameterType
	{
		Rule, Value, Parameter, MultipleParameter
	};

	u32 mType;

	explicit Error(u32 inType)
		: mType(inType)
	{ }
};

struct Value
{
	union
	{
		float AsFloat;
		u32 AsUInt;
	} mData;

	enum class Type
	{
		Float,
		UInt
	} mType;

	explicit Value(float inFloat = 0.0f)
		: mType(Type::Float)
	{
		mData.AsFloat = inFloat;
	}

	explicit Value(u32 inUint)
		: mType(Type::UInt)
	{
		mData.AsUInt = inUint;
	}

	bool operator >= (const Value &inValue)
	{
		if (inValue.mType == Type::Float)
			return mData.AsFloat >= inValue.mData.AsFloat;
		else
			return mData.AsUInt >= inValue.mData.AsUInt;
	}

	bool operator <= (const Value &inValue)
	{
		if (inValue.mType == Type::Float)
			return mData.AsFloat <= inValue.mData.AsFloat;
		else
			return mData.AsUInt <= inValue.mData.AsUInt;
	}

	Value operator -= (const Value &inValue)
	{
		if (inValue.mType == Type::Float)
			mData.AsFloat -= inValue.mData.AsFloat;
		else if (inValue.mType == Type::UInt)
			mData.AsUInt -= inValue.mData.AsUInt;

		return *this;
	}

	Value operator += (const Value &inValue)
	{
		if (inValue.mType == Type::Float)
			mData.AsFloat += inValue.mData.AsFloat;
		else
			mData.AsUInt += inValue.mData.AsUInt;

		return *this;
	}
};

struct ResourceParameterValues
{
	std::vector<Value> mValues;
	u32 mResourceID;
	u32 mParameterID;

	explicit ResourceParameterValues(u32 inResourceID, u32 inParameterID, const std::vector<Value> &inValues)
		: mValues(inValues)
		, mResourceID(inResourceID)
		, mParameterID(inParameterID)
	{ }
};

using InputParameters = std::vector<ResourceParameterValues>;
using OutputParameters = std::vector<ResourceParameterValues>;
using Parameters = std::map< u32, Value >;
using UpgradeRules = std::vector< ResourceParameterValues >;

struct LevelUpgrader;
struct Resource
{
	// list of input parameters
	InputParameters mInputParameters;

	// list of output parameters
	OutputParameters mOutputParameters;

	// list of upgrade rules
	UpgradeRules mUpgradeRules;

	// list of parameters
	Parameters mParameters;

	// ptr to upgrader structure
	LevelUpgrader *mUpgrader;

	// unique identifier
	u32 mIdentifier;

	// resource level
	u32 mLevel;

	// time to create new resource
	u32 mCreationTime;

	// time when start building
	u32 mAttachTime;

	// time to upgrade resource level
	u32 mUpgradeTime;

	// resource number limit
	u32 mLimit;

	// resource level limit
	u32 mLevelLimit;

	// last resource creation (mine) time
	u32 mLastResourceCreationTime;

	// resource mine time
	u32 mResourceCreationTime;
	
	// is resource availuable
	bool mIsEnabled = true;

	// used for bank or other depot resource types
	bool mIsStableResource = false;

	// upgrade level per tick
	bool mIsUpgraderOnTick = false;

	// use auto purchase if there no resources
	bool mIsAutoPurchaseEnabled = false;

	// is resource created
	bool mIsCreated = false;
};

struct Unit
{ 
	u32 mIdentifier;
};

struct Base
{
	std::map< u32, std::vector<Resource> > mResources;
	std::map< u32, Unit > mUnits;
	std::string mName;
	u32 mLevel;
};

struct LevelUpgrader
{
	Error Upgrade(Base *inBase, Resource *inBuilding);
};

class RTEngine final
{
public:
	explicit RTEngine();
	~RTEngine();

	void Destroy();
	void Tick(Base *inBase);

	void Upgrade(Base *inBase, u32 inResourceType, u32 inResourceID);
	void Upgrade(Base *inBase, Resource *inResource);

	void AddResource(Base *inBase, u32 inResourceType, const Resource &inResource);
	void AddUnit(Base *inBase, u32 inUnitType, const Unit &inUnit);

	bool GetResource(Base *inBase, u32 inResourceType, u32 inResourceID, Resource &outResource);
	bool GetResources(Base *inBase, u32 inResourceType, std::vector<Resource> &outResources);

	bool GetResourceParameter(Resource *inResource, u32 inParameterID, Value &outValue);
	void SetResourceParameter(Resource *inResource, u32 inParameterID, const Value &inValue);

	bool GetUnitByType(Base *inBase, u32 inUnitType, Unit &outUnit);
	bool GetUnitByID(Base *inBase, u32 inUnitID, Unit &outUnit);

	LevelUpgrader *GetDefaultLevelUpgrader();

private:

	bool GetInputElementsFor(Resource *inResource, Base *inBase);
	bool ProduceNewElements(Resource *inResource, Base *inBase);

private:
	LevelUpgrader *mLevelUpgrader;
	u32 mInternalCounter;
};

#endif //_RTENGINE_HPP_