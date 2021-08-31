//------------------------------------------------------------------------
/// Author : Philippe OFFERMANN
/// Date : /
/// Description : Behavior tree base decorator classes
//------------------------------------------------------------------------

#include "FZN/Includes.h"
#include "FZN/Game/BehaviorTree/BTBasicElements.h"
#include "FZN/Game/BehaviorTree/BTDecorators.h"


namespace fzn
{
	void Repeat::SetCount( int _count )
	{
		m_iMaxCount = _count;
	}

	void Repeat::OnInitialize()
	{
		m_iCount = 0;
	}

	BTElement::State Repeat::Update()
	{
		for( ;;)
		{
			m_pChild->Tick();

			if( m_pChild->GetState() == State::Running )
				return State::Running;
			else if( m_pChild->GetState() == State::Failure )
				return State::Failure;

			if( ++m_iCount == m_iMaxCount )
				return State::Success;

			m_pChild->Reset();
		}
		return State::Invalid;
	}


	void Timer::SetTimeLimit( DWORD _seconds )
	{
		m_maxTime = _seconds * 1000;
	}

	void Timer::OnInitialize()
	{
		m_initialTime = GetTickCount();
	}

	BTElement::State Timer::Update()
	{
		for( ;;)
		{
			m_currentTime = GetTickCount();

			if( m_currentTime - m_initialTime >= m_maxTime )
				return State::Success;

			m_pChild->Tick();

			if( m_pChild->GetState() == State::Running )
				return State::Running;
			else if( m_pChild->GetState() == State::Failure )
				return State::Failure;
			else if( m_pChild->GetState() == State::Success )
				return State::Success;

			m_pChild->Reset();
		}
		return State::Invalid;
	}


	void Delayer::SetTargetTime( DWORD _seconds )
	{
		m_targetTime = _seconds * 1000;
	}

	void Delayer::OnInitialize()
	{
		m_initialTime = GetTickCount();
	}

	BTElement::State Delayer::Update()
	{
		for( ;;)
		{
			m_currentTime = GetTickCount();

			if( m_currentTime - m_initialTime >= m_targetTime )
			{
				m_pChild->Tick();

				if( m_pChild->GetState() == State::Running )
					return State::Running;
				else if( m_pChild->GetState() == State::Failure )
					return State::Failure;
				else if( m_pChild->GetState() == State::Success )
					return State::Success;

				m_pChild->Reset();
			}
			return State::Running;
		}
		return State::Invalid;
	}


	BTElement::State Reverse::Update()
	{
		for( ;;)
		{
			m_pChild->Tick();

			if( m_pChild->GetState() == State::Running )
				return State::Running;
			else if( m_pChild->GetState() == State::Failure )
				return State::Success;
			else if( m_pChild->GetState() == State::Success )
				return State::Failure;
		}
		return State::Invalid;
	}
} //namespace fzn
