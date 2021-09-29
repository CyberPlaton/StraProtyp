#pragma once

#include <utility>
#include <vector>
#include <map>


template < class T >
struct State
{
	virtual void update(float) = 0;
	virtual void onEnter() = 0;
	virtual void onExit() = 0;
	virtual T* getOwner() = 0;
};


template < class T >
struct StateMachine
{


	void setInitialState(const std::string& initialState)
	{
		currentState = initialState;
		nextState = initialState;
	}

	std::string getCurrentState() { return currentState; }

	void update(float dt)
	{
		if (nextState.compare(currentState) == 0)
		{
			// Update.
			_getStateByName(currentState)->update(dt);
		}
		else
		{
			// Change State.
			_getStateByName(currentState)->onExit();

			currentState = nextState;

			_getStateByName(currentState)->onEnter();
		}
	}


	void setState(const std::string& name)
	{
		nextState = name;
	}


	State<T>* findState(const std::string& name)
	{
		return _getStateByName(name);
	}


	void storeStateDefinition(const std::string& name, State<T>* state)
	{
		states.emplace(name, state);
	}

private:

	std::string currentState;
	std::string nextState;

	std::map< std::string, State<T>* > states;

	State<T>* _getStateByName(const std::string& name)
	{
		for (auto& it : states)
		{
			if (it.first.compare(name) == 0) return it.second;
		}

		return nullptr;
	}
};