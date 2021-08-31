//------------------------------------------------------------------------
//Author : Philippe OFFERMANN
//Date : 13.02.15
//Description : Steering behaviors
//------------------------------------------------------------------------

#ifndef _STEERINGBEHAVIORS_H_
#define _STEERINGBEHAVIORS_H_

#include "FZN/Game/Steering/SteeringEntity.h"


namespace fzn
{
	class SteeringEntity;
	class SteeringObject;

	//=========================================================
	//===========================SEEK===========================
	//=========================================================

	class FZN_EXPORT Seek : public SteeringBehavior
	{
	public:
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Seeks a given target position
		//Parameter 1 : Concerned entity
		//Parameter 2 : class SteeringEntity to reach
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		Seek( class SteeringEntity* _entity, class SteeringEntity* _target ) : SteeringBehavior( _entity ), m_target( _target ){};
		sf::Vector2f& Update() { m_steering = SeekBehavior( m_entity, m_target->m_position ); return m_steering; };

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Seeks a given target position
		//Parameter 1 : Concerned entity
		//Parameter 2 : Position to reach
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		static sf::Vector2f SeekBehavior( class SteeringEntity* _entity, const sf::Vector2f& _targetPos );

	private:
		class SteeringEntity* m_target;		//Target to seek
	};


	//=========================================================
	//===========================FLEE===========================
	//=========================================================

	class FZN_EXPORT Flee : public SteeringBehavior
	{
	public:
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Flees a given target position
		//Parameter 1 : Concerned entity
		//Parameter 2 : class SteeringEntity to flee
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		Flee( class SteeringEntity* _entity, class SteeringEntity* _target ) : SteeringBehavior( _entity ), m_target( _target ){};
		sf::Vector2f& Update() { m_steering = FleeBehavior( m_entity, m_target->m_position ); return m_steering; };

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Flees a given target position
		//Parameter 1 : Concerned entity
		//Parameter 2 : Position to flee
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		static sf::Vector2f& FleeBehavior( class SteeringEntity* _entity, sf::Vector2f& _targetPos );

	private:
		class SteeringEntity* m_target;		//Target to flee
	};


	//=========================================================
	//=========================PURSUIT==========================
	//=========================================================

	class FZN_EXPORT Pursuit : public SteeringBehavior
	{
	public:
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Pursue a given target
		//Parameter 1 : Concerned entity
		//Parameter 2 : Target to reach
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		Pursuit( class SteeringEntity* _entity, class SteeringEntity* _target, float _c ) : SteeringBehavior( _entity ), m_target( _target ), m_fC( _c ){};
		sf::Vector2f& Update() { m_steering = PursuitBehavior( m_entity, m_target, m_fC ); return m_steering; };

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Pursue a given target
		//Parameter 1 : Concerned entity
		//Parameter 2 : Target to reach
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		static sf::Vector2f& PursuitBehavior( class SteeringEntity* _entity, class SteeringEntity* _target, float _c );

	private:
		class SteeringEntity* m_target;		//Target to pursue
		float m_fC;
	};


	//=========================================================
	//=========================EVASION==========================
	//=========================================================

	class FZN_EXPORT Evasion : public SteeringBehavior
	{
	public:
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Avoids a given target
		//Parameter 1 : Concerned entity
		//Parameter 2 : Target to avoid
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		Evasion( class SteeringEntity* _entity, class SteeringEntity* _target, float _c ) : SteeringBehavior( _entity ), m_target( _target ), m_fC( _c ){};
		sf::Vector2f& Update() { m_steering = EvasionBehavior( m_entity, m_target, m_fC ); return m_steering; };

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Avoids a given target
		//Parameter 1 : Concerned entity
		//Parameter 2 : Target to avoid
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		static sf::Vector2f& EvasionBehavior( class SteeringEntity* _entity, class SteeringEntity* _target, float _c );

	private:
		class SteeringEntity* m_target;		//Target to avoid
		float m_fC;
	};


	//=========================================================
	//=========================ARRIVAL==========================
	//=========================================================

	class FZN_EXPORT Arrival : public SteeringBehavior
	{
	public:
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Goes to a given position and stops
		//Parameter 1 : Concerned entity
		//Parameter 2 : class SteeringEntity to reach
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		Arrival( class SteeringEntity* _entity, class SteeringEntity* _target, float _slowingDistance ) : SteeringBehavior( _entity ), m_target( _target ), m_fSlowingDistance( _slowingDistance ){};
		sf::Vector2f& Update() { m_steering = ArrivalBehavior( m_entity, m_target->m_position, m_fSlowingDistance );  return m_steering; };

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Goes to a given position and stops
		//Parameter 1 : Concerned entity
		//Parameter 2 : Position to reach
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		static sf::Vector2f ArrivalBehavior( class SteeringEntity* _entity, const sf::Vector2f& _targetPos, float _slowingDistance );

