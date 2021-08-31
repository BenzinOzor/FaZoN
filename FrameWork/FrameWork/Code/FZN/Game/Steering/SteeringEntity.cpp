//------------------------------------------------------------------------
//Author : Philippe OFFERMANN
//Date : 13.02.15
//Description : Entities used in the steering behaviors
//------------------------------------------------------------------------

#include "FZN/Includes.h"
#include "FZN/Game/Steering/SteeringBehavior.h"
#include "FZN/Managers/SteeringManager.h"
#include "FZN/Managers/WindowManager.h"
#include "FZN/Game/Steering/SteeringEntity.h"


namespace fzn
{
	/////////////////CONSTRUCTOR / DESTRUCTOR/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Default constructor
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	SteeringEntity::SteeringEntity()
	{
		m_mass = 1.f;
		m_maxForce = 100.f;
		m_maxSpeed = 100.f;
		m_fWanderAngle = 0.f;

		m_shape.setFillColor( sf::Color( 0, 150, 150 ) );
		m_shape.setRadius( 8 );
		m_shape.setOutlineColor( sf::Color::Cyan );
		m_shape.setOutlineThickness( 1 );

		m_shape.setOrigin( sf::Vector2f( 8.f, 8.f ) );

		m_shape.setPosition( m_position );

		m_isInManager = FALSE;

		if( g_pFZN_Core->IsSteeringManagerActivated() && g_pFZN_SteeringMgr->AddEntity( this ) )
			m_isInManager = TRUE;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Destructor
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	SteeringEntity::~SteeringEntity()
	{
		ClearBehaviors();

		if( g_pFZN_Core->IsSteeringManagerActivated() && g_pFZN_SteeringMgr->RemoveEntity( this ) )
			m_isInManager = FALSE;
	}


	/////////////////ENTITY MANAGEMENT/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Calculation of the steering force to apply on the entity
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	sf::Vector2f SteeringEntity::Steer()
	{
		sf::Vector2f steeringDirection;
		int iNbBehaviors = m_behaviors.size();

		for( int i = 0 ; i < iNbBehaviors ; ++i )
		{
			WeightedBehavior* behavior = m_behaviors[i];
			steeringDirection += behavior->behavior->Update() *= behavior->weight;
			delete behavior;
		}
		return steeringDirection;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Calculation of the steering force to apply on the entity
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void SteeringEntity::Update()
	{
		sf::Vector2f steeringDirection = Steer();

		if( !Math::IsZeroByEpsilon( Math::VectorLengthSq( steeringDirection ) ) )
		{
			Math::VectorTruncate( steeringDirection, m_maxForce );
			steeringDirection /= m_mass;
			m_velocity = Math::VectorTruncated( m_velocity + steeringDirection * FrameTime, m_maxSpeed );

			m_lastForce = steeringDirection;
		}
		else
			m_lastForce = sf::Vector2f( 0.f, 0.f );

		m_position += m_velocity * FrameTime;
		m_shape.setPosition( m_position );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Display of the entity
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void SteeringEntity::Display()
	{
		g_pFZN_WindowMgr->Draw( m_shape );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Addition of a steering behavior on the class SteeringEntity
	//Parameter 1 : Desired behavior
	//Parameter 2 : Weight of the behavior
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void SteeringEntity::AddBehavior( SteeringBehavior* _behavior, float _weight )
	{
		m_behaviors.push_back( new WeightedBehavior( _behavior, _weight ) );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Removes all the behaviors from the entity
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void SteeringEntity::ClearBehaviors()
	{
		int iNbBehaviors = m_behaviors.size();
		WeightedBehavior* wBehavior = nullptr;

		for( int i = 0 ; i < iNbBehaviors ; ++i )
		{
			wBehavior = m_behaviors[i];

			delete wBehavior->behavior;
			wBehavior->behavior = nullptr;

			delete m_behaviors[i];
			m_behaviors[i] = nullptr;
		}

		m_behaviors.clear();

		m_fWanderAngle = 0.f;
	}


	/////////////////ACCESSORS / MUTATORS/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on the last force applied to the entity
	//Return value : Last force applied
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	sf::Vector2f SteeringEntity::GetLastForce()
	{
		return m_lastForce;
	}
} //namespace fzn
