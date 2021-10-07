#include "IMaptile.h"

std::string IMaptileCmp::getMaptileTypeAsString(IMaptileCmp::MType type)
{
	switch (type)
	{
	case MType::MT_JUNGLE:
		return "jungle";
	case MType::MT_ICE:
		return "ice";
	case MType::MT_SAND:
		return "sand";
	case MType::MT_SAVANNAH:
		return "savannah";
	case MType::MT_SNOW:
		return "snow";
	case MType::MT_TEMPERATE:
		return "temperate";
	case MType::MT_TUNDRA:
		return "tundra";
	case MType::MT_WATER_DEEP:
		return "water_deep";
	case MType::MT_WATER_SHALLOW:
		return "water_shallow";
	default:
		return "undefined";
	}
}

IMaptileCmp::MType IMaptileCmp::getMaptileTypeFromString(const std::string& type)
{
	if (type.compare("jungle") == 0)
	{
		return MType::MT_JUNGLE;
	}
	else if(type.compare("temperate") == 0)
	{
		return MType::MT_TEMPERATE;
	}
	else if (type.compare("tundra") == 0)
	{
		return MType::MT_TUNDRA;
	}
	else if (type.compare("sand") == 0)
	{
		return MType::MT_SAND;
	}
	else if (type.compare("savannah") == 0)
	{
		return MType::MT_SAVANNAH;
	}
	else if (type.compare("ice") == 0)
	{
		return MType::MT_ICE;
	}
	else if (type.compare("snow") == 0)
	{
		return MType::MT_SNOW;
	}
	else if (type.compare("water_deep") == 0)
	{
		return MType::MT_WATER_DEEP;
	}
	else if (type.compare("water_shallow") == 0)
	{
		return MType::MT_WATER_SHALLOW;
	}
}

