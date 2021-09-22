#include "ITech.h"

ITech::CheckArea ITech::getCheckAreaFromText(const std::string& text)
{
	if (text.compare("player_start_biome_check") == 0)
	{
		return CA_BIOME;
	}
	else if (text.compare("player_race_check") == 0)
	{
		return CA_RACE;
	}
	else if (text.compare("player_pop_check") == 0)
	{
		return CA_POP;
	}
	else if (text.compare("player_fort_check") == 0)
	{
		return CA_FORT;
	}
	else if (text.compare("player_city_check") == 0)
	{
		return CA_CITY;
	}
	else if (text.compare("player_building_check") == 0)
	{
		return CA_BUILDING;
	}
	else if (text.compare("player_unit_check") == 0)
	{
		return CA_UNIT;
	}
	else if (text.compare("player_tech_check") == 0)
	{
		return CA_TECH;
	}
	else
	{
	}
}

ITech::CheckType ITech::getCheckTypeFromText(const std::string& text)
{
	if (text.compare("greater") == 0)
	{
		return CT_GREATER;
	}
	else if (text.compare("smaller") == 0)
	{
		return CT_SMALLER;
	}
	else if (text.compare("equal") == 0)
	{
		return CT_EQUAL;
	}
	else if (text.compare("true") == 0)
	{
		return CT_TRUE;
	}
	else if (text.compare("false") == 0)
	{
		return CT_FALSE;
	}
}

ITech::TechArea ITech::getTechAreaFromText(const std::string& text)
{
	if (text.compare("military") == 0)
	{
		return TA_MILITARY;
	}
	else if (text.compare("civic") == 0)
	{
		return TA_CIVIC;
	}
	else if (text.compare("magick") == 0)
	{
		return TA_MAGIC;
	}
	else if (text.compare("technical") == 0)
	{
		return TA_TECHNICAL;
	}
}
