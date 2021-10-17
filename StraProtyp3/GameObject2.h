#pragma once
#include "IGameObject.h"
#include "IComponent.h"

/*
* The GOTag can be seen as the App unique identifier for a the GameObject.
* Thus we can identify each and we can easily search for each.
*
* The tag is generated automatically on construction like for example "GO_16_Furniture".
*/
using GOTag = std::string;

/*
* THE GOName is some given or generated name for a gameobject, which is not necesseraly
* unique and can be duplicate, like the name "John" in a city.
*/
using GOName = std::string;


class GameObject2 : public IGameObject
{
public:
	GameObject2() = default;
	GameObject2(const GOTag& tag, const GOName& name) : m_Name(name), m_Tag(tag) {};

	void setTag(const std::string& tag) override final
	{
		m_Tag = tag;
	}

	std::string getTag() override final
	{
		return m_Tag;
	}

	void setName(const std::string& name) override final
	{
		m_Name = name;
	}

	std::string getName() override final
	{
		return m_Name;
	}


	void setHashvalue(size_t) override final
	{

	}

	size_t getHashvalue() override final
	{
		return 0;
	}


	void addComponent(Pointer<IComponent> component) override final
	{
		component.reset();
	}

	void removeComponent(Pointer<IComponent> component) override final
	{
		component.reset();
	}

	Pointer<IComponent> getComponent(const std::string&) override final
	{
		return nullptr;
	}

	ComponentStorageVec<IComponent>& getComponents() override final
	{
		return components;
	}


	bool hasComponent(const std::string&) override final
	{
		return false;
	}


private:
	std::string m_Name;
	std::string m_Tag;
	ComponentStorageVec<IComponent> components;
};