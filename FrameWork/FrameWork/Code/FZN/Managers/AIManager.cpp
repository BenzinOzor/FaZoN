//------------------------------------------------------------------------
//Author : Philippe OFFERMANN
//Date : 18.10.14
//Description : Game object database management (definitions)
//Portions Copyright (C) Steve Rabin, 2001
//------------------------------------------------------------------------

#include "FZN/Includes.h"
#include "FZN/Managers/AIManager.h"


FZN_EXPORT fzn::AIManager* g_pFZN_AIMgr = nullptr;

namespace fzn
{
	/////////////////CONSTRUCTOR / DESTRUCTOR/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Default constructor
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	AIManager::AIManager()
	{
		m_nextFreeID = 1;

		g_pFZN_AIMgr = this;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Default destructor
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	AIManager::~AIManager()
	{
		m_gameObjects.Clear();

		g_pFZN_AIMgr = nullptr;
	}


	//=========================================================
	//==========================PUBLIC==========================
	//=========================================================

	/////////////////MAPS MANAGEMENT/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Add a GameObject to the map
	//Parameter : GameObject to store
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void AIManager::Store(GameObjectAI* _gameObject)
	{
		if(_gameObject == nullptr)
			return;

		if(m_gameObjects.FindElement(_gameObject) == nullptr)
			m_gameObjects.PushBack(_gameObject);
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on a GameObject in the map
	//Parameter : ID of the GameObject
	//Return value : Concerned GameObject (nullptr if not found)
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	GameObjectAI* AIManager::GetGameObject(int _ID)
	{
		int index = FindGameObject(_ID);

		if(index == -1)
			return nullptr;

		return m_gameObjects[index];
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Remove a GameObject from the map
	//Parameter : ID of the GameObject
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void AIManager::Remove(int _ID)
	{
		int index = FindGameObject(_ID);

		if(index == -1)
			return;

		m_gameObjects.Remove(index);
	}


	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on a new GameObject ID
	//Return value : ID of the GameObject
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	int AIManager::GetNewObjectID()
	{
		return (m_nextFreeID++);
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on the current highest ID in the game
	//Return value : Highest ID in the game
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	int AIManager::GetCurrentMaxID()
	{
		return m_nextFreeID;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Resets the current ID count
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void AIManager::ResetObjectID()
	{
		m_nextFreeID = 1;
	}

	//=========================================================
	//==========================PRIVATE=========================
	//=========================================================

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Finds a GameObject from its ID
	//Parameter : Id of the GameObject
	//Return value : Index of the GameObject in the vector (-1 if not found or ID not valid)
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	int AIManager::FindGameObject(int _ID)
	{
		if(_ID <= 0 || _ID > m_nextFreeID)
			return -1;

		int iNbAIs = m_gameObjects.Size();
		int IndexInArray = -1;

		for(int i=0 ; i<iNbAIs ; ++i)
		{
			if(m_gameObjects[i]->GetID() == _ID)
			{
				IndexInArray = i;
				i = iNbAIs;
			}
		}

		return IndexInArray;
	}
} //namespace fzn
