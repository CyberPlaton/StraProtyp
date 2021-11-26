#pragma once
#include "Component2.h" // Include for extending the Component2 class
#include "GameObject2.h"
#include "IGameObjectStorage.h" // Include for GameobjectStorage Interface, mainly for Instantiation, Finding and Deleting functions
#include "Common.h" // Include Common Definitions and Datastructs.

#include "ColorConsole.h"

#include "TransformComponent.h"


class NavMesh;
class GameobjectStorage;

class CollisionBoxComponent : public Component2
{
	friend class NavMesh;
	friend class GameobjectStorage;

public:
	CollisionBoxComponent(ComponentID name)
	{
		initialize("CollisionBox", name);
	}
	~CollisionBoxComponent()
	{
		width = 0;
		height = 0;
		m_Agent.reset();
	}

	float GetWidth() { return width; }
	float GetHeight() { return height; }


	bool Resolve(Pointer<GameObject2> other)
	{
		auto otr = other->getComponent<TransformComponent>("Transform");
		auto ocoll = other->getComponent<CollisionBoxComponent>("CollisionBox");

		return Resolve(otr->xpos, otr->ypos, ocoll->width, ocoll->height);
	}


	bool Resolve(float x, float y, float w, float h)
	{
		using namespace std;

		auto agent = m_Agent.lock();
		if (!agent) return false;

		cout << color(colors::CYAN);
		cout << "[CollisionBoxCmp::resolve] resolving Pawn \"" << agent->getName() << "\" and Object {x=" << x << ", y=" << y << ", w=" << w << ", h=" << h << "}" << white << endl;

		auto pTransform = agent->getComponent<TransformComponent>("Transform");

		// Resolve collision for this object 
		// If it is a walkable building
		cout << color(colors::DARKCYAN);
		cout << "\t ...Resolve for Solid Object..." << white << endl;


		if (pTransform->xpos < x + w &&
			pTransform->xpos + width > x &&
			pTransform->ypos < y + h &&
			pTransform->ypos + height > y)
		{

			cout << color(colors::DARKCYAN);
			cout << "\t\t ...Object is colliding with solid object..." << white << endl;

			return true;
		}


		cout << color(colors::DARKCYAN);
		cout << "\t\t ...No collision detected..." << white << endl;
		return false;
	}



private:

	Reference<GameObject2> m_Agent;

	float width = 0.0f;
	float height = 0.0f;
};