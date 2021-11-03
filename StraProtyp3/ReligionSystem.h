#pragma once

#include "GameObjectStorage.h" // Include all Components and GameObject2 Definitions.
#include "Random.h" // Random number generator.


using GameworldMatrix = std::vector< std::vector< Pointer<GameObject2> >>;


class ReligionSystem
{
public:
	static ReligionSystem* get();
	static void del();


	void UpdateReligions(GameworldMatrix& world);
	void CreateReligion(Pointer<GameObject2> city, ReligionID religion);


private:
	static ReligionSystem* g_ReligionSystem;

	ReligionSystem()
	{
	}

private:


	float _manhattenDistance(int x, int y, int xx, int yy);
	void _updateReligionPressure(Pointer<GameObject2> city);
	void _normalizeReligionValues(Pointer<GameObject2> city);
	void _updateMajorReligion(Pointer<GameObject2> city);
	std::vector< Pointer<GameObject2> > _getMaptilesWithinRange(int x, int y, GameworldMatrix& world, float range);
	Pointer<GameObject2> _getCityInMaptile(Pointer<GameObject2> maptile);

};