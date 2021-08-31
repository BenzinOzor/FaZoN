//------------------------------------------------------------------------
//Author : Philippe OFFERMANN
//Date : 13.02.15
//Description : Objects used as obstacles or waypoints
//------------------------------------------------------------------------

#include "FZN/Includes.h"
#include "FZN/Managers/SteeringManager.h"
#include "FZN/Game/Steering/SteeringObject.h"


namespace fzn
{
	/////////////////CONSTRUCTOR / DESTRUCTOR/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Default constructor
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	SteeringObject::SteeringObject()
	{
		setFillColor( sf::Color( 0, 150, 0, 0 ) );
		setOutlineColor( sf::Color::Red );
		setOutlineThickness( 1 );
		m_isInManager = FALSE;

		if( g_pFZN_Core->IsSteeringManagerActivated() && g_pFZN_SteeringMgr->AddObject( this ) )
			m_isInManager = TRUE;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Destructor
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	SteeringObject::~SteeringObject()
	{
		if( g_pFZN_Core->IsSteeringManagerActivated() && g_pFZN_SteeringMgr->RemoveObject( this ) )
			m_isInManager = FALSE;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Sets the position and radius of the obstacle
	//Parameter 1 : Position of the obstacle
	//Parameter 2 : Radius of the obstacle
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void SteeringObject::SetPositionAndRadius( const sf::Vector2f& _position, const float _radius )
	{
		setRadius( _radius );
		setOrigin( sf::Vector2f( _radius, _radius ) );
		setPosition( _position );
	}
} //namespace fzn
