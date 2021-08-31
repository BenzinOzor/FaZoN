//------------------------------------------------------------------------
//Author : Philippe OFFERMANN
//Date : 17.10.14
//Description : Game object management (definitions)
//Portions Copyright (C) Steve Rabin, 2001
//------------------------------------------------------------------------

#include "FZN/Includes.h"
#include "FZN/Game/GameObject/GameObject.h"


namespace fzn
{
	/////////////////CONSTRUCTOR / DESTRUCTOR/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Default constructor
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	GameObject::GameObject()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Default destructor
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	GameObject::~GameObject()
	{
	}


	/////////////////ACCESSORS / MUTATORS/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Mutator on the GameObject sprite
	//Parameter : New sprite
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void GameObject::SetSprite(sf::Sprite& _sprite)
	{
		m_sprite = _sprite;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Mutator on the GameObject sprite
	//Return value : Sprite
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	sf::Sprite* GameObject::GetSprite()
	{
		return &m_sprite;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Mutator on the GameObject animation
	//Parameter : New animation
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void GameObject::SetAnimation(Animation& _anim)
	{
		m_animation = _anim;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Mutator on the GameObject animation
	//Return value : Animation
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	Animation* GameObject::GetAnimation()
	{
		return &m_animation;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//R�cup�ration du CircleShape de l'entit�
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	sf::CircleShape* GameObject::GetHitbox()
	{
		return &m_hitbox;
	}
} //namespace fzn
