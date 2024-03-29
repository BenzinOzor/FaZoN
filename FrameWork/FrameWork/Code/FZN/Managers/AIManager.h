//------------------------------------------------------------------------
//Author : Philippe OFFERMANN
//Date : 18.10.14
//Description : Game object database management (declarations)
//Portions Copyright (C) Steve Rabin, 2001
//------------------------------------------------------------------------


#ifndef _AIMANAGER_H_
#define _AIMANAGER_H_

#include "FZN/DataStructure/Vector.h"
#include "FZN/Game/GameObjectAI/GameObjectAI.h"

#define NewID g_pFZN_AIMgr->GetNewObjectID()
#define MaxID g_pFZN_AIMgr->GetCurrentMaxID()

namespace fzn
{
	class FZN_EXPORT AIManager : public sf::NonCopyable
	{
	public:
		/////////////////CONSTRUCTOR / DESTRUCTOR/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Default constructor
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		AIManager();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Default destructor
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		~AIManager();


		/////////////////MAPS MANAGEMENT/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Add an GameObject to the map
		//Parameter : GameObject to store
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void Store( GameObjectAI* _gameObject );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on a GameObject in the map
		//Parameter : ID of the GameObject
		//Return value : Concerned GameObject (nullptr if not found)
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		GameObjectAI* GetGameObject( int _ID );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Remove a GameObject from the map
		//Parameter : ID of the GameObject
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void Remove( int _ID );

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on a new GameObject ID
		//Return value : ID of the GameObject
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		int GetNewObjectID();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on the current highest ID in the game
		//Return value : Highest ID in the game
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		int GetCurrentMaxID();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Resets the current ID count
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void ResetObjectID();

	private:
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Finds a GameObject from its ID
		//Parameter : Id of the GameObject
		//Return value : Index of the GameObject in the vector (-1 if not found or ID not valid)
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		int FindGameObject( int _ID );

		/////////////////MEMBER VARIABLES/////////////////

		int m_nextFreeID;										//ID of the next GameObject created
		Vector<GameObjectAI*> m_gameObjects;					//Array of Game Objects
	};
} //namespace fzn

extern FZN_EXPORT fzn::AIManager* g_pFZN_AIMgr;

#endif //_AIMANAGER_H_
