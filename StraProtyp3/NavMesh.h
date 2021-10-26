#pragma once

#include "Component2.h" // Include for extending the Component2 class
#include "GameObject2.h"
#include "IGameObjectStorage.h" // Include for GameobjectStorage Interface, mainly for Instantiation, Finding and Deleting functions
#include "Common.h" // Include Common Definitions and Datastructs.

#include "ColorConsole.h"


#include "TransformComponent.h"
#include "CollisionBoxComponent.h"


class NavMesh
{
public:

	static NavMesh* get()
	{
		if (!g_NavMesh)
		{
			g_NavMesh = new NavMesh(DEFAULT_MAPSIZE_X, DEFAULT_MAPSIZE_Y);
		}

		return g_NavMesh;
	}

	static void del()
	{
		if (g_NavMesh)
		{
			g_NavMesh->graph.reset();
			g_NavMesh->map.clear();
			g_NavMesh->width = 0;
			g_NavMesh->height = 0;
			delete g_NavMesh;
		}
	}


	bool Bake();
	std::vector<std::vector<float>> GetNavMesh() { return map; }
	std::vector<std::vector<float>> GetNavGraph() { return graph->nodes; }
	Pointer<Graph> GetGraph() { return graph; }
	int GetWidth() { return width; }
	int GetHeight() { return height; }


private:

	NavMesh(int width, int height) : width(width), height(height)
	{
		map.resize(width);

		for (int i = 0; i < width; i++)
		{
			map[i].resize(height);
		}

		for (int i = 0; i < width; i++)
		{
			for (int j = 0; j < height; j++)
			{
				map[i][j] = 1;
			}
		}
	}



	static NavMesh* g_NavMesh;

	std::vector<std::vector<float>> map;

	Pointer<Graph> graph;

	int width, height;
};