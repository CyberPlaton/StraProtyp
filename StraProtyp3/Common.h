#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>


#define DEFAULT_DECAL_SIZE_X 256
#define DEFAULT_DECAL_SIZE_Y 256
#define DEFAULT_MAPSIZE_X 30
#define DEFAULT_MAPSIZE_Y 20
#define DEFAULT_RELIGION_SPREAD_DISTANCE 20.0f


using NationID = std::string;
using NationCharacteristic = std::string;
using RaceID = std::string;
using TechID = std::string;
using TechArea = std::string;
using TechSubcategory = std::string;
using RessourceID = std::string;
using CityType = std::string;
using BuildingSlotType = std::string;
using CityFortificationLevel = std::string;
using ReligionID = std::string;
using UnitProfession = std::string;
using MaptileType = std::string;
using GOTag = std::string;
using GOName = std::string;
using ForestType = std::string;
using ForestBiome = std::string;



template < class A, class B >
struct Tuple
{
	Tuple(A x, B y) :x(x), y(y) {}
	Tuple() = default;
	~Tuple() = default;
	A x;
	B y;
};


//				   From x, and y    To x, and y,    Weight of Edge.
using edge = std::tuple<float, float, float, float, float>;
using node = std::tuple<float, float>;

#define node_x(n) std::get<0>(n)
#define node_y(n) std::get<1>(n)

#define edge_from_x(e) std::get<0>(e)
#define edge_from_y(e) std::get<1>(e)

#define edge_to_x(e) std::get<2>(e)
#define edge_to_y(e) std::get<3>(e)

#define edge_weight(e) std::get<4>(e)



struct Graph
{
	Graph(int width, int height)
	{
		nodes.resize(width);

		for (int i = 0; i < width; i++)
		{
			nodes[i].resize(height);
		}

		for (int i = 0; i < width; i++)
		{
			for (int j = 0; j < height; j++)
			{
				nodes[i][j] = 0; // No nodes in graph.
			}
		}
	}

	~Graph()
	{
		nodes.clear();
		edges.clear();
	}


	/*
	* Get weight of edge [a][aa]-----[b][bb].
	*							  ^
	*							  |
	*						edge --
	*/
	float getWeight(node from, node to)
	{
		return getWeight(node_x(from), node_y(from), node_x(to), node_y(to));
	}

	float getWeight(int a, int aa, int b, int bb)
	{
		for (auto& e : edges)
		{
			int x = edge_from_x(e);
			int xx = edge_from_y(e);
			int y = edge_to_x(e);
			int yy = edge_to_y(e);

			int w = edge_weight(e);

			// Edges are undirected. Thus check for both cases.
			if (a == x && aa == xx && y == b && yy == bb ||
				b == x && bb == xx && y == a && yy == aa) return w;
		}

		return INT_MAX;
	}


	/*
	* Set the weight of an edge.
	*/
	void setWeight(node x, node y, float w)
	{
		setWeight(node_x(x), node_y(x), node_x(y), node_y(y), w);
	}

	void setWeight(int a, int aa, int b, int bb, float w)
	{
		int index = 0;
		for (auto& e : edges)
		{
			int x = edge_from_x(e);
			int xx = edge_from_y(e);
			int y = edge_to_x(e);
			int yy = edge_to_y(e);


			if (a == x && aa == xx && y == b && yy == bb ||
				b == x && bb == xx && y == a && yy == aa)
			{
				index++;

				edges[index] = std::make_tuple(x, xx, y, yy, w);
				return;
			}

			index++;
		}
	}


	std::vector<node> getNeighbors(node n)
	{
		return getNeighbors(node_x(n), node_y(n));
	}

	std::vector<node> getNeighbors(int x, int y)
	{
		std::vector<std::tuple<float, float>> neighbors;

		for (auto& e : edges)
		{
			int fromx = edge_from_x(e);
			int fromy = edge_from_y(e);
			int tox = edge_to_x(e);
			int toy = edge_to_y(e);

			if (tox == x && toy == y)
			{
				neighbors.push_back({ fromx, fromy });
			}

			if (fromx == x && fromy == y)
			{
				neighbors.push_back({ tox, toy });
			}
		}

		return neighbors;
	}


	void addNode(int x, int y)
	{
		nodes[x][y] = 1;
	}

	void deleteNode(int x, int y)
	{
		nodes[x][y] = 0;
	}

	std::vector<std::vector<float>> nodes;
	std::vector< edge > edges;
};