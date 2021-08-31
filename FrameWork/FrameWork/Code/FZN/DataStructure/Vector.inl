//------------------------------------------------------------------------
//Author : Philippe OFFERMANN
//Date : 03.05.15
//Description : Custom list class
//------------------------------------------------------------------------

#include "FZN/DataStructure/Vector.h"


namespace fzn
{
	/////////////////CONSTRUCTOR / DESTRUCTOR/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Default parametered constructor
	//Parameter 1 : Vector initial size (20 by default)
	//Parameter 2 : Allocate (true) or not the elements at the creation of the list (acces by index right after creation for example)
	//NB : Number of elements will be initial size and push backs will add elements after the ones allocated
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class USER_TYPE>
	Vector<USER_TYPE>::Vector( int _initialSize, INT8 _allocate )
	{
		if( _initialSize < 0 )
			m_initialSize = DefaultListSize;
		else 
			m_initialSize = _initialSize;

		m_iCurrentIndex = 0;
		m_iCurrentMaxSize = m_initialSize;
		m_iNbElements = 0;
		m_iReallocSize = DefaultListReallocSize;

		m_list = new Element*[m_initialSize];

		if( !_allocate )
		{
			for( int i = 0 ; i < m_initialSize ; ++i )
				m_list[i] = nullptr;

			m_front = m_list[0];
			m_back = m_list[0];
		}
		else
		{
			for( int i = 0 ; i < m_initialSize ; ++i )
			{
				m_list[i] = new Element;
				m_list[i]->index = i;
				m_iCurrentIndex = i;
				m_iNbElements++;

				if( i > 0 )
				{
					m_list[i]->prev = m_list[i - 1];
					m_list[i]->prev->next = m_list[i];
				}
			}

			m_front = m_list[0];
			m_back = m_list[m_iCurrentIndex];
			m_iCurrentIndex++;
		}
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Copy constructor
	//Parameter : Vector to copy
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class USER_TYPE>
	Vector<USER_TYPE>::Vector( const Vector& _list )
	{
		m_initialSize = _list.m_initialSize;
		m_iCurrentIndex = _list.m_iCurrentIndex;
		m_iCurrentMaxSize = _list.m_iCurrentMaxSize;
		m_iNbElements = _list.m_iNbElements;
		m_iReallocSize = _list.m_iReallocSize;

		m_list = new Element*[m_iCurrentMaxSize];

		for( int i = 0 ; i < m_iCurrentMaxSize ; ++i )
			m_list[i] = nullptr;

		if( m_iCurrentIndex > 0 )
		{
			for( int i = 0 ; i < m_iCurrentIndex ; ++i )
			{
				m_list[i] = new Element;
				m_list[i]->userElement = _list.m_list[i]->userElement;
				m_list[i]->index = _list.m_list[i]->index;

				if( i > 0 )
				{
					m_list[i]->prev = m_list[i - 1];
					m_list[i]->prev->next = m_list[i];
				}
			}

			m_front = m_list[0];
			m_front->prev = nullptr;
			m_back = m_list[m_iCurrentIndex - 1];
			m_back->next = nullptr;
		}
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Functor =
	//Parameter : Vector to copy
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class USER_TYPE>
	Vector<USER_TYPE>& Vector<USER_TYPE>::operator=( const Vector& _list )
	{
		Clear();

		m_initialSize = _list.m_initialSize;
		m_iCurrentIndex = _list.m_iCurrentIndex;
		m_iCurrentMaxSize = _list.m_iCurrentMaxSize;
		m_iNbElements = _list.m_iNbElements;
		m_iReallocSize = _list.m_iReallocSize;


		m_list = new Element*[m_iCurrentMaxSize];

		for( int i = 0 ; i < m_iCurrentMaxSize ; ++i )
			m_list[i] = nullptr;

		if( m_iCurrentIndex > 0 )
		{
			for( int i = 0 ; i < m_iCurrentIndex ; ++i )
			{
				m_list[i] = new Element;
				m_list[i]->userElement = _list.m_list[i]->userElement;
				m_list[i]->index = _list.m_list[i]->index;

				if( i > 0 )
				{
					m_list[i]->prev = m_list[i - 1];
					m_list[i]->prev->next = m_list[i];
				}
			}

			m_front = m_list[0];
			m_front->prev = nullptr;
			m_back = m_list[m_iCurrentIndex - 1];
			m_back->next = nullptr;
		}

		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Default destructor
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class USER_TYPE>
	Vector<USER_TYPE>::~Vector()
	{
		Clear();
	}


	//=========================================================
	//==========================PUBLIC==========================
	//=========================================================

	/////////////////OTHER FUNCTIONS/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Adds an element at the end of the list
	//Parameter : Element to add
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class USER_TYPE>
	int Vector<USER_TYPE>::PushBack( const USER_TYPE& _element )
	{
		int usedIndex = -1;

		if( m_list == nullptr )
		{
			m_list = new Element*[m_initialSize];

			for( int i = 0 ; i < m_initialSize ; ++i )
				m_list[i] = nullptr;

			m_front = m_list[0];
			m_back = m_list[0];
		}

		m_list[m_iCurrentIndex] = new Element;
		m_list[m_iCurrentIndex]->userElement = _element;
		m_list[m_iCurrentIndex]->index = m_iCurrentIndex;

		if( m_iCurrentIndex > 0 )
		{
			m_back->next = m_list[m_iCurrentIndex];
			m_list[m_iCurrentIndex]->prev = m_back;
		}
		else
			m_front = m_list[0];

		m_back = m_list[m_iCurrentIndex];
		usedIndex = m_iCurrentIndex;
		m_iCurrentIndex++;
		m_iNbElements++;

		if( m_iNbElements >= m_iCurrentMaxSize )
			ReallocateList();

		return usedIndex;
	}

	template <class USER_TYPE>
	void fzn::Vector<USER_TYPE>::Insert( int iWhere, const USER_TYPE& _element )
	{
		if( m_list == nullptr )
		{
			m_list = new Element * [ m_initialSize ];

			for( int i = 0; i < m_initialSize; ++i )
				m_list[ i ] = nullptr;

			m_front = m_list[ 0 ];
			m_back = m_list[ 0 ];
		}

		int iElement = m_back->index;

		m_list[ iElement + 1 ] = new Element;
		m_list[ iElement + 1 ]->userElement = m_list[ iElement ]->userElement;
		m_list[ iElement + 1 ]->index = m_list[ iElement ]->index + 1;

		m_back->next = m_list[ m_iCurrentIndex ];
		m_list[ m_iCurrentIndex ]->prev = m_back;
		m_back = m_list[ iElement + 1 ];
		++m_iCurrentIndex;
		++m_iNbElements;

		while( iElement > iWhere )
		{
			m_list[ iElement ]->userElement = m_list[ iElement - 1 ]->userElement;
			--iElement;
		}

		m_list[ iWhere ]->userElement = _element;

		if( m_iNbElements >= m_iCurrentMaxSize )
			ReallocateList();
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Adds an element at the end of the list
	//Parameter : Element to add
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	/*template <class USER_TYPE>
	int Vector<USER_TYPE>::PushBack(USER_TYPE* _element)
	{
		int usedIndex = -1;

		if( m_list == nullptr )
		{
			m_list = new Element*[m_initialSize];

			for( int i = 0 ; i < m_initialSize ; ++i )
				m_list[i] = nullptr;

			m_front = m_list[0];
			m_back = m_list[0];
		}

		m_list[m_iCurrentIndex] = new Element;
		m_list[m_iCurrentIndex]->userElement = _element;
		m_list[m_iCurrentIndex]->index = m_iCurrentIndex;

		if(m_iCurrentIndex > 0)
		{
			m_back->next = m_list[m_iCurrentIndex];
			m_list[m_iCurrentIndex]->prev = m_back;
		}
		else m_front = m_list[0];

		m_back = m_list[m_iCurrentIndex];
		usedIndex = m_iCurrentIndex;
		m_iCurrentIndex++;
		m_iNbElements++;

		if(m_iNbElements >= m_iCurrentMaxSize)
			ReallocateList();

		return usedIndex;
	}*/

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Removes the last element of the list
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class USER_TYPE>
	void Vector<USER_TYPE>::PopBack()
	{
		if( m_iCurrentIndex == 0 )
			return;

		m_iCurrentIndex--;

		delete m_list[m_iCurrentIndex];
		m_list[m_iCurrentIndex] = nullptr;

		if( m_iNbElements > 1 )
		{
			m_back = m_list[m_iCurrentIndex - 1];
			m_back->next = nullptr;
		}
		else
		{
			m_back = m_list[0];
			m_front = m_list[0];
		}

		m_iNbElements--;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Finds the index of a given user element
	//Return value : Index (-1 if not found)
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class USER_TYPE>
	int Vector<USER_TYPE>::FindIndex( USER_TYPE& _userElement )
	{
		for( int i = 0 ; i < m_iNbElements ; ++i )
		{
			if( m_list[i]->userElement == _userElement )
				return i;
		}

		return -1;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Finds the index of a given user element
	//Return value : Index (-1 if not found)
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class USER_TYPE>
	int Vector<USER_TYPE>::FindIndex( USER_TYPE&& _userElement )
	{
		for( int i = 0 ; i < m_iNbElements ; ++i )
		{
			if( m_list[i]->userElement == _userElement )
				return i;
		}

		return -1;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Clears all the container
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class USER_TYPE>
	void Vector<USER_TYPE>::Clear()
	{
		if( m_list != nullptr )
		{
			for( int i = 0 ; i < m_iCurrentMaxSize ; ++i )
			{
				if( m_list[i] != nullptr )
				{
					delete m_list[i];
					m_list[i] = nullptr;
				}
			}

			delete[] m_list;
			m_list = nullptr;
		}

		m_iCurrentIndex = 0;
		m_iNbElements = 0;
		m_iCurrentMaxSize = m_initialSize;
	}


	/////////////////OPERATORS/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Access operator
	//Parameter : Index to reach in the list (returns first element if the index is not correct)
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class USER_TYPE>
	USER_TYPE& Vector<USER_TYPE>::operator[]( int _index )
	{
		if( _index < 0 || _index >= m_iNbElements )
		{
			FZN_COLOR_LOG( DBG_MSG_COL_RED, "Index %d out of range (%d max).", _index, m_iNbElements );
			return m_list[0]->userElement;
		}

		return m_list[_index]->userElement;
	}

	template <class USER_TYPE>
	const USER_TYPE& Vector<USER_TYPE>::operator[]( int _index ) const
	{
		if( _index < 0 || _index >= m_iNbElements )
		{
			FZN_COLOR_LOG( DBG_MSG_COL_RED, "Index %d out of range (%d max).", _index, m_iNbElements );
			return m_list[0]->userElement;
		}

		return m_list[_index]->userElement;
	}


	/////////////////ACCESSOR / MUTATOR/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on the front user element of the list
	//Return value : Front user element
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class USER_TYPE>
	USER_TYPE& Vector<USER_TYPE>::Front()
	{
		return m_front->userElement;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on the back user element of the list
	//Return value : Back user element
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class USER_TYPE>
	USER_TYPE& Vector<USER_TYPE>::Back()
	{
		return m_back->userElement;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on the number of elements in the list
	//Return value : Number of elements
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class USER_TYPE>
	int Vector<USER_TYPE>::Size() const
	{
		return m_iNbElements;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on current maximum size of the list
	//Return value : Maximum size
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class USER_TYPE>
	int Vector<USER_TYPE>::GetMaxSize()
	{
		return m_iCurrentMaxSize;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on the list reallocation size
	//Return value : Reallocation size
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class USER_TYPE>
	int Vector<USER_TYPE>::GetReallocSize()
	{
		return m_iReallocSize;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Mutator on the list reallocation size
	//Return value : New reallocation size
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class USER_TYPE>
	void Vector<USER_TYPE>::SetReallocSize( int _reallocSize )
	{
		if( _reallocSize > 0 )
			m_iReallocSize = _reallocSize;
	}


	//=========================================================
	//==========================PRIVATE=========================
	//=========================================================

	/////////////////OTHER FUNCTIONS/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Reallocates the list so new elements can be added to it
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class USER_TYPE>
	void Vector<USER_TYPE>::ReallocateList()
	{
		int iNewSize = m_iCurrentMaxSize + m_iReallocSize;

		Element** tmpList = new Element*[iNewSize];

		for( int i = 0 ; i < iNewSize ; ++i )
		{
			if( i < m_iNbElements )
			{
				tmpList[i] = m_list[i];
				m_list[i] = nullptr;
			}
			else 
				tmpList[i] = nullptr;
		}

		m_iCurrentMaxSize = iNewSize;
		delete[] m_list;
		m_list = tmpList;
	}
}
