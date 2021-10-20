#pragma once
/*
* A Component is basically a collection of data.
* Whereas Systems are collections of functionality which operate in a special way
* on that data.
*/

#include "IComponent.h"


class Component2 : public IComponent
{
public:
	Component2() = default;
	virtual ~Component2()
	{
		m_Type.clear();
		m_ID.clear();
		m_Hash = 0;
	}

	bool initialize(const ComponentType& type, const ComponentID& id) override
	{
		m_Type = type;
		m_ID = id;
		m_Hash = ++IComponent::g_ComponentHash;

		return true;
	}

	ComponentType getComponentType() override final
	{
		return m_Type;
	}

	ComponentID getComponentID() override final
	{
		return m_ID;
	}

	void setComponentType(const ComponentType& type) override final
	{
		m_Type = type;
	}

	void setComponentID(const ComponentID& id) override final
	{
		m_ID = id;
	}

	size_t getComponentHashvalue() override final
	{
		return m_Hash;
	}

	void setComponentHashvalue(size_t n) override final
	{
		m_Hash = n;
	}



private:
	size_t m_Hash = 0;
	ComponentID m_ID;
	ComponentType m_Type;
};