//------------------------------------------------------------------------
//Author : Philippe OFFERMANN
//Date : 13.02.15
//Description : Steering behaviors
//------------------------------------------------------------------------

#include "FZN/Includes.h"
#include "FZN/Game/Steering/SteeringBehavior.h"
#include "FZN/Game/Steering/SteeringObject.h"
#include "FZN/Managers/SteeringManager.h"
#include "FZN/Game/Steering/SteeringBehaviors.h"


namespace fzn
{
	//=========================================================
	//===========================SEEK===========================
	//=========================================================

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Seeks a given target position
	//Parameter 1 : Concerned entity
	//Parameter 2 : Position to reach
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	sf::Vector2f Seek::SeekBehavior( class SteeringEntity* _entity, const sf::Vector2f& _targetPos )
	{
		sf::Vector2f posToTarget = _targetPos - _entity->m_position;
		Math::VectorNormalize( posToTarget );
		sf::Vector2f desiredVelocity = posToTarget * _entity->m_maxSpeed;

		sf::Vector2f* returnVector = new sf::Vector2f( desiredVelocity - _entity->m_velocity );
		return *returnVector;
	}


	//=========================================================
	//===========================FLEE===========================
	//=========================================================

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Flees a given target position
	//Parameter 1 : Concerned entity
	//Parameter 2 : Position to flee
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	sf::Vector2f& Flee::FleeBehavior( class SteeringEntity* _entity, sf::Vector2f& _targetPos )
	{
		sf::Vector2f posToTarget = _entity->m_position - _targetPos;
		Math::VectorNormalize( posToTarget );
		sf::Vector2f desiredVelocity = posToTarget * _entity->m_maxSpeed;

		sf::Vector2f* returnVector = new sf::Vector2f( desiredVelocity - _entity->m_velocity );
		return *returnVector;
	}


	//=========================================================
	//=========================PURSUIT==========================
	//=========================================================

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Pursue a given target
	//Parameter 1 : Concerned entity
	//Parameter 2 : Target to reach
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	sf::Vector2f& Pursuit::PursuitBehavior( class SteeringEntity* _entity, class SteeringEntity* _target, float _c )
	{
		sf::Vector2f posToTarget = _entity->m_position - _target->m_position;

		float D = Math::VectorLength( posToTarget );
		float T = _c * D / Math::VectorLength( _entity->m_velocity );

		sf::Vector2f futurePos = _target->m_position + _target->m_velocity * T;

		sf::Vector2f* returnVector = new sf::Vector2f( Seek::SeekBehavior( _entity, futurePos ) );
		return *returnVector;
	}


	//=========================================================
	//=========================EVASION==========================
	//=========================================================

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Avoids a given target
	//Parameter 1 : Concerned entity
	//Parameter 2 : Target to avoid
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	sf::Vector2f& Evasion::EvasionBehavior( class SteeringEntity* _entity, class SteeringEntity* _target, float _c )
	{
		sf::Vector2f posToTarget = _target->m_position - _entity->m_position;

		float D = Math::VectorLength( posToTarget );
		float T = _c * D;

		sf::Vector2f futurePos = _target->m_position + _target->m_velocity * T;

		sf::Vector2f* returnVector = new sf::Vector2f( Flee::FleeBehavior( _entity, futurePos ) );
		return *returnVector;
	}


	//=========================================================
	//=========================ARRIVAL==========================
	//=========================================================

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Goes to a given position and stops
	//Parameter 1 : Concerned entity
	//Parameter 2 : Position to reach
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	sf::Vector2f Arrival::ArrivalBehavior( class SteeringEntity* _entity, const sf::Vector2f& _targetPos, float _slowingDistance )
	{
		sf::Vector2f targetOffset = _targetPos - _entity->m_position;
		float distance = Math::VectorLength( targetOffset );
		sf::Vector2f* returnVector = new sf::Vector2f;

		if( !Math::IsZeroByEpsilon( distance ) )
		{
			float rampedSpeed = _entity->m_maxSpeed * ( distance / _slowingDistance );

			float clippedSpeed = Math::Min( rampedSpeed, _entity->m_maxSpeed );

			sf::Vector2f desiredVelocity = targetOffset * ( clippedSpeed / distance );

			*returnVector = desiredVelocity - _entity->m_velocity;
		}
		else *returnVector = sf::Vector2f( 0.f, 0.f );

		return *returnVector;
	}


