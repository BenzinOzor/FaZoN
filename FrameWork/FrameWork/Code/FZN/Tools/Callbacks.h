#pragma once

#include <vector>

namespace fzn
{
	class CallbackBase
	{
	public:
		virtual void Call() = 0;
	};

	typedef std::vector<CallbackBase*> CallBacksVector;

	template< typename T>
	class MemberCallback : public CallbackBase
	{
	public:
		typedef void ( T::* CallbackFct )();
		MemberCallback( CallbackFct _pCallback, T* _pObject )
			: CallbackBase()
		{
			m_pCallback = _pCallback;
			m_pObject = _pObject;
		}

		virtual void Call()
		{
			( m_pObject->*m_pCallback )();
		}

		CallbackFct		m_pCallback;
		T*				m_pObject;
	};

	template< typename T, typename A >
	class Member1ArgCallback : public CallbackBase
	{
	public:
		typedef void ( T::* CallbackFct )( A );
		Member1ArgCallback( CallbackFct _pCallback, T* _pObject, A _oArg )
			: CallbackBase()
			, m_oArg( _oArg )
		{
			m_pCallback = _pCallback;
			m_pObject = _pObject;
		}

		virtual void Call()
		{
			( m_pObject->*m_pCallback )( m_oArg );
		}

		CallbackFct		m_pCallback;
		T*				m_pObject;
		A				m_oArg;
	};

	template< typename T, typename A, typename B >
	class Member2ArgCallback : public CallbackBase
	{
	public:
		typedef void ( T::* CallbackFct )( A, B );
		Member2ArgCallback( CallbackFct _pCallback, T* _pObject, A _oArg, B _oArg2 )
			: CallbackBase()
			, m_oArg( _oArg )
			, m_oArg2( _oArg2 )
		{
			m_pCallback = _pCallback;
			m_pObject = _pObject;
		}

		virtual void Call()
		{
			( m_pObject->*m_pCallback )( m_oArg, m_oArg2 );
		}

		CallbackFct		m_pCallback;
		T*				m_pObject;
		A				m_oArg;
		B				m_oArg2;
	};

	template< typename A >
	class Member1DynArgCallbackBase
	{
	public:
		Member1DynArgCallbackBase() {}
		virtual void Call( A /*oArg*/ ) = 0;

		virtual ~Member1DynArgCallbackBase() {}
	private:
		Member1DynArgCallbackBase& operator=( const Member1DynArgCallbackBase& );
	};

	template< typename T, typename A >
	class Member1DynArgCallback : public Member1DynArgCallbackBase<A>
	{
	public:
		typedef void( T::* CallbackFct )( A );
		Member1DynArgCallback( CallbackFct _pCallback, T* _pObject )
			: Member1DynArgCallbackBase<A>()
		{
			m_pCallback = _pCallback;
			m_pObject = _pObject;
		}

		virtual void Call( A oArg )
		{
			return ( m_pObject->*m_pCallback )( oArg );
		}

		CallbackFct		m_pCallback;
		T*				m_pObject;
	};

	template< typename A, typename B >
	class Member2DynArgCallbackBase
	{
	public:
		Member2DynArgCallbackBase() {}
		virtual void Call( A /*oArgA*/, B /*oArgB*/ ) = 0;

	private:
		Member2DynArgCallbackBase& operator=( const Member2DynArgCallbackBase& );
	};

	template< typename T, typename A, typename B >
	class Member2DynArgCallback : public Member2DynArgCallbackBase< A, B >
	{
	public:
		typedef void( T::* CallbackFct )( A, B );
		Member2DynArgCallback( CallbackFct _pCallback, T* _pObject )
			: Member2DynArgCallbackBase< A, B >()
		{
			m_pCallback = _pCallback;
			m_pObject = _pObject;
		}

		virtual void Call( A oArgA, B oArgB )
		{
			return ( m_pObject->*m_pCallback )( oArgA, oArgB );
		}

		CallbackFct		m_pCallback;
		T*				m_pObject;
	};
}
