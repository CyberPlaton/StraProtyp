#pragma once

#include "ColorConsole.h"

#include "GameObject.h"
#include "ComponentSystem.h"


#include <stack>
#include <queue>

class WalkableBuildingCmp;


class CollisionBoxCmp : public Component
{
public:
	CollisionBoxCmp(const ComponentID& name, float width, float height, GameObject* go) :
		width(width), height(height), this_agent(go)
	{
		this->name = name;
		type = "CollisionBox";

		init(type);
	}

	~CollisionBoxCmp()
	{
		this_agent = nullptr;
		type.clear();
		width = 0.0f;
		height = 0.0f;
	}

	ComponentType getType() override { return this->type; }

	/*
	* Resolve collision of owner Gameobjet and given one.
	* Collision is taking place if their dimensions are intersecting.
	*/
	bool resolve(GameObject* other);

	/*
	* Resolve collision between owner Gameobject and defined rectangle.
	* Collision is taking place if their dimensions are intersecting.
	*/
	bool resolve(float x, float y, float w, float h);


	GameObject* this_agent = nullptr;

	ComponentType type;

	float width = 0.0f;
	float height = 0.0f;
};




//					From x, y    To x, y,   Weight.
using edge = std::tuple<int, int, int, int, float>;
using node = std::tuple<int, int>;

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

		for (int i = 0; i < height; i++)
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
		std::vector<std::tuple<int, int>> neighbors;

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
				neighbors.push_back({tox, toy});
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

	std::vector<std::vector<int>> nodes;
	std::vector< edge > edges;
};


/*
* Not a component, but still very important.
*/
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
			delete g_NavMesh->graph;
			g_NavMesh->map.clear();
			g_NavMesh->width = 0;
			g_NavMesh->height = 0;
			delete g_NavMesh;
		}
	}


	bool bake();


	std::vector<std::vector<int>> getNavMesh() { return map; }
	std::vector<std::vector<int>> getNavGraph() { return graph->nodes; }
	Graph* getGraph() { return graph; }
	int getWidth() { return width; }
	int getHeight() { return height; }


private:

	NavMesh(int width, int height) : width(width), height(height)
	{
		map.resize(width);

		for (int i = 0; i < height; i++)
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

	std::vector<std::vector<int>> map;

	Graph* graph = nullptr;

	int width, height;
};



class NavigatorCmp : public Component
{
public:

	NavigatorCmp(const ComponentID& name, GameObject* go) : agent(go)
	{
		this->name = name;
		type = "Navigator";

		init(type);
	}


	ComponentType getType() override { return this->type; }


	/*
	* Check whether current set destination was reached.
	*/
	bool isDestinationReached()
	{
		TransformCmp* transform = agent->getComponent<TransformCmp>("Transform");
		
		return (transform->xpos == destx && transform->ypos == desty) == true ? true : false;
	}


	/*
	* Set the next waypoint/moving dest. for a gameobject.
	*/
	void setDestination(float x, float y)
	{
		destx = x;
		desty = y;
		movementPoints.clear(); // Reset the Pathfinder.
		curr_mp_index = 0;
		internal_agent_positionx = 0.0f; // Reset the internal agent position counter.
		internal_agent_positiony = 0.0f;
	}

	std::pair<int, int> getRandomDestinationAroundAgent(int radius)
	{
		using namespace std;

		int x = rand() % radius + 1;
		int y = rand() % radius + 1;

		while (!isTargetDestinationValid(x, y))
		{
			cout << color(colors::RED);
			cout << "[NavigatorCmp] Invalid Destination {"<< x << ","<< y << "}, searching for new one" << white << endl;

			x = rand() % radius + 1;
			y = rand() % radius + 1;
		}

		return std::make_pair(x, y);
	}

	bool isTargetDestinationValid(int x, int y)
	{
		if (x > NavMesh::get()->getWidth() ||
			y > NavMesh::get()->getHeight() ||
			x < 0 ||
			y < 0)
		{
			return false;
		}


		std::vector<std::vector<int>> v = NavMesh::get()->getNavGraph();

		for (int i = 0; i < v.size(); i++)
		{
			for (int j = 0; j < v[i].size(); j++)
			{
				if (x == i || y == j)
				{
					if (v[i][j] != 1)
					{
						return false;
					}
				}
			}
		}

		return true;
	}