	//=========================================================
	//====================OBSTACLE AVOIDACE======================
	//=========================================================

	sf::Vector2f& ObstacleAvoidance::Update()
	{
		SteeringObject* nearestObstacle = nullptr;
		UINT8 isCollisioning = FALSE;
		float distToNearest = 0.0;
		int nbObstacles = g_pFZN_SteeringMgr->GetObjectsNumber();

		sf::Vector2f velocity = m_entity->m_velocity;
		float dynamicLength = Math::VectorLength( velocity ) / m_entity->m_maxSpeed;
		Math::VectorNormalize( velocity );

		sf::Vector2f ahead = m_entity->m_position + velocity * dynamicLength;
		sf::Vector2f ahead2 = m_entity->m_position + velocity * m_farView * 0.5f;

		sf::Vector2f aheadToObstacle;
		sf::Vector2f ahead2ToObstacle;
		sf::Vector2f posToObstacle;
		//Determination of the nearest obstacle to avoid
		for( int i = 0 ; i < nbObstacles ; ++i )
		{
			aheadToObstacle = m_obstacles[i].getPosition() - ahead;
			ahead2ToObstacle = m_obstacles[i].getPosition() - ahead2;
			posToObstacle = m_obstacles[i].getPosition() - m_entity->m_position;

			if( Math::VectorLength( aheadToObstacle ) <= m_obstacles[i].getRadius() + m_fRadius || Math::VectorLength( ahead2ToObstacle ) <= m_obstacles[i].getRadius() + m_fRadius )
				isCollisioning = true;
			else isCollisioning = false;

			if( isCollisioning && ( nearestObstacle == nullptr || Math::VectorLength( posToObstacle ) < distToNearest ) )
			{
				nearestObstacle = &m_obstacles[i];
				distToNearest = Math::VectorLength( posToObstacle );
			}
		}

		if( nearestObstacle != nullptr )
		{
			aheadToObstacle = ahead - nearestObstacle->getPosition();
			Math::VectorNormalize( aheadToObstacle );
			aheadToObstacle *= m_entity->m_maxForce;

			m_steering = aheadToObstacle;
		}
		else m_steering *= 0.0f;

		return m_steering;
	}


	//=========================================================
	//=========================WANDER==========================
	//=========================================================

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Moves around randomly
	//Parameter 1 : Concerned entity
	//Parameter 2 : Distance between the circle at the front of the entity and itself
	//Parameter 3 : Radius of the circle at the front of the entity
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	sf::Vector2f& Wander::WanderBehavior( class SteeringEntity* _entity, float _fDistance, float _fRadius )
	{
		sf::Vector2f velocity = _entity->m_velocity;
		Math::VectorNormalize( velocity );
		sf::Vector2f circlePos = velocity * _fDistance;
		sf::Vector2f displacement( 0, -1 );
		displacement *= _fRadius;

		float fDisplacementLength = Math::VectorLength( displacement );

		displacement.x = cos( _entity->m_fWanderAngle ) * fDisplacementLength;
		displacement.y = sin( _entity->m_fWanderAngle ) * fDisplacementLength;

		_entity->m_fWanderAngle += rand() % 2 * ( Math::PIdiv4 ) - ( Math::PIdiv4 ) * 0.5f;

		m_steering = circlePos + displacement;

		return m_steering;
	}


	//=========================================================
	//=====================PATH FOLLOWING=======================
	//=========================================================

