#pragma once

#include "Component2.h" // Include for extending the Component2 class
#include "GameObject2.h"
#include "IGameObjectStorage.h" // Include for GameobjectStorage Interface, mainly for Instantiation, Finding and Deleting functions
#include "Common.h" // Include Common Definitions and Datastructs.

#include "ColorConsole.h"


#include "TransformComponent.h"
#include "NavMesh.h"


class GameobjectStorage;
class NavigatorComponent : public Component2
{
	friend class GameobjectStorage;

public:
	NavigatorComponent(const std::string& name)
	{
		initialize("Navigator", name);
	}


	bool IsDestinationReached();
	void SetDestination(float x, float y);
	
	bool IsTargetDestinationValid(float x, float y);
	bool Update(float dt);

private:

	Pointer<GameObject2> m_Agent;
	
	float destx, desty;
	
	float internal_positionx, internal_positiony;

	std::vector< node > movementPoints;
	int current_mp_index = 0;
	float current_mpx, current_mpy;



	bool _updateWaypointMovement(float dt, Pointer<TransformComponent> transform);
	std::vector< node >& _getPathToDest(float startx, float starty, float x, float y, Pointer<Graph> graph);
	bool _nodeInList(node n, std::vector<node>& v);
	bool _removeNode(node n, std::vector<node>& v);
	bool _isNodeDestNode(node x, node dest);
	node _getNodeWithLowestCost(std::vector<node>& v, std::map<node, float>& map);
	float _manhattenDistance(node x, node y);
};