	private:
		class SteeringEntity* m_target;				//Target to reach
		float m_fSlowingDistance;		//Distance under which the entity will slow down
	};


	//=========================================================
	//====================OBSTACLE AVOIDACE======================
	//=========================================================

	class FZN_EXPORT ObstacleAvoidance : public SteeringBehavior
	{
	public:
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Avoids the given obstacles
		//Parameter 1 : Concerned class SteeringEntity
		//Parameter 2 : Avoidance radius
		//Parameter 3 : Viewing distance
		//Parameter 4 : Obstacles to avoid
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		ObstacleAvoidance( class SteeringEntity* _entity, float _avoidingRadius, float _farView, SteeringObject* _obstacleArray )
			: SteeringBehavior( _entity ), m_fRadius( _avoidingRadius ), m_farView( _farView ), m_obstacles( _obstacleArray ){};
		sf::Vector2f& Update();

	private:
		float m_fRadius;			//Radius in which the entity will avoid obstacles
		float m_farView;			//Distance in front of the entity in which it will avoid obstacles
		SteeringObject* m_obstacles;		//Obstacle array
	};


	//=========================================================
	//=========================WANDER==========================
	//=========================================================

	class FZN_EXPORT Wander : public SteeringBehavior
	{
	public:
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Moves around randomly
		//Parameter 1 : Concerned entity
		//Parameter 2 : Distance between the circle at the front of the entity and itself
		//Parameter 3 : Radius of the circle at the front of the entity
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		Wander( class SteeringEntity* _entity, float _distance, float _radius ) : SteeringBehavior( _entity ), m_fDistance( _distance ), m_fRadius( _radius ){};
		sf::Vector2f& Update() { m_steering = WanderBehavior( m_entity, m_fDistance, m_fRadius );  return m_steering; };

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Moves around randomly
		//Parameter 1 : Concerned entity
		//Parameter 2 : Distance between the circle at the front of the entity and itself
		//Parameter 3 : Radius of the circle at the front of the entity
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		sf::Vector2f& WanderBehavior( class SteeringEntity* _entity, float _fDistance, float _fRadius );

	private:
		float m_fDistance;			//Position of the circle at front of the entity
		float m_fRadius;			//Radius of the circle in front of the entity
	};


	//=========================================================
	//=====================PATH FOLLOWING=======================
	//=========================================================

	class FZN_EXPORT PathFollowing : public SteeringBehavior
	{
	public:
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Follow a path into the given obstacles
		//Parameter 1 : Concerned class SteeringEntity
		//Parameter 2 : Obstacles to go through
		//Parameter 3 : Number of obstacles
		//Parameter 4 : Once at the end, the entity goes back to the beginning (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		PathFollowing( SteeringEntity* _entity, SteeringObject* _obstacleArray, int _nbObstacles, UINT8 _roundTrip )
			: SteeringBehavior( _entity ), m_obstacles( _obstacleArray ), m_iCurrentObstacle( 0 ), m_iNbObstacles( _nbObstacles ), m_roundTrip( _roundTrip ), m_iDirection( 1 ){};
		sf::Vector2f& Update();

	private:
		SteeringObject * m_obstacles;		//Obstacle array
		int m_iCurrentObstacle;	//Current target
		int m_iNbObstacles;
		UINT8 m_roundTrip;		//Once at the end, the entity goes back to the beginning (true) or not
		int m_iDirection;			//Current way to go through the obstacles (1 or -1)
	};


	//=========================================================
	//==========================UCA============================
	//=========================================================

	class FZN_EXPORT UnalignedCollisionAvoidance : public SteeringBehavior
	{
	public:
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Avoid collisions with other entities
		//Parameter 1 : Concerned entity
		//Parameter 2 : class SteeringEntity array
		//Parameter 3 : Avoidance radius
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		UnalignedCollisionAvoidance( class SteeringEntity* _entity, class SteeringEntity* _entities, float _radius ) : SteeringBehavior( _entity ), m_entities( _entities ), m_fRadius( _radius ){};
		sf::Vector2f& Update(){ m_steering = UCABehavior( m_entity, m_entities, m_fRadius ); return m_steering; };

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Avoid collisions with other entities
		//Parameter 1 : Concerned entity
		//Parameter 2 : class SteeringEntity array
		//Parameter 3 : Avoidance radius
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		sf::Vector2f& UCABehavior( class SteeringEntity* _entity, class SteeringEntity* _entities, float _radius );

	private:
		class SteeringEntity* m_entities;				//class SteeringEntity array
		float m_fRadius;				//Distance under which the entity will slow down
	};


	//=========================================================
	//=======================SEPARATION=========================
	//=========================================================

