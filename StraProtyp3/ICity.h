#pragma once

#include <vector>
#include <map>
#include <cmath>

#include "ColorConsole.h"

#include "ComponentSystem.h"
#include "IPlayer.h"
#include "ICity.h"
#include "IMaptile.h"


using RessourceID = std::string;

/*
* Definition of the city type, which kind it is "City" or "Fort",
* on which maptile it is, whether it is on a Forest or Hill and
* whether it has access to water. Available are
* 
* "city_plain_temperate" - city on basic temperate plains,
* "city_forest_temperate" - city on forest and temperate maptile,
* "city_hill_temperate" - city on hill and temperate maptile,
* "city_plain_port_temperate" - city on basic temperate plain with water access,
* "city_forest_port_temperate" - city on forest and temperate maptile with water access,
* "city_hill_port_temperate" - city on hill and temperate maptile with water access,
* "city_plain_river_temperate" - city on basic temperate plain on river,
* "city_forest_river_temperate" - city on forest and temperate maptile and on river,
* "city_hill_river_temperate" - city on a hill and temperate maptile and on river,
* "city_plain_river_port_temperate" - city on basic temperate plains and river with water access,
* "city_forest_river_port_temperate" - city on forest and temperate maptile with river and water access,
* "city_hill_river_port_temperate" - city on hill and temperate maptile with river and water access,
* 
* ...
* Analog for the other maptile types tundra, snow, savannah and jungle.
* 
*/
using CityType = std::string;


/*
* Definition of which Type a Building Slot is.
* In the city there is a certain number of each slot type.
* 
* We define "BuildingSlotType" as 
* "Normal" - for default buildings in the city,
* "Special" - for special buildings around city, which depend on race, technology etc.
* "Castle" - for buildings in the cities castle area, which are available depending on race and technology,
* "Port" - for buildings which are for Port-Cities and have access to water, which are available depending on technology.
*/
using BuildingSlotType = std::string;

/*
* Definition of the type of the cities Walls if there are any.
* These can be 
* "Earth Wall" - for basic earthen wall around city, which provides minimal defensive bonus to defenders,
* "Wooden Wall" - for more elaborate protection, with more defense bonus and size of the available Garrison,
* "Stone Wall" - for even more Garrison capacity and increased defense bonus,
* "Brick Wall" - for greatest Garrison capacity and defense bonus.
*/
using CityFortificationLevel = std::string;

/*
* Adding this to a Gameobject makes it a city.
*/
class ICityCmp : public Component
{
public:

	/*
	* Defines a slot in the city where a building is drawn.
	*/
	struct BuildingSlot
	{
		BuildingSlot(float x, float y, int n, BuildingSlotType type)
		: xpos(x), ypos(y), slotNumber(n), slotType(type)
		{

		}

		float xpos;
		float ypos;
		int slotNumber;
		BuildingSlotType slotType;

		bool isUsed = false;
		GameObject* building = nullptr;
	};



public:
	ICityCmp(const ComponentID& name, GameObject* cityGO) : cityGameobject(cityGO) {
		this->name = name;
		type = "City";

		init(type);
	}


	ComponentType getType() override { return this->type; }

	void pushSlot(const BuildingSlot& slot) { buildingSlots.push_back(slot); }
	void setCityName(const std::string& name) { cityName = name; }

	// Using a templated Vector for getting the position of the city.
	template < typename T >
	T getCityPosition()
	{
		TransformCmp* tr = cityGameobject->getComponent<TransformCmp>("Transform");
		return { tr->xpos, tr->ypos };
	}


	std::string getMajorReligion() { return majorReligion; }
	double getMajorReligionStrength() { return religions[majorReligion]; }

	void createReligion(const std::string& religion, double value)
	{
		religions[religion] = value;
		_updateMajorReligion();
	}

	std::map< std::string, float > getReligions() { return religions; }

