#include "NavigatorComponent.h"

bool NavigatorComponent::IsDestinationReached()
{
	auto transform = m_Agent->getComponent<TransformComponent>("Transform");

	return (transform->xpos == destx && transform->ypos == desty) == true ? true : false;
}


void NavigatorComponent::SetDestination(float x, float y)
{
	destx = x;
	desty = y;
	movementPoints.clear(); // Reset the Pathfinder.
	current_mp_index = 0;
	internal_positionx = 0.0f; // Reset the internal agent position counter.
	internal_positiony = 0.0f;

}



bool NavigatorComponent::IsTargetDestinationValid(float x, float y)
{
	if (x > NavMesh::get()->GetWidth() ||
		y > NavMesh::get()->GetHeight() ||
		x < 0 ||
		y < 0)
	{
		return false;
	}


	std::vector<std::vector<float>> v = NavMesh::get()->GetNavGraph();

	for (int i = 0; i < v.size(); i++)
	{
		for (int j = 0; j < v[i].size(); j++)
		{
			if (x == i || y == j)
			{
				if (v[i][j] != 1.0f)
				{
					return false;
				}
			}
		}
	}

	return true;
}


bool NavigatorComponent::Update(float dt)
{
	using namespace std;

	if (!m_Agent)
	{
		return false;
	}

	auto transform = m_Agent->getComponent<TransformComponent>("Transform");
	if (!transform)
	{
		return false;
	}


	// Is Target reached
	if (transform->xpos == destx && transform->ypos == desty)
	{
		// Reset internal data and return indication that destination was reached.
		movementPoints.clear();
		current_mp_index = 0;
		internal_positionx = 0.0f; // Reset the internal agent position counter.
		internal_positiony = 0.0f;
		return true;
	}


	if (movementPoints.size() == 0)
	{
		// Get a path from current position to given destination.
		movementPoints = this->_getPathToDest(transform->xpos, transform->ypos, destx, desty,  NavMesh::get()->GetGraph());
	}

	if (movementPoints.size() > 0)
	{
		// Internally update the movement of the agent.
		_updateWaypointMovement(dt, transform);
	}


	// Component update, if needed.
	if (internal_positionx > 1.0f)
	{
		transform->xpos += 1;
		internal_positionx = 0.0f;
	}
	else if (internal_positionx < -1.0f)
	{
		transform->xpos -= 1;
		internal_positionx = 0.0f;
	}


	if (internal_positiony > 1.0f)
	{
		transform->ypos += 1;
		internal_positiony = 0.0f;
	}
	else if (internal_positiony < -1.0f)
	{
		transform->ypos -= 1;
		internal_positiony = 0.0f;
	}

}



bool NavigatorComponent::_updateWaypointMovement(float dt, Pointer<TransformComponent> transform)
{
	// Get next waypoint.
	current_mpx = std::get<0>(movementPoints[current_mp_index]);
	current_mpy = std::get<1>(movementPoints[current_mp_index]);

	// Move to waypoint.
	// In movement to waypoint we do not have to check for obstacles.
	if (transform->xpos < current_mpx)
	{
		internal_positionx += dt;
	}

	else if (transform->xpos > current_mpx)
	{
		internal_positionx -= dt;
	}


	if (transform->ypos < current_mpy)
	{
		internal_positiony += dt;
	}
	else if (transform->ypos > current_mpy)
	{
		internal_positiony -= dt;
	}

	// Check whether waypoint reached and needs to be updated.
	if (transform->xpos == current_mpx && transform->ypos == current_mpy)
	{
		current_mp_index++;
	}

	return true;
}


std::vector< node >& NavigatorComponent::_getPathToDest(float startx, float starty, float x, float y, Pointer<Graph> graph)
{
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


		if (_isNodeDestNode(nearest, { x, y }))
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
	}

	return path;
}


bool NavigatorComponent::_nodeInList(node n, std::vector<node>& v)
{
	std::vector<node>::iterator it = std::find(v.begin(), v.end(), n);
	if (it != v.end())
	{
		return true;
	}

	return false;
}


bool NavigatorComponent::_removeNode(node n, std::vector<node>& v)
{
	std::vector<node>::iterator it = std::find(v.begin(), v.end(), n);
	if (it != v.end())
	{
		v.erase(it);
		return true;
	}

	return false;
}


bool NavigatorComponent::_isNodeDestNode(node x, node dest)
{
	if (node_x(x) == node_x(dest) &&
		node_y(x) == node_y(dest))
	{
		return true;
	}

	return false;
}


node NavigatorComponent::_getNodeWithLowestCost(std::vector<node>& v, std::map<node, float>& map)
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


float NavigatorComponent::_manhattenDistance(node x, node y)
{
	float xx, xy, yx, yy;
	xx = node_x(x);
	xy = node_y(x);
	yx = node_x(y);
	yy = node_y(y);

	// Vectorial 2D-Distance.
	return abs(xx - yx) + abs(xy - yy);
}

