#include "FZNStateMachine.h"


FZNStateMachine::~FZNStateMachine()
{
	for( FZNStateBase* pState : m_oStatePool )
	{
		CheckNullptrDelete( pState );
	}
	m_oStatePool.clear();
}

void FZNStateMachine::Enter( int _iNewState )
{
	if( _iNewState < 0 || _iNewState >= (int)m_oStatePool.size() || m_oStatePool[ _iNewState ] == nullptr )
		return;

	if( m_pCurrentState != nullptr )
		m_pCurrentState->OnExit( _iNewState );

	int iPreviousState = GetCurrentStateID();
	m_pCurrentState = m_oStatePool[ _iNewState ];

	m_pCurrentState->OnEnter( iPreviousState );
}

void FZNStateMachine::Update()
{
	if( m_pCurrentState == nullptr )
		return;

	int iNewState = m_pCurrentState->OnUpdate();

	Enter( iNewState );
}

void FZNStateMachine::Display()
{
	if( m_pCurrentState == nullptr )
		return;

	m_pCurrentState->OnDisplay();
}

int FZNStateMachine::GetCurrentStateID() const
{
	if( m_pCurrentState != nullptr )
		return m_pCurrentState->GetStateID();

	return -1;
}

bool FZNStateMachine::IsCurrentStateID( int _iStateID ) const
{
	if( m_pCurrentState != nullptr )
		return m_pCurrentState->GetStateID() == _iStateID;

	return false;
}

/*template < class EnumState >
bool FZNStateMachine::IsCurrentState( EnumState _eState ) const
{
	return IsCurrentStateID( static_cast< int >( _eState ) );
}
*/