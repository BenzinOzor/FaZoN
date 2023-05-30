#pragma once

#include <vector>


namespace fzn
{
	enum class DataCallbackType
	{
		Update,
		Display,
		Event,
		COUNT,
	};

	class DataCallbackBase
	{
	public:
		virtual void Call() = 0;

		int m_iPriority{ 0 };
	};

	typedef std::vector<DataCallbackBase*> DataCallBacksVector;
	
	template< typename T>
	class DataCallback : public DataCallbackBase
	{
	public:
		typedef void ( T::* CallbackFct )();

		DataCallback( T* _pObject, CallbackFct _pCallback, int _iPriority = 0 ) :
			m_pObject( _pObject ),
			m_pCallback( _pCallback )
		{
			m_iPriority = _iPriority;
		}

		virtual void Call() override
		{
			( m_pObject->*m_pCallback )();
		}

		T*				m_pObject{ nullptr };
		CallbackFct		m_pCallback{ nullptr };
	};

	class DataCallbacksHolder
	{
	public:
		template< typename CallbackType >
		void AddCallback( CallbackType* _pObject, typename DataCallback<CallbackType>::CallbackFct _pFct, DataCallbackType _eCallbackType, int _iPriority = 0 )
		{
			if( _pObject == nullptr || _pFct  == nullptr || _eCallbackType >= DataCallbackType::COUNT )
				return;

			m_daCallbacks[ (int)_eCallbackType ].push_back( new DataCallback<CallbackType>( _pObject, _pFct, _iPriority ) );

			std::sort( m_daCallbacks[ (int)_eCallbackType ].begin(), m_daCallbacks[ (int)_eCallbackType ].end(), DataCallbackSorter );
		}

		template< typename CallbackType >
		void RemoveCallback( CallbackType* _pObject, typename DataCallback<CallbackType>::CallbackFct _pFct, DataCallbackType _eCallbackType )
		{
			if( _pObject == nullptr || _pFct == nullptr || _eCallbackType >= DataCallbackType::COUNT )
				return;

			for( int iCallback = 0; iCallback < m_daCallbacks[ (int)_eCallbackType ].size(); ++iCallback )
			{
				if( DataCallback<CallbackType>* pCallback = dynamic_cast< DataCallback<CallbackType>* >( m_daCallbacks[ (int)_eCallbackType ][ iCallback ] ) )
				{
					if( pCallback->m_pObject == _pObject && pCallback->m_pCallback == _pFct )
					{
						delete m_daCallbacks[ (int)_eCallbackType ][ iCallback ];
						m_daCallbacks[ (int)_eCallbackType ].erase( m_daCallbacks[ (int)_eCallbackType ].begin() + iCallback );
						return;
					}
				}
			}
		}

		void ExecuteCallbacks( DataCallbackType _eCallbackType )
		{
			for( DataCallbackBase* pCallback : m_daCallbacks[ (int)_eCallbackType ] )
				pCallback->Call();
		}

		static bool DataCallbackSorter( const DataCallbackBase* _pCallbackA, const DataCallbackBase* _pCallbackB )
		{
			if( _pCallbackA == nullptr )
				return false;

			if( _pCallbackB == nullptr )
				return true;

			return _pCallbackA->m_iPriority < _pCallbackB->m_iPriority;
		}

	protected:
		DataCallBacksVector m_daCallbacks[ (int)DataCallbackType::COUNT ];
	};

} //fzn