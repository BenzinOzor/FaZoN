//------------------------------------------------------------------------
//Author : Philippe OFFERMANN
//Date : 27.05.16
//Description : Custom map class
//------------------------------------------------------------------------

#ifndef _MAPDEFINITIONS_H_
#define _MAPDEFINITIONS_H_

#include "FZN/DataStructure/Map.h"

namespace fzn
{
	template <class USER_KEY, class USER_TYPE>
	Map<USER_KEY, USER_TYPE>::Map(int _iInitialSize /*=DefaultListSize*/) : m_lElements(_iInitialSize)
	{
	}

	template<class USER_KEY, class USER_TYPE>
	Map<USER_KEY, USER_TYPE>::Map(const Map & _map) : m_lElements(_map.m_lElements)
	{
	}

	template<class USER_KEY, class USER_TYPE>
	Map<USER_KEY, USER_TYPE>& Map<USER_KEY, USER_TYPE>::operator=(const Map & _map)
	{
		m_lElements = _map.m_lElements;
	}

	template <class USER_KEY, class USER_TYPE>
	Map<USER_KEY, USER_TYPE>::~Map()
	{
	}

	template<class USER_KEY, class USER_TYPE>
	USER_TYPE& Map<USER_KEY, USER_TYPE>::operator[](USER_KEY _key)
	{
		Map<USER_KEY, USER_TYPE>::MapElement* elmt = Find(_key);

		if(elmt != nullptr)
			return elmt->data;
		else
		{
			m_lElements.PushBack( MapElement( _key ) );
			return m_lElements.Last()->userElement.data;
		}
	}

} //namespace fzn

#endif //_MAPDEFINITIONS_H_
