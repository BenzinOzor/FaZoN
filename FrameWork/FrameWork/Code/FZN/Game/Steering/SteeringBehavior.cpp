//------------------------------------------------------------------------
//Author : Philippe OFFERMANN
//Date : 13.02.15
//Description : Steering behaviors base class
//------------------------------------------------------------------------

#include "FZN/Includes.h"
#include "FZN/Game/Steering/SteeringBehavior.h" 


namespace fzn
{
	/////////////////CONSTRUCTOR / DESTRUCTOR/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Parametered constructor
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	SteeringBehavior::SteeringBehavior( SteeringEntity* _entity )
	{
		m_entity = _entity;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Destructor
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	SteeringBehavior::~SteeringBehavior()
	{
	}
}
