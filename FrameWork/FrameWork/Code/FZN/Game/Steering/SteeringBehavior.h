//------------------------------------------------------------------------
//Author : Philippe OFFERMANN
//Date : 13.02.15
//Description : Steering behaviors base class
//------------------------------------------------------------------------

#ifndef _STEERINGBEHAVIOR_H_
#define _STEERINGBEHAVIOR_H_


namespace fzn
{
	class SteeringEntity;

	//=========================================================
	//=========================BEHAVIOR=========================
	//=========================================================

	class FZN_EXPORT SteeringBehavior
	{
	public:
		/////////////////CONSTRUCTOR / DESTRUCTOR/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Parametered constructor
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		SteeringBehavior(SteeringEntity* _entity);
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Destructor
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		~SteeringBehavior();


		/////////////////BEHAVIOR MANAGEMENT/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Update of the behavior
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		virtual sf::Vector2f& Update() = 0;

	protected:
		/////////////////MEMBER VARIABLES/////////////////

		SteeringEntity* m_entity;			//class SteeringEntity on which the behavior is applied
		sf::Vector2f m_steering;	//Steer force to apply on the entity
	};


	//=========================================================
	//===================WEIGHTEDBEHAVIOR=======================
	//=========================================================

	struct WeightedBehavior
	{
		/////////////////CONSTRUCTOR / DESTRUCTOR/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Default constructor
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		WeightedBehavior()
		{
			behavior = nullptr;
			weight = 0.f;
		}
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Parametered constructor
		//Parameter 1 : Chosen behavior
		//Parameter 2 : SteeringBehavior's weight
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		WeightedBehavior(SteeringBehavior* _behavior, float _weight)
		{
			behavior = _behavior;
			weight = _weight;
		}


		/////////////////MEMBER VARIABLES/////////////////

		float weight;
		SteeringBehavior* behavior;
	};
} //namespace fzn

#endif //_STEERINGBEHAVIOR_H_
