#pragma once

#include <string>
#include <map>
#include <vector>
#include <typeinfo>
#include <iostream>


/*
* The ComponentID is how the user names it, this can be anything, even a unique name for each component,
* for example for the TransformCmp of some Agent named "Ralf" we can assign the ComponentID as := "GO_98_Ralf_Transform".
*/
using ComponentID = std::string;

/*
* The ComponentType is more of a runtime  type information and describes what kind of class the component it,
* thus we can know to what we can cast it. For now it is a string, but will be enumed or hashed...
*/
using ComponentType = std::string;



class IComponent
{
public:

	static size_t g_ComponentHash;

public:
	virtual bool initialize(const ComponentType&, const ComponentID&, size_t) = 0;

	virtual ComponentType getComponentType() = 0;
	virtual ComponentID getComponentID() = 0;

	virtual void setComponentType(const ComponentType&) = 0;
	virtual void setComponentID(const ComponentID&) = 0;

	virtual size_t getComponentHashvalue() = 0;
	virtual void setComponentHashvalue(size_t) = 0;
};