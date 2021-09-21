#pragma once

#include <string>
#include "tinyxml2.h"

#include "IPlayer.h"

/*
* TechID is the unique name of a technology.
*/
using TechID = std::string;
class ITech
{
public:
	enum TechArea
	{
		TA_MILITARY,
		TA_CIVIC,
		TA_MAGIC,
	};

	enum CheckArea
	{
		CA_BIOME,
		CA_RACE,
		CA_POP,
		CA_FORT,
		CA_CITY,
		CA_BUILDING,
		CA_UNIT,
		CA_TECH
	};


	enum CheckType
	{
		CT_EQUAL,
		CT_GREATER,
		CT_SMALLER,
		CT_TRUE,
		CT_FALSE
	};

public:

	static CheckArea getCheckAreaFromText(tinyxml2::XMLElement* elem);
	static CheckType getCheckTypeFromText(tinyxml2::XMLElement* elem);
	static TechArea getTechAreaFromText(tinyxml2::XMLElement* elem);

public:


	virtual TechID getID() const = 0;


	/*
	* Function checks whether given player has all requirements to be able
	* to researcg this technology.
	*/
	virtual bool checkWhetherAvailableForPlayer(IPlayer*) = 0;


	virtual TechArea getTechArea() = 0;
	virtual int getResearchPoints() = 0;
	virtual CheckArea getCheckArea() = 0;
	virtual CheckType getCheckType() = 0;
};
