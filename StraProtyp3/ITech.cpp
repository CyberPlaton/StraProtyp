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
	else if (text.compare("civics") == 0)
	{
		return TA_CIVICS;
	}
	else if (text.compare("magick") == 0)
	{
		return TA_MAGIC;
	}
	else if (text.compare("technical") == 0)
	{
		return TA_TECHNICAL;
	}
	else if (text.compare("basic") == 0)
	{
		return TA_BASIC;
	}
}


std::string ITech::getCheckAreaAsText(ITech::CheckArea area)
{
	switch (area)
	{
	case CA_BIOME:
		return "player_start_biome_check";
	case CA_RACE:
		return "player_race_check";
	case CA_POP:
		return "player_pop_check";
	case CA_FORT:
		return "player_fort_check";
	case CA_CITY:
		return "player_city_check";
	case CA_BUILDING:
		return "player_building_check";
	case CA_UNIT:
		return "player_unit_check";
	case CA_TECH:
		return "player_tech_check";
	default:
		return "undefined";
	}
}



std::string ITech::getCheckTypeAsText(ITech::CheckType type)
{
	switch (type)
	{
	case CT_EQUAL:
		return "equal";
	case CT_GREATER:
		return "greater";
	case CT_SMALLER:
		return "smaller";
	case CT_TRUE:
		return "true";
	case CT_FALSE:
		return "false";
	default:
		return "undefined";
	}
}


std::string ITech::getTechAreaAsText(TechArea area)
{
	switch (area)
	{
	case TA_CIVICS:
		return "civics";
	case TA_MAGIC:
		return "magick";
	case TA_MILITARY:
		return "military";
	case TA_TECHNICAL:
		return "technical";
	case TA_BASIC:
		return "basic";
	default:
		return "undefined";
	}
}