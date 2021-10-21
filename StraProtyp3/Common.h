#pragma once

#include <string>
#include <vector>
#include <map>


using RaceID = std::string;
using TechID = std::string;
using RessourceID = std::string;
using CityType = std::string;
using BuildingSlotType = std::string;
using CityFortificationLevel = std::string;
using ReligionID = std::string;
using UnitProfession = std::string;
using MaptileType = std::string;
using GOTag = std::string;
using GOName = std::string;



template < class A, class B >
struct Tuple
{
	Tuple(A x, B y) :x(x), y(y) {}
	Tuple() = default;
	~Tuple() = default;
	A x;
	B y;
};