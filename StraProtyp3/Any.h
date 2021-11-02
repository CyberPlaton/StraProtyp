#pragma once

#include <string>
#include <any>

class Any
{
public:
	Any() = default;
	Any(std::any value, const std::string& type)
	{
		m_Value = value;
		m_Type = type;
	}
	Any(const Any& other)
	{
		m_Value = other.m_Value;
		m_Type = other.m_Type;
	}

	~Any()
	{
		m_Value.reset();
		m_Type.clear();
	}


	/*
	* Get the type.
	* 
	* The type is that one, the user has set on creation.
	*/
	std::string type() const { return m_Type; }


	/*
	* Function returns the data stored as given datatype.
	* 
	* 
	* Invalid type conversions are handled silently by ignoring.
	* 
	* Some errors can be catched at compile time.
	* 
	* 
	* Note:
	* On storing non-trivial datatypes, e.g. objects or pointers, lookout for correct
	* storing/pointing to memory and accessing that memory again, otherwise you may get undefined behavior.
	*/
	template < typename T >
	T as()
	{
		try
		{
			return std::any_cast<T>(m_Value);
		}
		catch (...)
		{
			// ...
		}
	}



	/*
	* Change the stored value.
	* 
	* The old value is not beeing freed, so the user is responsible.
	*/
	template < typename T >
	void set(T value, const std::string& type)
	{
		m_Value = std::any(value);
		m_Type = type;
	}


private:

	std::any m_Value;
	std::string m_Type;
};