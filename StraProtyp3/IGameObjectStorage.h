#pragma once
/*
* Each Class deriving from Component should include this header to access
* GameObject creation/deletion/referencing etc.
*/

#include "IGameObject.h"

template < class T >
using GameobjectStorageVec = std::vector< Pointer< T > >;

using PrefabStorageMap = std::map< std::string, std::string>;


class GameObject2;
class IGameobjectStorage
{
public:

	virtual Pointer<GameObject2> Instantiate(std::string, float, float) = 0;
	virtual Pointer<GameObject2> GetReference(std::string) = 0;
	virtual GameobjectStorageVec<GameObject2>& GetStorage() = 0;
	virtual PrefabStorageMap& GetPrefabStorage() = 0;
	virtual bool DestroyGameobject(std::string) = 0;
	virtual bool DestroyGameobject(Pointer<GameObject2>) = 0;

	virtual bool LoadPrefabs(std::string) = 0;
	virtual void AddPrefab(std::string, std::string) = 0;


	// With this static we can Access the deriving class Object "GameobjectStorage"
	// which implements the functions above.
	// Like that, this Header file can be included in any of the "Component" deriving component classes
	// and still they can be later included in the "GameObjectStorage" Header file without errors.
	//
	// Remember though, on app initialization this Pointer should be populated with an Object.
	static IGameobjectStorage* g_IGameobjectStorage;
	static size_t g_GameobjectCount;
};