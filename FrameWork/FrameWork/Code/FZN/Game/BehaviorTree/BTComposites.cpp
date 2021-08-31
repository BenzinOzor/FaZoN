//------------------------------------------------------------------------
/// Author : Philippe OFFERMANN
/// Date : /
/// Description : Behavior tree base composite classes
//------------------------------------------------------------------------

#include "FZN/Includes.h"
#include "FZN/Game/BehaviorTree/BTBasicElements.h"
#include "FZN/Game/BehaviorTree/BTComposites.h"


namespace fzn
{
	//=========================================================
	///=======================SEQUENCE==========================
	//=========================================================

	//-------------------------------------------------------------------------------------------------
	/// Initializes the Sequence.
	//-------------------------------------------------------------------------------------------------
	void Sequence::OnInitialize()
	{
		m_iCurrentChild = 0;
	}

	//-------------------------------------------------------------------------------------------------
	/// Updates the Sequence.
	/// @return	The Sequence state
	//-------------------------------------------------------------------------------------------------
	BTElement::State Sequence::Update()
	{
		BTElement::State childStatus;

		while( 1 )
		{
			childStatus = m_oChildren[m_iCurrentChild]->Tick();

			if( childStatus != State::Success )
				return childStatus;

			if( ++m_iCurrentChild >= (int)m_oChildren.size() )
				return State::Success;
		}
	}


	//=========================================================
	///=======================SELECTOR==========================
	//=========================================================

	//-------------------------------------------------------------------------------------------------
	/// Initializes the Selector.
	//-------------------------------------------------------------------------------------------------
	void Selector::OnInitialize()
	{
		m_iCurrentChild = 0;
	}

	//-------------------------------------------------------------------------------------------------
	/// Updates the Selector.
	/// @return	The Selector state.
	//-------------------------------------------------------------------------------------------------
	BTElement::State Selector::Update()
	{
		BTElement::State childStatus;

		while( 1 )
		{
			childStatus = m_oChildren[m_iCurrentChild]->Tick();

			if( childStatus != State::Failure )
				return childStatus;

			if( ++m_iCurrentChild >= (int)m_oChildren.size() )
				return State::Success;
		}
	}


	//=========================================================
	///===================RANDOM SELECTOR======================
	//=========================================================
	
	//-------------------------------------------------------------------------------------------------
	/// Initializes the Selector.
	//-------------------------------------------------------------------------------------------------
	void RandomSelector::OnInitialize()
	{
		m_iCurrentChild = Rand( 0, m_oChildren.size() );
	}

	//-------------------------------------------------------------------------------------------------
	/// Updates the Selector.
	/// @return	The Selector state.
	//-------------------------------------------------------------------------------------------------
	BTElement::State RandomSelector::Update()
	{
		if( m_iCurrentChild >= m_oChildren.size() )
			return State::Failure;

		return m_oChildren[ m_iCurrentChild ]->Tick();
	}

	void Loop::OnInitialize()
	{
		m_iCurrentChild = 0;
	}

	BTElement::State Loop::Update()
	{
		BTElement::State childStatus;

		while( 1 )
		{
			childStatus = m_oChildren[ m_iCurrentChild ]->Tick();

			if( childStatus == State::Failure )
			{
				m_iCurrentChild = 0;
				return State::Running;
			}

			if( childStatus != State::Success )
				return childStatus;

			if( ++m_iCurrentChild >= (int)m_oChildren.size() )
				return State::Success;
		}
	}

} //namespace fzn
