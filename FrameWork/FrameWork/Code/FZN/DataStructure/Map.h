//------------------------------------------------------------------------
//Author : Philippe OFFERMANN
//Date : 27.05.16
//Description : Custom map class
//------------------------------------------------------------------------

#ifndef _MAP_H_
#define _MAP_H_

#include "FZN/DataStructure/Vector.h"


namespace fzn
{
	template <class USER_KEY, class USER_TYPE> class Map
	{
	public:
		struct MapElement
		{
			MapElement() { index = 0; }
			MapElement( USER_KEY _key ) : MapElement() { key = _key; }
			~MapElement() {}

			USER_KEY		key;
			USER_TYPE		data;
			int				index;
		};

		Map( int _iInitialSize = DefaultListSize );
		Map( const Map& _map );
		Map& operator=( const Map& _map );
		~Map();

		USER_TYPE& operator[]( USER_KEY _key );

		MapElement* Find( USER_KEY& _key )
		{
			int iNbElem = m_lElements.Size();
			for( int iElement = 0 ; iElement < iNbElem ; ++iElement )
			{
				if( m_lElements[iElement].key == _key )
					return &m_lElements[iElement];
			}

			return nullptr;
		}

		MapElement* Find( USER_KEY&& _key )
		{
			int iNbElem = m_lElements.Size();
			for( int iElement = 0 ; iElement < iNbElem ; ++iElement )
			{
				if( m_lElements[iElement].key == _key )
					return &m_lElements[iElement];
			}

			return nullptr;
		}

	private:
		Vector<MapElement>		m_lElements;
	};
} //namespace fzn

#include "FZN/DataStructure/Map.inl"

#endif //_MAP_H_
