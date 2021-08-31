//------------------------------------------------------------------------
//Author : Philippe OFFERMANN
//Date : 09.12.14
//Description : Animation manager (Contains created animations and updates them)
//------------------------------------------------------------------------

#include "FZN/Includes.h"
#include "FZN/Display/Animation.h"
#include "FZN/Managers/AnimManager.h"


FZN_EXPORT fzn::AnimManager* g_pFZN_AnimMgr = nullptr;

namespace fzn
{
	/////////////////CONSTRUCTOR / DESTRUCTOR/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Default constructor
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	AnimManager::AnimManager()
	{
		m_iNbAnims = 0;

		g_pFZN_AnimMgr = this;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Destructor
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	AnimManager::~AnimManager()
	{
		m_animations.clear();
		
		g_pFZN_AnimMgr = nullptr;
	}


	//=========================================================
	//==========================PUBLIC==========================
	//=========================================================

	/////////////////ARRAY MANAGEMENT/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Adds an animation in the manager's array
	//Parameter : Animation to add
	//Return value : The animation has been added (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	INT8 AnimManager::Add( Animation* _anim )
	{
		if( _anim == nullptr )
			return FALSE;

		for( const Animation* pAnim : m_animations )
		{
			if( pAnim == _anim )
				return FALSE;
		}

		m_animations.push_back( _anim );
		m_animations.back()->Stop();
		m_iNbAnims++;
		return TRUE;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Removes an animation from the manager's array
	//Parameter : Animation to remove
	//Return value : The animation has been removed (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	INT8 AnimManager::Remove( Animation* _anim )
	{
		if( _anim == nullptr || !_anim->m_isInManager )
			return FALSE;


		std::vector< Animation* >::iterator it = m_animations.begin();

		while( it != m_animations.end() )
		{
			if( (*it) == _anim )
			{
				m_animations.erase( it );
				--m_iNbAnims;
				return TRUE;
			}

			++it;
		}

		return FALSE;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Updates all the animations in the array
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void AnimManager::Update()
	{
		for( int i = 0 ; i < m_iNbAnims ; ++i )
			m_animations[i]->Update();
	}
} //namespace fzn