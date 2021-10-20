#pragma once
/*
* Interface class for the GameObject class.
*/

#include <vector>
#include <string>
#include <memory>
#include <iostream>

#include "IComponent.h"

template < class T >
using Pointer = std::shared_ptr<T>;

template < class T >
using Reference = std::weak_ptr<T>;

template < class T >
using ComponentStorageVec = std::vector< Pointer< T > >;


class IGameObject
{
public:
	static size_t g_GameObjectCount;
public:

	virtual void setTag(const std::string&) = 0;
	virtual std::string getTag() = 0;

	virtual void setName(const std::string&) = 0;
	virtual std::string getName() = 0;

	
	virtual void setHashvalue(size_t) = 0;
	virtual size_t getHashvalue() = 0;

	virtual void addComponent(Pointer<IComponent>) = 0;
	virtual void removeComponent(Reference<IComponent>) = 0;
	virtual Reference<IComponent> getComponent(const std::string&) = 0;
	virtual ComponentStorageVec<IComponent>& getComponents() = 0;

	virtual bool hasComponent(const std::string&) = 0;
};