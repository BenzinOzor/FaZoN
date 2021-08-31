//------------------------------------------------------------------------
//Author : Philippe OFFERMANN
//Date : 13.02.15
//Description : Steering formations
//------------------------------------------------------------------------

#include "FZN/Includes.h"
#include "FZN/Game/Steering/SteeringBehavior.h"
#include "FZN/Game/Steering/SteeringBehaviors.h"
#include "FZN/Game/Steering/SteeringEntity.h"
#include "FZN/Managers/SteeringManager.h"
#include "FZN/Game/Steering/SteeringFormations.h"


namespace fzn
{
	//=========================================================
	//==========================CIRCLE==========================
	//=========================================================

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Circle formation around a given leader
	//Parameter 1 : Concerned entity
	//Parameter 2 : Leader
	//Parameter 3 : SteeringEntity's ID in the formation
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	sf::Vector2f& CircleFormation::Update()
	{
		int iEntityCount = g_pFZN_SteeringMgr->GetEntitiesNumber();
		m_steering = sf::Vector2f( 0.f, 0.f );

		if( m_leader->m_mass != 0.f )
			iEntityCount--;

		float fCircleRadius = iEntityCount * 5.f;
		sf::Vector2f circlePos = m_leader->m_position;

		float fAngleToEntity = (float)( ( m_ID * 360 ) / iEntityCount );
		fAngleToEntity = Math::DegToRad( fAngleToEntity );

		sf::Vector2f targetPos;
		targetPos.x = circlePos.x + fCircleRadius * cosf( fAngleToEntity );
		targetPos.y = circlePos.y + fCircleRadius * sinf( fAngleToEntity );

		m_steering = Arrival::ArrivalBehavior( m_entity, targetPos, 100.f );

		return m_steering;
	}


	//=========================================================
	//=========================VSHAPE==========================
	//=========================================================

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Circle formation around a given leader
	//Parameter 1 : Concerned entity
	//Parameter 2 : Leader
	//Parameter 3 : SteeringEntity's ID in the formation
	//Parameter 4 : Offset between entities
	//Parameter 5 : Angle of the formation
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	sf::Vector2f& VFormation::Update()
	{
		sf::Vector2f lineCenter = m_leader->m_position;

		sf::Vector2f startPos = m_leader->m_velocity * -1.f;
		Math::VectorNormalize( startPos );

		int iOffsetPos = m_ID;
		INT8 isIDOdd = iOffsetPos & 0x1;
		float fDirModifier = ( isIDOdd ) ? -1.f : 1.f;

		sf::Vector2f direction = startPos;
		Math::VectorRotateR( direction, fDirModifier * ( m_fAngle * 0.5f ) );

		if( isIDOdd )
			iOffsetPos--;

		iOffsetPos *= ( int )0.5f;

		startPos *= m_fOffset;
		startPos += lineCenter;
		direction *= iOffsetPos * m_fOffset;

		sf::Vector2f targetPos = startPos + direction;

		m_steering = Arrival::ArrivalBehavior( m_entity, targetPos, 100.f );

		return m_steering;
	}


	//=========================================================
	//===========================LINE===========================
	//=========================================================

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Circle formation around a given leader
	//Parameter 1 : Concerned entity
	//Parameter 2 : Leader
	//Parameter 3 : SteeringEntity's ID in the formation
	//Parameter 4 : Offset between entities
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	sf::Vector2f& LineFormation::Update()
	{
		sf::Vector2f lineCenter = m_leader->m_position;

		sf::Vector2f startPos = m_leader->m_velocity * -1.f;
		Math::VectorNormalize( startPos );

		int iOffsetPos = m_ID;
		INT8 isIDOdd = iOffsetPos & 0x1;
		float fDirModifier = ( isIDOdd ) ? -1.f : 1.f;

		sf::Vector2f direction = startPos;
		Math::VectorRotateR( direction, fDirModifier * Math::PIdiv2 );

		if( isIDOdd )
			iOffsetPos--;

		iOffsetPos *= ( int )0.5f;

		startPos *= m_fOffset;
		startPos += lineCenter;
		direction *= iOffsetPos * m_fOffset;

		sf::Vector2f targetPos = startPos + direction;

		m_steering = Arrival::ArrivalBehavior( m_entity, targetPos, 100.f );

		return m_steering;
	}


	//=========================================================
	//========================MULTILINE==========================
	//=========================================================

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Circle formation around a given leader
	//Parameter 1 : Concerned entity
	//Parameter 2 : Leader
	//Parameter 3 : SteeringEntity's ID in the formation
	//Parameter 4 : Offset between entities
	//Parameter 5 : Entities on each line
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	sf::Vector2f& MultilineFormation::Update()
	{
		sf::Vector2f lineCenter = m_leader->m_position;

		int iEntityCount = g_pFZN_SteeringMgr->GetEntitiesNumber();
		if( m_leader->m_mass != 0.f )
			iEntityCount--;

		sf::Vector2f startPos = m_leader->m_velocity * -1.0f;
		Math::VectorNormalize( startPos );

		int iOffsetPos = m_ID;
		float fLineOffset = 0;
		int iNumberOfLines = iEntityCount / m_iEntitiesPerLine;

		if( iEntityCount % m_iEntitiesPerLine == 0 )
			iNumberOfLines--;

		int iEntitiesTillLastLine = iNumberOfLines * m_iEntitiesPerLine;
		int iEntitiesRemaining = iEntityCount - iEntitiesTillLastLine;


		// If the ID is higher than the number of entities per line, a new line has to be done.
		if( m_ID > m_iEntitiesPerLine )
		{
			fLineOffset = (float)( ( m_ID - 1 ) / m_iEntitiesPerLine );
			iOffsetPos -= (int)fLineOffset * m_iEntitiesPerLine;
		}

		// Parity is calculated from CoefficientOffset because an ID firstly even can become odd with the latest operation (with 5 entities per line, 6 becomes 1).
		int isIDOdd = iOffsetPos & 0x1;

		float fDirModifier = ( isIDOdd ) ? -1.0f : 1.0f;

		sf::Vector2f Direction = startPos;

		// If the number of entities per line is even, the line center must be moved.
		// The test is done here because we need the direction vector and it avoids to do another normalization on the temporary vector.
		// After would be too late because Direction is rotated (maybe not in an interesting way).
		if( ( m_ID <= iEntitiesTillLastLine && !( m_iEntitiesPerLine & 0x1 ) ) || ( m_ID > iEntitiesTillLastLine && !( iEntitiesRemaining & 0x1 ) ) )
		{
			sf::Vector2f tempDir = Direction;
			Math::VectorRotateR( tempDir, -1 * Math::PIdiv2 );		// This vector is rotated in the direction of the odd entity IDs (if leader is going down, rotation to the left of the screen).
			tempDir *= ( m_fOffset / 2.f );				// The vector is scaled to half the size of the offset there is between entities so the center is halfway this offset.
			lineCenter += tempDir;
		}

		Math::VectorRotateR( Direction, fDirModifier * Math::PIdiv2 );

		if( isIDOdd )
			iOffsetPos--;

		iOffsetPos /= 2;

		startPos *= ( m_fOffset * ( 1 + fLineOffset ) );
		startPos += lineCenter;
		sf::Vector2f ArrivalTarget = startPos + Direction * ( (float)iOffsetPos * m_fOffset );

		m_steering = Arrival::ArrivalBehavior( m_entity, ArrivalTarget, 100.f );

		return m_steering;
	}
} //namespace fzn
