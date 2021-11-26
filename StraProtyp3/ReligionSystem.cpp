#include "ReligionSystem.h"

ReligionSystem* ReligionSystem::g_ReligionSystem = nullptr;


void ReligionSystem::CreateReligion(Pointer<GameObject2> city, ReligionID religion)
{
	Pointer<CityComponent> c = city->getComponent<CityComponent>("City");

	c->AddReligion(religion, 100.0f);
}


void ReligionSystem::UpdateReligions(GameworldMatrix& world)
{
	for (int i = 0; i < world.size(); i++)
	{
		for (int j = 0; j < world[i].size(); j++)
		{
			Pointer<MaptileComponent> m = world[i][j]->getComponent<MaptileComponent>("Maptile");

			if (m->HasCity())
			{
				// City which applies the pressure to other cities.
				Pointer<GameObject2> city = _getCityInMaptile(world[i][j]);
				Pointer<CityComponent> currentCity = city->getComponent<CityComponent>("City");
				
				// Apply accumulated pressure values from previous turn.
				_updateReligionPressure(city);


				// Normalize religion values.
				_normalizeReligionValues(city);


				// Update MajorReligion for city.
				_updateMajorReligion(city);


				// Update religious pressure on all neighboring cities for current city.
				std::vector< Pointer< GameObject2> > v = _getMaptilesWithinRange(i, j, world, DEFAULT_RELIGION_SPREAD_DISTANCE);
			

				for (auto e : v)
				{
					Pointer<MaptileComponent> m = e->getComponent<MaptileComponent>("Maptile");
					if (m->HasCity())
					{
						Pointer<GameObject2> c = _getCityInMaptile(e);
						Pointer<CityComponent> otherCity = c->getComponent<CityComponent>("City");
						Pointer<TransformComponent> otherCityTransform = c->getComponent<TransformComponent>("Transform");

						float majorReligionValue = currentCity->GetReligions()[currentCity->GetMajorReligion()];

						float dist = _manhattenDistance(i, j, otherCityTransform->GetXPos(), otherCityTransform->GetYPos());

						if (dist <= 0.0f) continue;

						float pressure = majorReligionValue / (dist * dist * dist);

						if (pressure >= 100.0f) continue;

						otherCity->AddPressureValue(currentCity->GetMajorReligion(), pressure);
					}


				}
			}
		}
	}
}


float ReligionSystem::_manhattenDistance(int x, int y, int xx, int yy)
{
	return std::abs(x - xx) + std::abs(y - yy);
}


void ReligionSystem::_updateReligionPressure(Pointer<GameObject2> city)
{
	Pointer<CityComponent> c = city->getComponent<CityComponent>("City");

	if (c->GetReligionPressureValues().size() == 0) return;


	for (auto& r : c->GetReligionPressureValues())
	{

		// Increase religion value by precentage.
		c->GetReligions()[r.first] += (float)r.second;
	}

	// Reset the pressure values.
	for (auto& it : c->GetReligionPressureValues())
	{
		it.second = 0.0;
	}
}


void ReligionSystem::_normalizeReligionValues(Pointer<GameObject2> city)
{
	Pointer<CityComponent> c = city->getComponent<CityComponent>("City");


	float sum = 0.0;
	for (auto& r : c->GetReligions())
	{
		sum += r.second;
	}

	std::map< std::string, float > percentageValues;
	for (auto& r : c->GetReligions())
	{
		float percentValue = (r.second * 100.0f) / sum;
		percentageValues.emplace(r.first, percentValue);
	}

	for (auto& pv : percentageValues)
	{
		c->GetReligions()[pv.first] = pv.second;
	}
}


void ReligionSystem::_updateMajorReligion(Pointer<GameObject2> city)
{
	Pointer<CityComponent> c = city->getComponent<CityComponent>("City");

	float max = 0.0;
	std::string majorReligion;
	for (auto& r : c->GetReligions())
	{
		if (r.second > max)
		{
			max = r.second;
			majorReligion = r.first;
		}
	}

	c->SetMajorReligion(majorReligion);
}


std::vector< Pointer<GameObject2> > ReligionSystem::_getMaptilesWithinRange(int x, int y, GameworldMatrix& world, float range)
{
	std::vector< Pointer<GameObject2> > ret;

	for (int i = 0; i < world.size(); i++)
	{
		for (int j = 0; j < world[i].size(); j++)
		{
			if (i == x && j == y) continue;

			float d = _manhattenDistance(x, y, i, j);

			if (d <= range)
			{
				ret.push_back(world[i][j]);
			}

		}
	}

	return ret;
}


Pointer<GameObject2> ReligionSystem::_getCityInMaptile(Pointer<GameObject2> maptile)
{
	Pointer<MaptileComponent> m = maptile->getComponent<MaptileComponent>("Maptile");

	for (auto e : m->GetGameobjects())
	{
		if (e->hasComponent(EComponentType::CT_City))
		{
			return e;
		}
	}

	return nullptr;
}





ReligionSystem* ReligionSystem::get()
{
	if (!g_ReligionSystem)
	{
		g_ReligionSystem = new ReligionSystem();
	}


	return g_ReligionSystem;
}


void ReligionSystem::del()
{
	if (g_ReligionSystem)
	{
		delete g_ReligionSystem;
	}
}