	/*
	* General update function.
	* Returns true if destination was reached.
	*/
	bool update(float dt)
	{
		using namespace std;

		TransformCmp* t = agent->getComponent< TransformCmp >("Transform");
		if (t)
		{
			// Is Target reached
			if (t->xpos == destx && t->ypos == desty)
			{
				// Reset internal data and return indication that destination was reached.
				movementPoints.clear();
				curr_mp_index = 0;
				internal_agent_positionx = 0.0f; // Reset the internal agent position counter.
				internal_agent_positiony = 0.0f;
				return true;
			}


			if (movementPoints.size() == 0)
			{
				// Get a path from current position to given destination.
				movementPoints = this->_getPathToDest(t->xpos, t->ypos, destx, desty, NavMesh::get()->getGraph());
			}
			
			if (movementPoints.size() > 0)
			{
				// Internally update the movement of the agent.
				_updateWaypointMovement(dt, t);
			}


			// Component update, if needed.
			if (internal_agent_positionx > 1.0f)
			{
				t->xpos += 1;
				internal_agent_positionx = 0.0f;
			}
			else if (internal_agent_positionx < -1.0f)
			{
				t->xpos -= 1;
				internal_agent_positionx = 0.0f;
			}


			if (internal_agent_positiony > 1.0f)
			{
				t->ypos += 1;
				internal_agent_positiony = 0.0f;
			}
			else if (internal_agent_positiony < -1.0f)
			{
				t->ypos -= 1;
				internal_agent_positiony = 0.0f;
			}
		}



		return false;
	}


	GameObject* agent = nullptr;

	ComponentType type;

	float destx = 0.0f;
	float desty = 0.0f;

	/*
	* Used to count in floats the agents position,
	* and then update the actuall transform component.
	*/
	float internal_agent_positionx = 0.0f;
	float internal_agent_positiony = 0.0f;


	std::vector<node> movementPoints;
	int curr_mp_index = 0;
	float curr_mpx;
	float curr_mpy;


	bool _updateWaypointMovement(float dt, TransformCmp* t)
	{
		// Get next waypoint.
		curr_mpx = std::get<0>(movementPoints[curr_mp_index]);
		curr_mpy = std::get<1>(movementPoints[curr_mp_index]);

		// Move to waypoint.
		// In movement to waypoint we do not have to check for obstacles.
		if (t->xpos < curr_mpx)
		{
			internal_agent_positionx += dt;
		}

		else if (t->xpos > curr_mpx)
		{
			internal_agent_positionx -= dt;
		}


		if (t->ypos < curr_mpy)
		{
			internal_agent_positiony += dt;
		}
		else if (t->ypos > curr_mpy)
		{
			internal_agent_positiony -= dt;
		}

		// Check whether waypoint reached and needs to be updated.
		if (t->xpos == curr_mpx && t->ypos == curr_mpy)
		{
			curr_mp_index++;
		}

		return true;
	}


	/*
	* Implementing the A*-Algorithm.
	*/
	std::vector<node> _getPathToDest(int startx, int starty, int x, int y, Graph* graph)
	{
		using namespace std;
		bool path_found = false;
		std::vector<node> path;

		// Distance between some node and starting node.
		std::map<node, float> g;

		// Heuristic, estimated distance from some node to end point.
		std::map<node, float> h;

		// Total cost for some node.
		std::map<node, float> f;

		// Parent node for some node, meaning, it was visited before visiting the accessed node.
		std::map<node, node> parent_map;

		// Initialization
		std::vector<node> open_list;
		open_list.push_back({ startx, starty });
		f[{startx, starty}] = 0.0f;
		
		std::vector<node> closed_list;
		std::vector<node> nearest_node_neighbors;



		while (!open_list.empty())
		{
			// Get current node.
			node nearest = _getNodeWithLowestCost(open_list, f);
			_removeNode(nearest, open_list);
			closed_list.push_back(nearest);


			if (_isNodeDestinationNode(nearest, { x, y }))
			{

				path_found = true;
				break;
			}
			else
			{
				// Get the neighbors for current node.
				nearest_node_neighbors = graph->getNeighbors(nearest);


				// Examine neighbors of current node.
				for (auto& n : nearest_node_neighbors)
				{
					// Continue if n is in closed, meaning was visited already.
					if (_nodeInList(n, closed_list)) continue;


					// Make "current" node parent of "this" node
					parent_map[n] = nearest;


					// Compute f, g, h value for node.
					g[n] = g[n] + _manhattenDistance(nearest, n);
					h[n] = _manhattenDistance(n, { x, y });
					f[n] = g[n] + h[n];



					if (_nodeInList(n, open_list))
					{
						for (auto& node : open_list)
						{
							if (g[n] > g[node]) continue;
						}
					}


					open_list.push_back(n);
				}
			}
		}


		std::vector<node> temp;

		if (!path_found)
		{
			path.clear();
		}
		else
		{
			// Reconstruct path.
			node child_node = node(x, y); // Initial node.
			temp.push_back(child_node);

			while (true)
			{
				try
				{
					node parent_of_node = parent_map.at(child_node);

					temp.push_back(parent_of_node);

					child_node = parent_of_node;
				}
				catch (...)
				{
					break;
				}
			}

			// Reverse path to be correct.
			for (int i = temp.size() - 1; i >= 0; i--)
			{
				path.push_back(temp[i]);
			}

			//path = closed_list;
		}


		return path;
	}

