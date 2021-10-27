#pragma once
#include "IGameObject.h"
#include "IComponent.h"
#include "ColorConsole.h"


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
	GameObject2(const GOTag& tag, const GOName& name) : m_Name(name), m_Tag(tag) 
	{
		m_Hash = ++IGameObject::g_GameObjectCount;
	};
	GameObject2(const GameObject2& other)
	{
		m_Tag = other.m_Tag;
		m_Name = other.m_Name;
		m_Hash = ++IGameObject::g_GameObjectCount;
	}
	~GameObject2()
	{
		using namespace std;

		// Delete Components.
		for (int i = 0; i < components.size(); i++)
		{
			if (components[i].use_count() > 1)
			{
				cout << color(colors::RED);
				cout << "[GameObject2::~GameObject2] Component \""<< components[i]->getComponentID() << "\" has more than one Uses! Current count: " << components[i].use_count() << white << endl;
			}

			components[i]->destroy();
			components[i].reset();
		}

		components.clear();
	}


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


	void setHashvalue(size_t h) override final
	{
		m_Hash = h;
	}

	size_t getHashvalue() override final
	{
		return m_Hash;
	}


	void addComponent(Pointer<IComponent> component) override final
	{
		using namespace std;

		for (auto& c : components)
		{
			if (c->getComponentType().compare(component->getComponentType()) == 0)
			{
				cout << color(colors::MAGENTA);
				cout << "Component \""<< component->getComponentID() << "\{"<< component->getComponentType() << "} will not be added as duplicate" << white << endl;
				component.reset();
				return;
			}
		}

		components.push_back(component);
	}

	void removeComponent(Pointer<IComponent> component) override final
	{
		for (int i = 0; i < components.size(); i++)
		{
			if (components[i]->getComponentID().compare(component->getComponentID()) == 0)
			{
				components[i].reset(); // Reset the Component and clear from memory.
				components.erase(components.begin() + i); // Remove it from Vector entry.
				return;
			}
		}
	}


	template < typename T >
	Pointer<T> getComponent(const std::string& id)
	{
		for (int i = 0; i < components.size(); i++)
		{
			if (components[i]->getComponentType().compare(id) == 0)
			{
				auto ref = std::static_pointer_cast<T>(components[i]);
				return ref;
			}
		}
	}


	Pointer<IComponent> getComponent(const std::string& id) override final
	{
		for (int i = 0; i < components.size(); i++)
		{
			if (components[i]->getComponentID().compare(id) == 0)
			{
				return std::shared_ptr<IComponent>(components[i]);
			}
		}
	}

	ComponentStorageVec<IComponent>& getComponents() override final
	{
		return components;
	}


	bool hasComponent(const std::string& tag) override final
	{
		for (auto& c : components)
		{
			if (c->getComponentType().compare(tag) == 0) return true;
		}

		return false;
	}


private:
	std::string m_Name;
	std::string m_Tag;
	size_t m_Hash;
	ComponentStorageVec<IComponent> components;
};