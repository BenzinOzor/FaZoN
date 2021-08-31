//------------------------------------------------------------------------
/// Author : Philippe OFFERMANN
/// Date : /
/// Description : Behavior tree base classes
//------------------------------------------------------------------------

#include "FZN/Includes.h"
#include "FZN/Game/BehaviorTree/BTBasicElements.h"


namespace fzn
{
	//=========================================================
	///=======================BTELEMENT========================
	//=========================================================

	//-------------------------------------------------------------------------------------------------
	/// Default constructor.
	//-------------------------------------------------------------------------------------------------
	BTElement::BTElement() 
	: m_eState( State::Invalid )
	{
	}


	//-------------------------------------------------------------------------------------------------
	/// Manages the calls to the initializing, updating and terminating functions
	/// @return	The current state of the BTElement.
	//-------------------------------------------------------------------------------------------------
	BTElement::State BTElement::Tick()
	{
		if( m_eState != State::Running )
			OnInitialize();

		m_eState = Update();

		if( m_eState != State::Running )
			OnTerminate( m_eState );

		return m_eState;
	}

	//-------------------------------------------------------------------------------------------------
	/// Resets the BTElement.
	//-------------------------------------------------------------------------------------------------
	void BTElement::Reset()
	{
		m_eState = State::Invalid;
	}

	//-------------------------------------------------------------------------------------------------
	/// Aborts the BTElement and calls OnTerminate.
	//-------------------------------------------------------------------------------------------------
	void BTElement::Abort()
	{
		OnTerminate( State::Aborted );
		m_eState = State::Aborted;
	}


	//-------------------------------------------------------------------------------------------------
	/// Indicates whether or not the BTElement is terminated.
	/// @return	If true, the BTElement is terminated (Status = success or failure).
	//-------------------------------------------------------------------------------------------------
	bool BTElement::IsTerminated() const
	{
		return m_eState == State::Success || m_eState == State::Failure;
	}

	//-------------------------------------------------------------------------------------------------
	/// Indicates whether or not the BTElement is running.
	/// @return	If true, the BTElement is running.
	//-------------------------------------------------------------------------------------------------
	bool BTElement::IsRunning() const
	{
		return m_eState == State::Running;
	}

	//-------------------------------------------------------------------------------------------------
	/// Gets the current state of the BTElement.
	/// @return	The BTElement state.
	//-------------------------------------------------------------------------------------------------
	BTElement::State BTElement::GetState() const
	{
		return m_eState;
	}


	//=========================================================
	///=======================COMPOSITE========================
	//=========================================================

	Decorator::~Decorator()
	{
		CheckNullptrDelete( m_pChild );
	}

	void Decorator::SetChild( BTElement* _pChild )
	{
		m_pChild = _pChild;
	}


	Composite::Composite()
	: m_iCurrentChild( 0 )
	{

	}

	//=========================================================
	///=======================COMPOSITE========================
	//=========================================================

	Composite::~Composite()
	{
		for( BTElement* pChild : m_oChildren )
		{
			CheckNullptrDelete( pChild );
		}
	}

	//-------------------------------------------------------------------------------------------------
	/// Adds a child in the Composite.
	/// @param [in]	_pChild	: BTElement child of the Composite
	//-------------------------------------------------------------------------------------------------
	void Composite::AddChild( BTElement* _pChild )
	{
		m_oChildren.push_back( _pChild );
	}

	//-------------------------------------------------------------------------------------------------
	/// Removes the given child from the Composite
	/// @param [in]	_pChild	: BTElement child to remove.
	//-------------------------------------------------------------------------------------------------
	void Composite::RemoveChild( BTElement* _pChild )
	{
		for( std::vector< BTElement* >::iterator it = m_oChildren.begin() ; it != m_oChildren.end(); ++it )
		{
			if( (*it) == _pChild )
			{
				m_oChildren.erase( it );
				return;
			}
		}
	}

	//-------------------------------------------------------------------------------------------------
	/// Clears the children of the Composite.
	//-------------------------------------------------------------------------------------------------
	void Composite::ClearChildren()
	{
		m_oChildren.clear();
	}

} //namespace fzn
