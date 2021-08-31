//------------------------------------------------------------------------
//Author : Philippe OFFERMANN
//Date : 13.02.15
//Description : Steering formations
//------------------------------------------------------------------------

#ifndef _FORMATIONS_H_
#define _FORMATIONS_H_


namespace fzn
{
	//=========================================================
	//========================FORMATION========================
	//=========================================================

	class FZN_EXPORT SteeringFormation : public SteeringBehavior
	{
	public:
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Seeks a given target position
		//Parameter 1 : Concerned entity
		//Parameter 2 : Leader to follow
		//Parameter 3 : ID of the entity in the formation
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		SteeringFormation(SteeringEntity* _entity, SteeringEntity* _leader, int _ID) : SteeringBehavior(_entity), m_leader(_leader), m_ID(_ID) {};

	protected:
		SteeringEntity* m_leader;		//Leader to follow
		int m_ID;				//SteeringEntity's ID in the formation
	};


	//=========================================================
	//==========================CIRCLE==========================
	//=========================================================

	class FZN_EXPORT CircleFormation : public SteeringFormation
	{
	public:
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Circle formation around a given leader
		//Parameter 1 : Concerned entity
		//Parameter 2 : Leader
		//Parameter 3 : SteeringEntity's ID in the formation
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		CircleFormation(SteeringEntity* _entity, SteeringEntity* _leader, int _ID) : SteeringFormation(_entity, _leader, _ID){};
		sf::Vector2f& Update();
	};


	//=========================================================
	//=========================VSHAPE==========================
	//=========================================================

	class FZN_EXPORT VFormation : public SteeringFormation
	{
	public:
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Circle formation around a given leader
		//Parameter 1 : Concerned entity
		//Parameter 2 : Leader
		//Parameter 3 : SteeringEntity's ID in the formation
		//Parameter 4 : Offset between entities
		//Parameter 5 : Angle of the formation
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		VFormation(SteeringEntity* _entity, SteeringEntity* _leader, int _ID, float _offset, float _angle)
			: SteeringFormation(_entity, _leader, _ID), m_fOffset(_offset), m_fAngle(_angle){};
		sf::Vector2f& Update();

	private:
		float m_fOffset;		//Offset between entities
		float m_fAngle;		//Angle of the formation
	};


	//=========================================================
	//===========================LINE===========================
	//=========================================================

	class FZN_EXPORT LineFormation : public SteeringFormation
	{
	public:
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Circle formation around a given leader
		//Parameter 1 : Concerned entity
		//Parameter 2 : Leader
		//Parameter 3 : SteeringEntity's ID in the formation
		//Parameter 4 : Offset between entities
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		LineFormation(SteeringEntity* _entity, SteeringEntity* _leader, int _ID, float _offset)
			: SteeringFormation(_entity, _leader, _ID), m_fOffset(_offset){};
		sf::Vector2f& Update();

	private:
		float m_fOffset;		//Offset between entities
	};


	//=========================================================
	//========================MULTILINE==========================
	//=========================================================

	class FZN_EXPORT MultilineFormation : public SteeringFormation
	{
	public:
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Circle formation around a given leader
		//Parameter 1 : Concerned entity
		//Parameter 2 : Leader
		//Parameter 3 : SteeringEntity's ID in the formation
		//Parameter 4 : Offset between entities
		//Parameter 5 : Entities on each line
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		MultilineFormation(SteeringEntity* _entity, SteeringEntity* _leader, int _ID, float _offset, int _entitiesPerLine)
			: SteeringFormation(_entity, _leader, _ID), m_fOffset(_offset), m_iEntitiesPerLine(_entitiesPerLine){};
		sf::Vector2f& Update();

	private:
		float m_fOffset;				//Offset between entities
		int m_iEntitiesPerLine;		//Entities on each line
	};
} //namespace fzn

#endif //_FORMATION_H_