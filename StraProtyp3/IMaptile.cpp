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
