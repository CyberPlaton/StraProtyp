#pragma once
/*
* Interface class for a component.
* See a component as Data, where it stores some data related to a very specific field.
* And thus Systems can operate on that Data altering the state of an entity.
*/

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


enum EComponentType
{
	CT_Transform,
	CT_Animator,
	CT_Building,
	CT_City,
	CT_CollisionBox,
	CT_Forest,
	CT_Hill,
	CT_Improvement,
	CT_Maptile,
	CT_Mountain,
	CT_Nation,
	CT_Navigator,
	CT_Player,
	CT_Renderable,
	CT_Ressource,
	CT_River,
	CT_Road,
	CT_Technology,
	CT_Unit,
	CT_Enum_Max
};

std::string EComponentTypeString(EComponentType type);

class IComponent
{
public:

	static size_t g_ComponentHash;

public:
	virtual bool initialize(ComponentType, ComponentID) = 0;

	virtual ComponentType getComponentType() = 0;
	virtual ComponentID getComponentID() = 0;

	virtual void setComponentType(ComponentType) = 0;
	virtual void setComponentID(ComponentID) = 0;

	virtual size_t getComponentHashvalue() = 0;
	virtual void setComponentHashvalue(size_t) = 0;


	virtual void update() = 0;
	virtual void destroy() = 0;
};