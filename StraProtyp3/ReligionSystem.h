#pragma once

#include "GameObjectStorage.h" // Include all Components and GameObject2 Definitions.
#include "Random.h" // Random number generator.


using GameworldMatrix = std::vector< std::vector< Pointer<GameObject2> >>;


class ReligionSystem
{
	struct ReligionColor
	{
		ReligionColor() : a(0), r(0), g(0), b(0)
		{
		}
		ReligionColor(int r, int g, int b, int a) : a(a), r(r), g(g), b(b)
		{
		}
		~ReligionColor() = default;

		int a, r, g, b;
	};


public:
	static ReligionSystem* get();
	static void del();


	void UpdateReligions(GameworldMatrix& world);
	void CreateReligion(Pointer<GameObject2> city, ReligionID religion);
	

	void SetReligionColor(std::string religion, int r, int g, int b, int a)
	{
		m_ReligionColors[religion].a = a; m_ReligionColors[religion].r = r; m_ReligionColors[religion].g = g; m_ReligionColors[religion].b = b;
	}

	ReligionColor GetReligionColor(std::string religion) { return m_ReligionColors[religion]; }


private:
	static ReligionSystem* g_ReligionSystem;

	std::map< std::string, ReligionColor > m_ReligionColors;

private:

	ReligionSystem()
	{
	}

	float _manhattenDistance(int x, int y, int xx, int yy);
	void _updateReligionPressure(Pointer<GameObject2> city);
	void _normalizeReligionValues(Pointer<GameObject2> city);
	void _updateMajorReligion(Pointer<GameObject2> city);
	std::vector< Pointer<GameObject2> > _getMaptilesWithinRange(int x, int y, GameworldMatrix& world, float range);
	Pointer<GameObject2> _getCityInMaptile(Pointer<GameObject2> maptile);

};