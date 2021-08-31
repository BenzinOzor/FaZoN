#pragma once

namespace fzn
{
	class CallbackBase
	{
	public:
		CallbackBase() {}
		virtual void Call() = 0;

		virtual ~CallbackBase() {}
	private:
		CallbackBase& operator=( const CallbackBase& _oCallback );
	};

	template< typename T>
	class MemberCallback : public CallbackBase
	{
	public:
		typedef void ( T::* CallbackType )();
		MemberCallback( CallbackType _pCallback, T* _pObject )
			: CallbackBase()
		{
			m_pCallback = _pCallback;
			m_pObject = _pObject;
		}

		virtual ~MemberCallback()
		{
		}

		virtual void Call()
		{
			( m_pObject->*m_pCallback )();
		}

		CallbackType	m_pCallback;
		T*				m_pObject;
	};

	template< typename T, typename A >
	class Member1ArgCallback : public CallbackBase
	{
	public:
		typedef void ( T::* CallbackType )( A );
		Member1ArgCallback( CallbackType _pCallback, T* _pObject, A _oArg )
			: CallbackBase()
			, m_oArg( _oArg )
		{
			m_pCallback = _pCallback;
			m_pObject = _pObject;
		}

		virtual ~Member1ArgCallback()
		{
		}

		virtual void Call()
		{
			( m_pObject->*m_pCallback )( m_oArg );
		}

		CallbackType	m_pCallback;
		T*				m_pObject;
		A				m_oArg;
	};

	template< typename T, typename A, typename B >
	class Member2ArgCallback : public CallbackBase
	{
	public:
		typedef void ( T::* CallbackType )( A, B );
		Member2ArgCallback( CallbackType _pCallback, T* _pObject, A _oArg, B _oArg2 )
			: CallbackBase()
			, m_oArg( _oArg )
			, m_oArg2( _oArg2 )
		{
			m_pCallback = _pCallback;
			m_pObject = _pObject;
		}

		virtual ~Member2ArgCallback()
		{
		}

		virtual void Call()
		{
			( m_pObject->*m_pCallback )( m_oArg, m_oArg2 );
		}

		CallbackType	m_pCallback;
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
		typedef void( T::* CallbackType )( A );
		Member1DynArgCallback( CallbackType _pCallback, T* _pObject )
			: Member1DynArgCallbackBase<A>()
		{
			m_pCallback = _pCallback;
			m_pObject = _pObject;
		}

		virtual void Call( A oArg )
		{
			return ( m_pObject->*m_pCallback )( oArg );
		}

		virtual ~Member1DynArgCallback() {}

		CallbackType	m_pCallback;
		T*				m_pObject;
	};

	template< typename A, typename B >
	class Member2DynArgCallbackBase
	{
	public:
		Member2DynArgCallbackBase() {}
		virtual void Call( A /*oArgA*/, B /*oArgB*/ ) = 0;

		virtual ~Member2DynArgCallbackBase() {}
	private:
		Member2DynArgCallbackBase& operator=( const Member2DynArgCallbackBase& );
	};

	template< typename T, typename A, typename B >
	class Member2DynArgCallback : public Member2DynArgCallbackBase< A, B >
	{
	public:
		typedef void( T::* CallbackType )( A, B );
		Member2DynArgCallback( CallbackType _pCallback, T* _pObject )
			: Member2DynArgCallbackBase< A, B >()
		{
			m_pCallback = _pCallback;
			m_pObject = _pObject;
		}

		virtual void Call( A oArgA, B oArgB )
		{
			return ( m_pObject->*m_pCallback )( oArgA, oArgB );
		}

		virtual ~Member2DynArgCallback() {}

		CallbackType	m_pCallback;
		T*				m_pObject;
	};
}
