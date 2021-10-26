#include "NavMesh.h"

NavMesh* NavMesh::g_NavMesh = nullptr;

bool NavMesh::Bake()
{
	graph = std::make_shared<Graph>(width, height);
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			graph->addNode(i, j);
		}
	}


	for (auto& go : IGameobjectStorage::g_IGameobjectStorage->GetStorage())
	{
		if (go->getTag().find("Building") == std::string::npos) continue;

		// Start baking the Graph too.
		//
		if (go->hasComponent("CollisionBox"))
		{
			int x, y, w, h;

			auto transform = go->getComponent<TransformComponent>("Transform");
			auto coll = go->getComponent<CollisionBoxComponent>("CollisionBox");

			x = transform->xpos;
			y = transform->ypos;

			w = coll->width;
			h = coll->height;

			for (int i = x; i < x + w; i++)
			{
				for (int j = y; j < y + h; j++)
				{
					map[i][j] = INT_MAX;
					graph->deleteNode(i, j);
				}
			}
		}
		else
		{
		}
	}


	for (int i = 0; i < width - 1; i++)
	{
		for (int j = 0; j < height - 1; j++)
		{
			if (graph->nodes[i][j] == 1)
			{
				// We make exactly 2 edges for each node,
				// thus every node has 4 edges in total.
				if (graph->nodes[i + 1][j] == 1)
				{
					edge e = std::make_tuple(i, j, i + 1, j, 1);
					graph->edges.push_back(e);
				}
				if (graph->nodes[i][j + 1] == 1)
				{
					edge e2 = std::make_tuple(i, j, i, j + 1, 1);
					graph->edges.push_back(e2);
				}
			}
		}
	}



	return true;
}