	sf::Vector2f& PathFollowing::Update()
	{
		if( m_obstacles != nullptr )
		{
			sf::Vector2f obstacleToPos = m_obstacles[m_iCurrentObstacle].getPosition() - m_entity->m_position;

			if( Math::VectorLengthSq( obstacleToPos ) <= Math::Square( m_obstacles[m_iCurrentObstacle].getRadius() ) )
			{
				if( m_roundTrip )
				{
					m_iCurrentObstacle += m_iDirection;

					if( m_iCurrentObstacle == m_iNbObstacles || m_iCurrentObstacle < 0 )
					{
						m_iDirection *= -1;
						m_iCurrentObstacle += m_iDirection;
					}
				}
				else if( m_iCurrentObstacle < m_iNbObstacles - 1 )
					m_iCurrentObstacle++;
			}

			if( ( m_iDirection == -1 && m_iCurrentObstacle > 0 ) || ( m_iDirection == 1 && m_iCurrentObstacle < m_iNbObstacles - 1 ) )
				m_steering = Seek::SeekBehavior( m_entity, sf::Vector2f( m_obstacles[m_iCurrentObstacle].getPosition() ) );
			else
				m_steering = Arrival::ArrivalBehavior( m_entity, sf::Vector2f( m_obstacles[m_iCurrentObstacle].getPosition() ), 100.f );

		}
		else
			m_steering = sf::Vector2f( 0.f, 0.f );

		return m_steering;
	}


	//=========================================================
	//==========================UCA============================
	//=========================================================

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Avoid collisions with other entities
	//Parameter 1 : Concerned entity
	//Parameter 2 : class SteeringEntity array
	//Parameter 3 : Avoidance radius
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	sf::Vector2f& UnalignedCollisionAvoidance::UCABehavior( class SteeringEntity* _entity, class SteeringEntity* _entities, float _radius )
	{
		sf::Vector2f futurePos = _entity->m_position + _entity->m_velocity;
		sf::Vector2f targetFuturePos;
		sf::Vector2f distance;

		int nbEntities = g_pFZN_SteeringMgr->GetEntitiesNumber();
		class SteeringEntity* targetEntity = nullptr;
		m_steering = sf::Vector2f( 0.f, 0.f );
		for( int i = 0 ; i < nbEntities ; ++i )
		{
			targetEntity = &_entities[i];
			if( _entity != targetEntity )
			{
				targetFuturePos = targetEntity->m_position + targetEntity->m_velocity;
				distance = targetFuturePos - futurePos;
				if( Math::VectorLengthSq( distance ) <= Math::Square( _radius * 2 ) )
				{
					m_steering = Evasion::EvasionBehavior( _entity, targetEntity, 0.02f );
				}
			}
		}

		return m_steering;
	}


	//=========================================================
	//=======================SEPARATION=========================
	//=========================================================

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Avoid collisions with other entities
	//Parameter 1 : Concerned entity
	//Parameter 2 : class SteeringEntity array
	//Parameter 3 : Distance around the entity
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	sf::Vector2f& Separation::Update()
	{
		sf::Vector2f posToEntity;
		int iNbEntities = g_pFZN_SteeringMgr->GetEntitiesNumber();

		class SteeringEntity* targetEntity = nullptr;

		m_steering = sf::Vector2f( 0.f, 0.f );

		for( int i = 0 ; i < iNbEntities ; ++i )
		{
			targetEntity = &m_entities[i];
			if( m_entity != targetEntity )
			{
				posToEntity = m_entity->m_position - targetEntity->m_position;
				if( Math::VectorLengthSq( posToEntity ) <= Math::Square( m_fDistanceMax ) )
				{
					Math::VectorNormalize( posToEntity );
					posToEntity *= m_fDistanceMax;
					m_steering += posToEntity;
				}
			}
		}

		return m_steering;
	}