	void updateReligion(std::vector<std::vector<GameObject*>>& world)
	{
		using namespace std;

		// Apply accomodated pressure values from previous turn.
		_updatePressureValues();


		// Base decay for all religions.
		/*
		for (auto& r : religions)
		{
			r.second -= 0.2f;
			if (r.second < 0.0) r.second = 0.0;
		}
		*/

		// Normalize religion values to be summed to 100.
		// This possibly makes the base decay obsolete.
		_normalizeReligionValues();


		// Update the major religion for the city.
		_updateMajorReligion();


		// Update religious pressure on all neighboring cities,
		// where "neighboring" means with manhatten distance lower than DEFAULT_RELIGION_SPREAD_DISTANCE.
		std::vector< GameObject* > maptiles = _getMaptilesWithinRange(world, DEFAULT_RELIGION_SPREAD_DISTANCE);


		// For each maptile in range with a city.
		TransformCmp* cityTransform = cityGameobject->getComponent<TransformCmp>("Transform");
		for (auto& m : maptiles)
		{
			IMaptileCmp* maptileComponent = m->getComponent<IMaptileCmp>("Maptile");
			TransformCmp* maptileTransform = m->getComponent<TransformCmp>("Transform");
			if (maptileComponent->hasCity())
			{

				double majorReligionValue = religions[majorReligion];


				// Compute the distance between this city and other.
				double dist = _manhattenDistance(cityTransform->xpos, cityTransform->ypos, maptileTransform->xpos, maptileTransform->ypos);
				

				// Allow for the city to influence itself by default,
				// to disallow it change "0.0" to "1.0".
				if (dist <= 0.0) continue;


				// Compute pressure value of major religion for other city.
				double pressureValue = majorReligionValue / (dist * dist * dist);


				if (pressureValue >= 100.0) continue;


				ICityCmp* city = _getCityOfMaptile(maptileComponent);

				// Add the pressure value for the city to be update in next religious update.
				city->addPressureValue(majorReligion, pressureValue);


				//cout << color(colors::GREEN);
				//cout << "[" << getCityName() << "]->[" << city->getCityName() << "] Add ReligionPressure \"" << majorReligion << "\":" << pressureValue << endl;
				//cout << "[" << getCityName() << "]->[" << city->getCityName() << "] ManhattenDistance:" << dist << white << endl << endl;

			}
		}
	}


	void addPressureValue(const std::string& religion, double value)
	{
		// Add or set.
		religionPressureValue[religion] = value;
	}



	/*
	* TODO
	* Define the position of the Unit in the city by altering its Transform
	* according to profession etc.
	*/
	void addUnit(GameObject* unit)
	{ 
		units.push_back(unit); 
		unit->getComponent<IUnitCmp>("Unit")->setInCity(true);
	}


	void setData(const std::string& entry, int amount)
	{
		data[entry] = amount;
	}

	void setMaxStorage(int n) { maxStorage = n; }

	void setCityType(const std::string& type, const std::string& cityType, bool forest = false, bool hill = false, bool river = false, bool port = false)
	{
		using namespace std;

		std::string cType = cityType;

		// Set city as forest or hill city.
		// They are mutualy exclusive.
		if (forest)
		{
			cType += "_forest";
		}
		else if (hill)
		{
			cType += "_hill";
		}

		// Check whether city is on river
		if (river)
		{
			cType += "_river";
		}

		// Check whether city has water access
		if (port)
		{
			cType += "_port";
		}


		// Set correct underlying maptile type
		if (type.compare("temperate") == 0)
		{
			cType += "_temperate";
		}
		else if (type.compare("snow") == 0)
		{
			cType += "_snow";

		}
		else if (type.compare("tundra") == 0)
		{
			cType += "_tundra";

		}
		else if (type.compare("savannah") == 0)
		{
			cType += "_savannah";

		}
		else if (type.compare("jungle") == 0)
		{
			cType += "_jungle";

		}
		else
		{
			cout << color(colors::RED);
			cout << "Undefined Maptile \""<< type << "\", defining as default temperate..." << white << endl;
			cType += "temperate";
		}

		this->cityType = cType;
	}



	void setCityFortificationLevel(const CityFortificationLevel& level) { fortificationLevel = level; }

	void setPlayer(IPlayer* p) { player = p; }


	/*
	* Set the building in a slot.
	*/
	bool assignBuildingToSlot(GameObject* go, int slot, BuildingSlotType s)
	{
		if (!buildingSlots[slot - 1].isUsed && 
			buildingSlots[slot - 1].slotNumber == slot &&
			buildingSlots[slot - 1].slotType.compare(s) == 0) // Sanity Checks.
		{
			buildingSlots[slot - 1].building = go;
			buildingSlots[slot - 1].isUsed = true;


			buildings.push_back(go);

			// Set the Position of the Gameobject 
			// According to Slot.
			TransformCmp* cmp = go->getComponent<TransformCmp>("Transform");
			cmp->xpos = buildingSlots[slot - 1].xpos;
			cmp->ypos = buildingSlots[slot - 1].ypos;

			return true;
		}

		return false;
	}



	void increaseRessource(const RessourceID& name, int amount)
	{
		cityRessources[name] += amount;
		if (cityRessources[name] > 100) cityRessources[name] = 100;
	}

	void decreaseRessource(const RessourceID& name, int amount)
	{
		cityRessources[name] -= amount;
		if (cityRessources[name] < 0) cityRessources[name] = 0;
	}

	bool hasRessourceAmount(const RessourceID& name, int amount)
	{
		return cityRessources[name] >= amount;
	}

	CityFortificationLevel getFortificationLevel() { return fortificationLevel; }
	CityType getCityType() { return cityType; }
	std::string getCityName() { return cityName; }