	bool _nodeInList(node n, std::vector<node>& v)
	{
		std::vector<node>::iterator it = std::find(v.begin(), v.end(), n);
		if (it != v.end())
		{
			return true;
		}

		return false;
	}

	void _removeNode(node n, std::vector<node>& v)
	{
		std::vector<node>::iterator it = std::find(v.begin(), v.end(), n);
		if (it != v.end())
		{
			v.erase(it);
		}
	}


	/*
	* Check whether given node is our destination node.
	*/
	bool _isNodeDestinationNode(node x, node dest)
	{
		if (node_x(x) == node_x(dest) &&
			node_y(x) == node_y(dest))
		{
			return true;
		}

		return false;
	}

	/*
	* Returns the node with smallest distance to destination.
	*/
	node _getNodeWithLowestCost(std::vector<node>& v, std::map<node, float>& map)
	{
		float min = (float)INT_MAX;
		node node(-1, -1);

		for (auto& n : v)
		{
			float d = map[n];

			if (d < min)
			{
				min = d;
				node = n;
			}
		}

		return node;
	}


	/*
	* Compute the manhatten distance from node x to node y.
	*/
	float _manhattenDistance(node x, node y)
	{
		float xx, xy, yx, yy;
		xx = node_x(x);
		xy = node_y(x);
		yx = node_x(y);
		yy = node_y(y);

		// Vectorial 2D-Distance.
		return abs(xx - yx) + abs(xy - yy);
	}

};


class AnimatorCmp : public Component
{
public:
	enum Animations
	{
		ANIM_NONE,
		ANIM_IDLE,
		ANIM_SLEEP,
		ANIM_WALK,
		ANIM_DRINK,
		ANIM_EAT
	};
public:
	AnimatorCmp(const ComponentID& name, GameObject* go) : agent(go)
	{
		this->name = name;
		type = "Animator";

		init(type);

		currentAnimation = Animations::ANIM_NONE;
	}


	ComponentType getType() override { return this->type; }


	Animations getAnimation()
	{
		return currentAnimation;
	}


	bool setAnimation(Animations anim)
	{
		currentAnimation = anim;
		return true;
	}


private:

	ComponentType type;
	GameObject* agent = nullptr;


	Animations currentAnimation;
};



/*
* The walkable building is a special component which does the following.
* For a given building it creates a doorway on given location and
* inserts walkable nodes into the NavMesh before baking.
*/
class WalkableBuildingCmp : public Component
{
public:
	WalkableBuildingCmp(const ComponentID& name, GameObject* go, int doorx, int doory)
		: building(go), doorway(std::make_pair(doorx, doory))
	{
		this->name = name;
		type = "WalkableBuilding";

		init(type);
	}

	ComponentType getType() override { return this->type; }


	std::pair<int, int> getDoorToBuilding()
	{
		TransformCmp* tr = building->getComponent<TransformCmp>("Transform");
		return std::make_pair(tr->xpos + doorway.first, tr->ypos + doorway.second);
	}


private:
	ComponentType type;
	GameObject* building = nullptr;
	std::pair<int, int> doorway;
};