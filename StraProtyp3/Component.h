#pragma once

#include "IComponent.h"

class Component2 : public IComponent
{
public:
	Component2() = default;


	bool initialize(const ComponentType& type, const ComponentID& id, size_t hash) override
	{
		m_Type = type;
		m_Hash = hash;
		m_ID = id;

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