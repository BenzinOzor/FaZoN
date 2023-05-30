//------------------------------------------------------------------------
//Author : Philippe OFFERMANN
//Date : 09.12.14
//Description : Animation manager (Contains created animations and updates them)
//------------------------------------------------------------------------

#ifndef _ANIMMANAGER_H_
#define _ANIMMANAGER_H_

#include <vector>

namespace fzn
{
	class Animation;

	class FZN_EXPORT AnimManager : public sf::NonCopyable
	{
	public:
		/////////////////CONSTRUCTOR / DESTRUCTOR/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Default constructor
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		AnimManager();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Destructor
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		~AnimManager();


		/////////////////ARRAY MANAGEMENT/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Adds an animation in the manager's array
		//Parameter : Animation to add
		//Return value : The animation has been added (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		INT8 Add( Animation* _anim );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Removes an animation from the manager's array
		//Parameter : Animation to remove
		//Return value : The animation has been removed (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		INT8 Remove( Animation* _anim );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Updates all the animations in the array
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void Update();

	private:
		/////////////////MEMBER VARIABLES/////////////////

		std::vector< Animation* > m_animations;				//Container of all the animations in use
		int m_iNbAnims;										//Number of animations currently in the list
	};
} //namespace fzn

extern FZN_EXPORT fzn::AnimManager* g_pFZN_AnimMgr;

#endif //_ANIMMANAGER_H_