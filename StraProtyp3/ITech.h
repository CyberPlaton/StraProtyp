#pragma once

#include <string>
#include <vector>

#include "tinyxml2.h"

#include "IPlayer.h"

/*
* TechID is the unique name of a technology.
*/
using TechID = std::string;

/*
* Each tech has a TechSubcategory, for example
* the TechArea could be TA_CIVIC and TechSubcategory could be Economy 
* to indicate that the Technology is in the area of Economics.
*/
using TechSubcategory = std::string;

class TechInstance;

class ITech
{
public:
	enum TechArea
	{
		TA_MILITARY,
		TA_CIVICS,
		TA_MAGIC,
		TA_TECHNICAL
	};

	// For "checkWhetherAvailableForPlayer" function
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

	// For "checkWhetherAvailableForPlayer" function
	enum CheckType
	{
		// Integer
		CT_EQUAL,
		CT_GREATER,
		CT_SMALLER,

		// Boolean
		CT_TRUE,
		CT_FALSE
	};

public:

	static CheckArea getCheckAreaFromText(const std::string& text);
	static std::string getCheckAreaAsText(CheckArea area);
	static CheckType getCheckTypeFromText(const std::string& text);
	static std::string getCheckTypeAsText(CheckType type);
	static TechArea getTechAreaFromText(const std::string& text);
	static std::string getTechAreaAsText(TechArea area);
public:


	virtual TechID getID() const = 0;


	/*
	* Function checks whether given player has all requirements to be able
	* to researcg this technology.
	*/
	virtual bool checkWhetherAvailableForPlayer(IPlayer*) = 0;


	virtual TechArea getTechArea() = 0;
	virtual int getResearchPoints() = 0;
	virtual TechSubcategory getSubcategory() = 0;

	virtual float getBaseweight() = 0;
	virtual float getAccumulatedWeight(std::vector< TechInstance* >, IPlayer*) = 0;
};