	class FZN_EXPORT Separation : public SteeringBehavior
	{
	public:
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Avoid collisions with other entities
		//Parameter 1 : Concerned entity
		//Parameter 2 : class SteeringEntity array
		//Parameter 3 : Distance around the entity
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		Separation( class SteeringEntity* _entity, class SteeringEntity* _entities, float _distanceMax ) : SteeringBehavior( _entity ), m_entities( _entities ), m_fDistanceMax( _distanceMax ){};
		sf::Vector2f& Update();

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Avoid collisions with other entities
		//Parameter 1 : Concerned entity
		//Parameter 2 : class SteeringEntity array
		//Parameter 3 : Distance around the entity
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		sf::Vector2f& SeparationBehavior( class SteeringEntity* _entity, class SteeringEntity* _entities, float _distanceMax );

	private:
		class SteeringEntity* m_entities;				//class SteeringEntity array
		float m_fDistanceMax;			//Distance around the entity 
	};


	//=========================================================
	//========================COHESION==========================
	//=========================================================

	class FZN_EXPORT Cohesion : public SteeringBehavior
	{
	public:
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Goes to the other entities around
		//Parameter 1 : Concerned entity
		//Parameter 2 : class SteeringEntity array
		//Parameter 3 : Distance around the entity
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		Cohesion( class SteeringEntity* _entity, class SteeringEntity* _entities, float _distanceMax ) : SteeringBehavior( _entity ), m_entities( _entities ), m_fDistanceMax( _distanceMax ){};
		sf::Vector2f& Update(){ m_steering = CohesionBehavior( m_entity, m_entities, m_fDistanceMax ); return m_steering; };

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Goes to the other entities around
		//Parameter 1 : Concerned entity
		//Parameter 2 : class SteeringEntity array
		//Parameter 3 : Distance around the entity
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		sf::Vector2f& CohesionBehavior( class SteeringEntity* _entity, class SteeringEntity* _entities, float _distanceMax );

	private:
		class SteeringEntity* m_entities;				//class SteeringEntity array
		float m_fDistanceMax;			//Distance around the entity 
	};


	//=========================================================
	//========================ALIGNMENT=========================
	//=========================================================

	class FZN_EXPORT Alignment : public SteeringBehavior
	{
	public:
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Align an entity to the others around
		//Parameter 1 : Concerned entity
		//Parameter 2 : class SteeringEntity array
		//Parameter 3 : Distance around the entity
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		Alignment( class SteeringEntity* _entity, class SteeringEntity* _entities, float _distanceMax ) : SteeringBehavior( _entity ), m_entities( _entities ), m_fDistanceMax( _distanceMax ){};
		sf::Vector2f& Update(){ m_steering = AlignmentBehavior( m_entity, m_entities, m_fDistanceMax ); return m_steering; };

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Align an entity to the others around
		//Parameter 1 : Concerned entity
		//Parameter 2 : class SteeringEntity array
		//Parameter 3 : Distance around the entity
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		sf::Vector2f& AlignmentBehavior( class SteeringEntity* _entity, class SteeringEntity* _entities, float _distanceMax );

	private:
		class SteeringEntity* m_entities;				//class SteeringEntity array
		float m_fDistanceMax;			//Distance around the entity 
	};


	//=========================================================
	//====================LEADER FOLLOWING=======================
	//=========================================================

	class FZN_EXPORT LeadFollowing : public SteeringBehavior
	{
	public:
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Follow a given leader and stay out of its way
		//Parameter 1 : Concerned entity
		//Parameter 2 : Leader to follow
		//Parameter 3 : Avoiding distance around the leader
		//Parameter 4 : Slowing distance when arriving near the leader
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		LeadFollowing( class SteeringEntity* _entity, class SteeringEntity* _leader, float _avoidDistance, float _arrivalSlowingDistance )
			: SteeringBehavior( _entity ), m_lead( _leader ), m_fAvoidDistance( _avoidDistance ), m_fArrivalDistance( _arrivalSlowingDistance ){};
		sf::Vector2f& Update(){ m_steering = LeadFollowBehavior( m_entity, m_lead, m_fAvoidDistance, m_fArrivalDistance ); return m_steering; };

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Follow a given leader and stay out of its way
		//Parameter 1 : Concerned entity
		//Parameter 2 : Leader to follow
		//Parameter 3 : Avoiding distance around the leader
		//Parameter 4 : Slowing distance when arriving near the leader
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		sf::Vector2f& LeadFollowBehavior( class SteeringEntity* _entity, class SteeringEntity* _leader, float _avoidDistance, float _arrivalSlowingDistance );

	private:
		class SteeringEntity* m_lead;					//class SteeringEntity to follow
		float m_fAvoidDistance;		//Avoiding distance (to stay out of the way of the leader)
		float m_fArrivalDistance;		//Slowing distance for arrival
	};
} //namespace fzn

#endif //_STEERINGBEHAVIORS_H_