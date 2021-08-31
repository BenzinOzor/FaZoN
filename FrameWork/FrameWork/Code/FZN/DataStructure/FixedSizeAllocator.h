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

#ifndef _FSA_H_
#define _FSA_H_


namespace fzn
{
	template <class USER_TYPE> class FixedSizeAllocator
	{
	public:
		/////////////////DATA/////////////////

		enum
		{
			FSA_DEFAULT_SIZE = 100
		};

		// This class enables us to transparently manage the extra data 
		// needed to enable the user class to form part of the double-linked
		// list class
		struct FSA_ELEMENT
		{
			USER_TYPE UserType;

			FSA_ELEMENT* pPrev;
			FSA_ELEMENT* pNext;
		};

		/////////////////CONSTRUCTOR / DESTRUCTOR/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Constructor
		//Parameter : Maximum number of elements
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		FixedSizeAllocator( UINT _maxElements = FSA_DEFAULT_SIZE );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Destructor
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		~FixedSizeAllocator();


		/////////////////OTHER FUNCTIONS/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Allocate a new USER_TYPE and return a pointer to it
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		USER_TYPE* Alloc();

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Free the given user type
		// For efficiency I don't check whether the user_data is a valid pointer that was allocated. I may add some debug only checking
		// (To add the debug check you'd need to make sure the pointer is in 
		// the m_pMemory area and is pointing at the start of a node)
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void Free( USER_TYPE* _userData );

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// For debugging this displays both lists (using the prev/next list pointers)
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void Debug();


		/////////////////ACCESSORS / MUTATORS/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on the first element
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		USER_TYPE* GetFirst();

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on the first element
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		USER_TYPE* GetNext( USER_TYPE* _node );

	private:
		/////////////////MEMBER VARIABLES/////////////////

		FSA_ELEMENT * m_pFirstFree;
		FSA_ELEMENT* m_pFirstUsed;
		UINT m_MaxElements;
		FSA_ELEMENT* m_pMemory;
	};
} //namespace fzn

#include "FZN/DataStructure/FixedSizeAllocator.inl"

#endif //_FSA_H_
