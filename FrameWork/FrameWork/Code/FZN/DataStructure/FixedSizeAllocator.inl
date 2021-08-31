/*
FixedSizeAllocator class
Copyright 2001 Justin Heyes-Jones

This class is a constant time O(1) memory manager for objects of
a specified type. The type is specified using a template class.

Memory is allocated from a fixed size buffer which you can specify in the
class constructor or use the default.

Using GetFirst and GetNext it is possible to iterate through the elements
one by one, and this would be the most common use for the class.

I would suggest using this class when you want O(1) add and delete
and you don't do much searching, which would be O(n). Structures such as binary
trees can be used instead to get O(logn) access time.
*/

#include "FZN/DataStructure/FixedSizeAllocator.h"


namespace fzn
{
	//=========================================================
	//==========================PUBLIC==========================
	//=========================================================

	/////////////////CONSTRUCTOR / DESTRUCTOR/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Constructor
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class USER_TYPE>
	FixedSizeAllocator<USER_TYPE>::FixedSizeAllocator( UINT _maxElements /*= FSA_DEFAULT_SIZE*/ ) : m_MaxElements( _maxElements ), m_pFirstUsed( nullptr )
	{
		// Allocate enough memory for the maximum number of elements
		/*char* pMem = new char[m_MaxElements * sizeof(FSA_ELEMENT)];

		m_pMemory = (FSA_ELEMENT*)pMem;*/

		m_pMemory = new FSA_ELEMENT[m_MaxElements];

		// Set the free list first pointer
		m_pFirstFree = m_pMemory;

		// Clear the memory
		memset( m_pMemory, 0, sizeof( FSA_ELEMENT ) * m_MaxElements );

		// Point at first element
		FSA_ELEMENT* pElement = m_pFirstFree;

		// Set the double linked free list
		for( UINT i = 0; i < m_MaxElements; ++i )
		{
			pElement->pPrev = pElement - 1;
			pElement->pNext = pElement + 1;
			pElement++;
		}

		// first element should have a null prev
		m_pFirstFree->pPrev = nullptr;
		// last element should have a null next
		( pElement - 1 )->pNext = nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Destructor
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class USER_TYPE>
	FixedSizeAllocator<USER_TYPE>::~FixedSizeAllocator()
	{
		// Free up the memory
		delete[] m_pMemory;
	}


	/////////////////OTHER FUNCTIONS/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Allocate a new USER_TYPE and return a pointer to it
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class USER_TYPE>
	USER_TYPE* FixedSizeAllocator<USER_TYPE>::Alloc()
	{
		FSA_ELEMENT* pNewNode = nullptr;

		if( !m_pFirstFree )
			return nullptr;
		else
		{
			pNewNode = m_pFirstFree;
			m_pFirstFree = pNewNode->pNext;

			// if the new node points to another free node then
			// change that nodes prev free pointer...
			if( pNewNode->pNext )
				pNewNode->pNext->pPrev = nullptr;

			// node is now on the used list
			pNewNode->pPrev = nullptr; // the allocated node is always first in the list

			if( m_pFirstUsed == nullptr )
				pNewNode->pNext = nullptr; // no other nodes
			else
			{
				m_pFirstUsed->pPrev = pNewNode; // insert this at the head of the used list
				pNewNode->pNext = m_pFirstUsed;
			}

			m_pFirstUsed = pNewNode;
		}

		return reinterpret_cast<USER_TYPE*>( pNewNode );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Free the given user type
	// For efficiency I don't check whether the user_data is a valid pointer that was allocated. I may add some debug only checking
	// (To add the debug check you'd need to make sure the pointer is in 
	// the m_pMemory area and is pointing at the start of a node)
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class USER_TYPE>
	void FixedSizeAllocator<USER_TYPE>::Free( USER_TYPE* _userData )
	{
		if( _userData == nullptr )
			return;

		FSA_ELEMENT *pNode = reinterpret_cast<FSA_ELEMENT*>( _userData );

		// manage used list, remove this node from it
		if( pNode->pPrev )
			pNode->pPrev->pNext = pNode->pNext;
		else
		{
			// this handles the case that we delete the first node in the used list
			m_pFirstUsed = pNode->pNext;
		}

		if( pNode->pNext )
			pNode->pNext->pPrev = pNode->pPrev;

		// add to free list
		if( m_pFirstFree == nullptr )
		{
			// free list was empty
			m_pFirstFree = pNode;
			pNode->pPrev = nullptr;
			pNode->pNext = nullptr;
		}
		else
		{
			// Add this node at the start of the free list
			m_pFirstFree->pPrev = pNode;
			pNode->pNext = m_pFirstFree;
			m_pFirstFree = pNode;
		}
	}


	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// For debugging this displays both lists (using the prev/next list pointers)
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class USER_TYPE>
	void FixedSizeAllocator<USER_TYPE>::Debug()
	{
		printf( "free list " );

		FSA_ELEMENT *p = m_pFirstFree;
		while( p )
		{
			printf( "%x!%x ", p->pPrev, p->pNext );
			p = p->pNext;
		}
		printf( "\n" );

		printf( "used list " );

		p = m_pFirstUsed;
		while( p )
		{
			printf( "%x!%x ", p->pPrev, p->pNext );
			p = p->pNext;
		}
		printf( "\n" );
	}


	/////////////////ACCESSORS / MUTATORS/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on the first element
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class USER_TYPE>
	USER_TYPE* FixedSizeAllocator<USER_TYPE>::GetFirst()
	{
		return reinterpret_cast<USER_TYPE*>( m_pFirstUsed );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on the first element
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class USER_TYPE>
	USER_TYPE* FixedSizeAllocator<USER_TYPE>::GetNext( USER_TYPE* _node )
	{
		return reinterpret_cast<USER_TYPE*>( ( reinterpret_cast<FSA_ELEMENT *>( _node ) )->pNext );
	}
} //namespace fzn
