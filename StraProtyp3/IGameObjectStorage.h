#pragma once
/*
* Each Class deriving from Component should include this header to access
* GameObject creation/deletion/referencing etc.
*/

#include "IGameObject.h"

template < class T >
using GameobjectStorageVec = std::vector< Pointer< T > >;

class GameObject2;
class IGameobjectStorage
{
public:

	virtual Pointer<GameObject2> Instantiate(const std::string&) = 0;
	virtual Reference<GameObject2> GetReference(const std::string&) = 0;
	virtual GameobjectStorageVec<GameObject2>& GetStorage() = 0;


	// With this static we can Access the deriving class Object "GameobjectStorage"
	// which implements the functions above.
	// Like that, this Header file can be included in any of the "Component" deriving component classes
	// and still they can be later included in the "GameObjectStorage" Header file without errors.
	//
	// Remember though, on app initialization this Pointer should be populated with an Object.
	static IGameobjectStorage* g_IGameobjectStorage;
};