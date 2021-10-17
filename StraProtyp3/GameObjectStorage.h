#pragma once

#include "ColorConsole.h"


// Interface for imlpementation.
#include "IGameobjectStorage.h"


// Include the essential Gameobject interface.
// Every Entity in the game is a Gameobject with a number of Components (ECS).
#include "GameObject2.h"



// Include the essential components for the App.
// Every component which does exist in the game,
// must be included here, because the "GameobjectStorage" creates GOs and
// adds all the Components predefined in the prefab.
#include "TestingComponent.h"


class GameobjectStorage : public IGameobjectStorage
{
public:
	// Singleton Class.
	// Should be reachable Game wide.
	static IGameobjectStorage* get();
	static void del();


	// Construct a new Gameobject from preexisting prefab definition.
	// Returns a shared pointer,
	// where the instance itself is destructed on App shutdown.
	Pointer<GameObject2> Instantiate(const std::string& prefabName) override
	{
		Pointer<GameObject2> ptr = std::make_shared<GameObject2>(prefabName, prefabName);
		gameObjectStorage.push_back(ptr);

		return ptr;
	}

	// Try to get a reference to an existing Gameobject.
	// Returns a weak pointer,
	// where the reference count is not affected.
	Reference<GameObject2> GetReference(const std::string& gameobjectTag) override
	{
		// Return a weak reference to a shared pointer.
		// Thus not increasing the reference count.
		// The correct index of the Gameobject should be found first.
		return gameObjectStorage[0];
	}



	GameobjectStorageVec<GameObject2>& GetStorage() override
	{
		return gameObjectStorage;
	}

private:


	GameobjectStorageVec<GameObject2> gameObjectStorage;

};