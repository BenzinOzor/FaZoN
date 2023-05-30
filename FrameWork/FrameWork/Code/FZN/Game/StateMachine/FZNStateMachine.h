#pragma once

#include <vector>

#include "FZN/Defines.h"


class FZNStateMachine;

class FZNStateBase
{
public:
	explicit FZNStateBase() {}
	virtual ~FZNStateBase() {}

	virtual void OnEnter( int _iPreviousStateID ) = 0;
	virtual void OnExit( int _iNextStateID ) = 0;
	virtual int OnUpdate() = 0;
	virtual void OnDisplay() = 0;

	int GetStateID() const { return m_iStateID; }

protected:
	int m_iStateID{ -1 };
};

template< typename T>
class FZNState : public FZNStateBase
{
	friend class FZNStateMachine;

public:
	typedef void( T::* TransitionCallBack )( int );
	typedef int( T::* UpdateCallBack )();
	typedef void( T::* DisplayCallBack )();

	explicit FZNState( T* _pObject, int _iStateID, TransitionCallBack _pOnEnter = nullptr, TransitionCallBack _pOnExit = nullptr, UpdateCallBack _pOnUpdate = nullptr, DisplayCallBack _pOnDisplay = nullptr )
		: FZNStateBase()
		, m_pObject( _pObject )
		, m_pEnterFct( _pOnEnter )
		, m_pExitFct( _pOnExit )
		, m_pUpdateFct( _pOnUpdate )
		, m_pDisplayFct( _pOnDisplay )
	{
		m_iStateID = _iStateID;
	}
	virtual ~FZNState() {}

	virtual void OnEnter( int _iPreviousStateID ) override
	{
		if( m_pObject == nullptr || m_pEnterFct == nullptr )
			return;

		( m_pObject->*m_pEnterFct )( _iPreviousStateID );
	}

	virtual void OnExit( int _iNextStateID ) override
	{
		if( m_pObject == nullptr || m_pExitFct == nullptr )
			return;

		( m_pObject->*m_pExitFct )( _iNextStateID );
	}

	virtual int OnUpdate() override
	{
		if( m_pObject == nullptr || m_pUpdateFct == nullptr )
			return -1;

		return ( m_pObject->*m_pUpdateFct )();
	}

	virtual void OnDisplay() override
	{
		if( m_pObject == nullptr || m_pDisplayFct == nullptr )
			return;

		return ( m_pObject->*m_pDisplayFct )();
	}

protected:
	T*					m_pObject{ nullptr };
	TransitionCallBack	m_pEnterFct{ nullptr };
	TransitionCallBack	m_pExitFct{ nullptr };
	UpdateCallBack		m_pUpdateFct{ nullptr };
	DisplayCallBack		m_pDisplayFct{ nullptr };
};

class FZN_EXPORT FZNStateMachine
{
public:
	~FZNStateMachine();

	void Enter( int _iNewState );
	template < class EnumState >
	void Enter( EnumState _eNewState )
	{
		return Enter( static_cast<int>( _eNewState ) );
	}
	void Update();
	void Display();

	int GetCurrentStateID() const;
	bool IsCurrentStateID( int _iStateID ) const;
	template < class EnumState >
	bool IsCurrentState( EnumState _eState ) const
	{
		return IsCurrentStateID( static_cast<int>( _eState ) );
	}

	template <typename T, class EnumState>
	void CreateState( EnumState _eStateID, typename FZNState<T>::TransitionCallBack _pOnEnter = nullptr, typename FZNState<T>::TransitionCallBack _pOnExit = nullptr, typename FZNState<T>::UpdateCallBack _pOnUpdate = nullptr, typename FZNState<T>::DisplayCallBack _pOnDisplay = nullptr )
	{
		int iState = static_cast< int >( _eStateID );

		if( iState < 0 )
			return;

		if( iState >= (int)m_oStatePool.size() )
			m_oStatePool.resize( iState + 1 );

		if( m_oStatePool[ iState ] != nullptr )
			OverrideStateFunctions<T>( iState, _pOnEnter, _pOnExit, _pOnUpdate, _pOnDisplay );

		T* pObject = dynamic_cast<T*>( this );

		if( pObject == nullptr )
			return;

		m_oStatePool[ iState ] = new FZNState<T>( pObject, iState, _pOnEnter, _pOnExit, _pOnUpdate, _pOnDisplay );
	}

	template <typename T, class EnumState>
	void OverrideStateFunctions( EnumState _eStateID, typename FZNState<T>::TransitionCallBack _pOnEnter = nullptr, typename FZNState<T>::TransitionCallBack _pOnExit = nullptr, typename FZNState<T>::UpdateCallBack _pOnUpdate = nullptr, typename FZNState<T>::DisplayCallBack _pOnDisplay = nullptr )
	{
		int iState = static_cast<int>( _eStateID );

		if( iState < 0 || iState >= (int)m_oStatePool.size() || m_oStatePool[ iState ] == nullptr )
			return;

		FZNState< T >* pState = static_cast<FZNState< T >*>( m_oStatePool[ iState ] );

		if( pState == nullptr )
			return;

		T* pObject = dynamic_cast<T*>( this );

		if( pObject == nullptr )
			return;

		if( _pOnEnter != nullptr )
			pState->m_pEnterFct = _pOnEnter;

		if( _pOnExit != nullptr )
			pState->m_pExitFct = _pOnExit;

		if( _pOnUpdate != nullptr )
			pState->m_pUpdateFct = _pOnUpdate;

		if( _pOnDisplay != nullptr )
			pState->m_pDisplayFct = _pOnDisplay;
	}

protected:
	virtual void _CreateStates() = 0;

	FZNStateBase*					m_pCurrentState{ nullptr };
	std::vector< FZNStateBase* >	m_oStatePool;
};