	std::vector< GameObject* > getUnits() { return units; }
	std::vector< GameObject* > getBuildings(){ return buildings; }
	std::map< RessourceID, int > getCityRessources() { return cityRessources; }
	std::map< std::string, int > getCityData() { return data; }
	std::vector< BuildingSlot > getCityBuildingSlots() { return buildingSlots; }


private:
	std::string type;

	// To which player this city belongs.
	IPlayer* player = nullptr;

	std::string cityName;

	// Whether a city or fort and
	// whether on a Plain, Forest, Hill etc.
	CityType cityType;

	CityFortificationLevel fortificationLevel;

	// Cities Building slots.
	std::vector< BuildingSlot > buildingSlots;


	// Stores the ressources of the city.
	std::map< RessourceID, int > cityRessources;


	// Max storage is defined for each ressource.
	int maxStorage = -INT_MAX;


	// The units and buildings IN the city.
	// Buildings are added on creation and removed on destruction and
	// units are added on entering the city and removed on exiting the city.
	// Thus if the player is watching a city, only what is in it is being drawn,
	// plus the surrounding lands...
	std::vector< GameObject* > units;
	std::vector< GameObject* > buildings;


	// Central data storage for stuff like "Happiness" etc.
	std::map< std::string, int > data;

	// Religions of the city.
	std::map< std::string, float > religions;
	std::map< std::string, double > religionPressureValue;
	std::string majorReligion; // Current Hightest Value religion.

	// The Gameobject representing the city.
	GameObject* cityGameobject = nullptr;



private:

	std::vector< GameObject* > _getMaptilesWithinRange(std::vector<std::vector<GameObject*>>& world, double dist)
	{
		std::vector< GameObject* > ret;

		TransformCmp* transform = cityGameobject->getComponent<TransformCmp>("Transform");

		for (int x = 0; x < world.size(); x++)
		{
			for (int y = 0; y < world[x].size(); y++)
			{

				TransformCmp* maptileTransform = world[x][y]->getComponent<TransformCmp>("Transform");

				double manDist = std::abs(transform->xpos - maptileTransform->xpos) + std::abs(transform->ypos - maptileTransform->ypos);

				if (manDist <= dist)
				{
					ret.push_back(world[x][y]);
				}

			}
		}



		return ret;
	}

	// Go through all religions and set the highest value one as new major religion for the city.
	void _updateMajorReligion()
	{
		double max = 0.0;
		for (auto& r : religions)
		{
			if (r.second > max)
			{
				max = r.second;
				majorReligion = r.first;
			}
		}
	}


	void _updatePressureValues()
	{
		using namespace std;

		if (religionPressureValue.size() == 0) return;

		/*
		// Compute sum value of all religion pressures.
		double sum = 0.0;
		sum += religions[majorReligion];
		for (auto& d :religionPressureValue)
		{
			sum += d.second;
		}


		// Increase religion value of pressurized religions based on percentage value.
		for (auto& r : religionPressureValue)
		{
			// Compute percentage value.
			double p = (r.second * 100.0) / sum;

			// Increase religion value by precentage.
			religions[r.first] += (float)p;



			cout << color(colors::MAGENTA);
			cout << "["<< getCityName() << "] Religion Surplus: \"" << r.first << "\" : " << r.second << ", value: "<< religions[r.first] << white << endl;
		}
		*/

		for (auto& r : religionPressureValue)
		{

			// Increase religion value by precentage.
			religions[r.first] += (float)r.second;

			//cout << color(colors::MAGENTA);
			//cout << "[" << getCityName() << "] Religion Surplus: \"" << r.first << "\":" << r.second << ", ReligionValue: " << religions[r.first] << white << endl;
		}

		// Reset the pressure values.
		for (auto& it : religionPressureValue)
		{
			it.second = 0.0;
		}
	}

	// Normalize the religion values to be percentage based and between 0 and 100.
	void _normalizeReligionValues()
	{
		float sum = 0.0;
		for (auto& r : religions)
		{
			sum += r.second;
		}

		std::map< std::string, float > percentageValues;
		for (auto& r : religions)
		{
			float percentValue = (r.second * 100.0f) / sum;
			percentageValues.emplace(r.first,  percentValue);
		}

		for (auto& pv : percentageValues)
		{
			religions[pv.first] = pv.second;
		}
	}


	double _manhattenDistance(int x1, int y1, int x2, int y2)
	{
		return std::abs(x1 - x2) + std::abs(y1 - y2);
	}


	ICityCmp* _getCityOfMaptile(IMaptileCmp* maptile)
	{
		for (auto& tag : maptile->getGameobjects())
		{
			GameObject* go = GameObjectStorage::get()->getGOByTag(tag);
			if (go->hasComponent("City"))
			{
				return go->getComponent<ICityCmp>("City");
			}
		}

		return nullptr;
	}
};