	//=========================================================
	//========================COHESION==========================
	//=========================================================

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Goes to the other entities around
	//Parameter 1 : Concerned entity
	//Parameter 2 : class SteeringEntity array
	//Parameter 3 : Distance around the entity
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	sf::Vector2f& Cohesion::CohesionBehavior( class SteeringEntity* _entity, class SteeringEntity* _entities, float _distanceMax )
	{
		int iNbEntities = g_pFZN_SteeringMgr->GetEntitiesNumber();
		int iNbNearEntities = 0;
		class SteeringEntity* targetEntity = nullptr;
		sf::Vector2f posToTarget;
		sf::Vector2f averagePos;

		sf::Vector2f steer;

		for( int i = 0 ; i < iNbEntities ; ++i )
		{
			targetEntity = &_entities[i];
			if( _entity != targetEntity )
			{
				posToTarget = _entity->m_position - targetEntity->m_position;
				if( Math::VectorLengthSq( posToTarget ) <= Math::Square( _distanceMax ) )
				{
					iNbNearEntities++;
					averagePos += targetEntity->m_position;
				}
			}
		}

		if( iNbNearEntities > 0 )
		{
			averagePos.x /= iNbNearEntities;
			averagePos.y /= iNbNearEntities;
		}

		m_steering = averagePos - _entity->m_position;

		return m_steering;
	}


	//=========================================================
	//========================ALIGNMENT=========================
	//=========================================================

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Align an entity to the others around
	//Parameter 1 : Concerned entity
	//Parameter 2 : class SteeringEntity array
	//Parameter 3 : Distance around the entity
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	sf::Vector2f& Alignment::AlignmentBehavior( class SteeringEntity* _entity, class SteeringEntity* _entities, float _distanceMax )
	{
		int iNbEntities = g_pFZN_SteeringMgr->GetEntitiesNumber();
		int iNbNearEntities = 0;
		class SteeringEntity* targetEntity = nullptr;
		sf::Vector2f posToTarget;
		sf::Vector2f averageVelocity;

		for( int i = 0 ; i < iNbEntities ; ++i )
		{
			targetEntity = &_entities[i];
			if( _entity != targetEntity )
			{
				posToTarget = _entity->m_position - targetEntity->m_position;
				if( Math::VectorLengthSq( posToTarget ) <= Math::Square( _distanceMax ) )
				{
					iNbNearEntities++;
					averageVelocity += targetEntity->m_velocity;
				}
			}
		}

		if( iNbNearEntities > 0 )
		{
			averageVelocity.x /= iNbNearEntities;
			averageVelocity.y /= iNbNearEntities;
		}

		m_steering = averageVelocity - _entity->m_velocity;

		return m_steering;
	}


	//=========================================================
	//====================LEADER FOLLOWING=======================
	//=========================================================

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Follow a given leader and stay out of its way
	//Parameter 1 : Concerned entity
	//Parameter 2 : Leader to follow
	//Parameter 3 : Avoiding distance around the leader
	//Parameter 4 : Slowing distance when arriving near the leader
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	sf::Vector2f& LeadFollowing::LeadFollowBehavior( class SteeringEntity* _entity, class SteeringEntity* _leader, float _avoidDistance, float _arrivalSlowingDistance )
	{
		sf::Vector2f leaderAhead = _leader->m_position + _leader->m_velocity;
		sf::Vector2f futurePos = _entity->m_position + _entity->m_velocity;

		sf::Vector2f distToLead = leaderAhead - futurePos;

		sf::Vector2f steer;

		if( Math::VectorLengthSq( distToLead ) <= Math::Square( _avoidDistance ) )
			m_steering = Evasion::EvasionBehavior( _entity, _leader, 0.02f );
		else
			m_steering = Arrival::ArrivalBehavior( _entity, _leader->m_position, _arrivalSlowingDistance );

		return m_steering;
	}
} //namespace fzn
