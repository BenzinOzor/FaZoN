//------------------------------------------------------------------------
//Author : Philippe OFFERMANN
//Date : 13.02.15
//Description : Manages the steering entities
//------------------------------------------------------------------------

#include "FZN/Includes.h"
#include "FZN/Game/Steering/SteeringEntity.h"
#include "FZN/Game/Steering/SteeringObject.h"
#include "FZN/DataStructure/Vector.h"
#include "FZN/Managers/SteeringManager.h"


FZN_EXPORT fzn::SteeringManager* g_pFZN_SteeringMgr = nullptr;

namespace fzn
{
	/////////////////CONSTRUCTOR / DESTRUCTOR/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Default constructor
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	SteeringManager::SteeringManager()
	{
		m_iNbEntities = m_entities.Size();

		m_iNbObjects = m_objects.Size();

		g_pFZN_SteeringMgr = this;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Destructor
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	SteeringManager::~SteeringManager()
	{
		m_entities.Clear();
		m_objects.Clear();

		g_pFZN_SteeringMgr = nullptr;
	}


	//=========================================================
	//==========================PUBLIC==========================
	//=========================================================

	/////////////////ARRAYS MANAGEMENT/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Adds an entity in the manager's array
	//Parameter : Entity to add
	//Return value : Index in which the entity has been placed
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	INT8 SteeringManager::AddEntity( SteeringEntity* _entity )
	{
		if( _entity == nullptr )
			return FALSE;

		if( m_entities.FindElement( _entity ) == nullptr )
		{
			m_entities.PushBack( _entity );
			m_iNbEntities++;
			return TRUE;
		}

		return FALSE;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Removes an entity from the manager's array
	//Parameter : Entity to remove
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	INT8 SteeringManager::RemoveEntity( SteeringEntity* _entity )
	{
		if( _entity == nullptr || !_entity->m_isInManager )
			return FALSE;

		int iEntityIndex = m_entities.FindIndex( _entity );

		if( iEntityIndex == -1 )
			return FALSE;

		m_entities.Remove( iEntityIndex );

		m_iNbEntities--;
		return TRUE;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Adds an object in the manager's array
	//Parameter : Object to add
	//Return value : Index in which the object has been placed
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	INT8 SteeringManager::AddObject( SteeringObject* _object )
	{
		if( _object == nullptr )
			return FALSE;

		if( m_objects.FindElement( _object ) == nullptr )
		{
			m_objects.PushBack( _object );
			m_iNbObjects++;
			return TRUE;
		}

		return FALSE;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Removes an object from the manager's array
	//Parameter : Object to remove
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	INT8 SteeringManager::RemoveObject( SteeringObject* _object )
	{
		if( _object == nullptr || !_object->m_isInManager )
			return FALSE;

		int iObjectIndex = m_objects.FindIndex( _object );

		if( iObjectIndex == -1 )
			return FALSE;

		m_objects.Remove( iObjectIndex );

		m_iNbObjects--;
		return TRUE;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Updates all the entities in the array
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void SteeringManager::Update()
	{
		for( int i = 0 ; i < m_iNbEntities ; ++i )
			m_entities[i]->Update();
	}


	/////////////////ACCESSOR / MUTATOR/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on number of entities in the manager
	//Return value : Number of entities
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	int SteeringManager::GetEntitiesNumber()
	{
		return m_iNbEntities;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on number of objects in the manager
	//Return value : Number of objects
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	int SteeringManager::GetObjectsNumber()
	{
		return m_iNbObjects;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on the entities array
	//Return value : Entities
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	Vector<SteeringEntity*>& SteeringManager::GetEntities()
	{
		return m_entities;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on the objects array
	//Return value : Objects
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	Vector<SteeringObject*>& SteeringManager::GetObjects()
	{
		return m_objects;
	}
} //namespace fzn
