//------------------------------------------------------------------------
/// Author : Philippe OFFERMANN
/// Date : 25.03.14
/// Description : Custom animation class
//------------------------------------------------------------------------

#ifndef _ANIMATION_H_
#define _ANIMATION_H_

#include <string>
#include <basetsd.h>

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Clock.hpp>

#include "FZN/Includes.h"

namespace fzn
{
	class FZN_EXPORT Animation : public sf::Drawable
	{
	public:
		//-------------------------------------------------------------------------------------------------
		/// Possible statuses of an Animation.
		//-------------------------------------------------------------------------------------------------
		enum State
		{
			Playing,
			Paused,
			Stopped
		};


		//-------------------------------------------------------------------------------------------------
		/// Default constructor.
		//-------------------------------------------------------------------------------------------------
		Animation();
		//-------------------------------------------------------------------------------------------------
		/// Constructor.
		/// @param	_name	: Name of the animation to load.
		/// @param	_path	: Path to the anm file.
		//-------------------------------------------------------------------------------------------------
		Animation(const std::string& _name, const std::string& _path);
		//-------------------------------------------------------------------------------------------------
		/// Copy constructor.
		/// @param	_animation	: The animation to copy.
		//-------------------------------------------------------------------------------------------------
		Animation(const Animation& _animation);
		//-------------------------------------------------------------------------------------------------
		/// Function call operator.
		/// @param	_animation	: The animation to copy.
		//-------------------------------------------------------------------------------------------------
		void operator()(const Animation& _animation);
		//-------------------------------------------------------------------------------------------------
		/// Assignment operator.
		/// @param	_animation	: The animation to copy.
		/// @return	A copy of the given Animation.
		//-------------------------------------------------------------------------------------------------
		virtual Animation& operator=(const Animation& _animation);
		//-------------------------------------------------------------------------------------------------
		/// Destructor.
		//-------------------------------------------------------------------------------------------------
		virtual ~Animation();

		//-------------------------------------------------------------------------------------------------
		/// Loads from file.
		/// @param	_name	: Name of the animation to load.
		/// @param	_path	: Path to the anm file.
		//-------------------------------------------------------------------------------------------------

		/// <summary>Loads the animation from an anm file.</summary>
		/// <param name="_name">The name of the animation to load.</param>
		/// <param name="_path">The path to the .anm file.</param>
		void LoadFromFile(const std::string& _name, const std::string& _path);
		//-------------------------------------------------------------------------------------------------
		/// Animation update (index and rect positionning management).
		//-------------------------------------------------------------------------------------------------
		virtual void Update();
		//-------------------------------------------------------------------------------------------------
		/// Animation flip on the x coordinate
		//-------------------------------------------------------------------------------------------------
		void FlipX();
		//-------------------------------------------------------------------------------------------------
		/// Animation flip on the x coordinate.
		/// @param	_iFlip	: Transformation to apply (0 : inversion / 1 : Force positive scale / -1 : Force negative scale).
		//-------------------------------------------------------------------------------------------------
		virtual void FlipX(int _iFlip);
		virtual int GetFlipX() const;

		//-------------------------------------------------------------------------------------------------
		/// Pauses the Animation.
		//-------------------------------------------------------------------------------------------------
		void Pause();
		//-------------------------------------------------------------------------------------------------
		/// Stops the Animation (index is set back to 0).
		//-------------------------------------------------------------------------------------------------
		virtual void Stop();
		//-------------------------------------------------------------------------------------------------
		/// Plays the Animation.
		//-------------------------------------------------------------------------------------------------
		virtual void Play( bool bReset = true );
		//-------------------------------------------------------------------------------------------------
		/// Plays the Animation.
		/// @param	_reset	: If true, plays the animation from the beginning
		//-------------------------------------------------------------------------------------------------
		void Play(INT8 _reset);
		//-------------------------------------------------------------------------------------------------
		/// Indicates whether or not the animation is finished.
		/// @return	If true, the animation is finished.
		//-------------------------------------------------------------------------------------------------
		INT8 Ended();
		//-------------------------------------------------------------------------------------------------
		/// Gets the current state of the animation (Playing, Paused, Stopped).
		/// @return	The animation state.
		//-------------------------------------------------------------------------------------------------
		State GetStatus();
		bool IsPlaying() const;
		bool IsPaused() const;
		bool IsStopped() const;
		//-------------------------------------------------------------------------------------------------
		/// Gets the current sprite of the animation (cuted frame).
		/// @return	The animation sprite.
		//-------------------------------------------------------------------------------------------------
		sf::Sprite GetSprite() const;
		//-------------------------------------------------------------------------------------------------
		/// Sets the position of the animation.
		/// @param [in]	_position	: The wanted position.
		//-------------------------------------------------------------------------------------------------
		void SetPosition( const sf::Vector2f& _position);
		//-------------------------------------------------------------------------------------------------
		/// Gets the position of the animation.
		/// @return	The current position.
		//-------------------------------------------------------------------------------------------------
		virtual sf::Vector2f GetPosition() const;
		//-------------------------------------------------------------------------------------------------
		/// Sets the origin of the animation.
		/// @param [in]	_origin	: The wanted origin.
		//-------------------------------------------------------------------------------------------------
		void SetOrigin(sf::Vector2f& _origin);
		//-------------------------------------------------------------------------------------------------
		/// Gets the origin of the animation.
		/// @return	The current origin.
		//-------------------------------------------------------------------------------------------------
		sf::Vector2f GetOrigin();
		//-------------------------------------------------------------------------------------------------
		/// Sets the current frame index of the animation.
		/// @param	_index	: The wanted index.
		//-------------------------------------------------------------------------------------------------
		void SetCurrentIndex(int _index);
		//-------------------------------------------------------------------------------------------------
		/// Gets the current frame index of the animation.
		/// @return	The current frame index.
		//-------------------------------------------------------------------------------------------------
		int	GetCurrentIndex();
		void SetUseUnmodifiedFrameTime( bool _bUnmodified );
		virtual void SetAlpha( sf::Uint8 _uAlpha );
		virtual void SetColor( sf::Color _oColor, bool _bOverrideAlpha = false );

		void SetVisible( bool _bVisible );
		bool IsVisible() const;

		/////////////////MEMBER VARIABLES/////////////////

		INT8			m_bLoop;					//Loop playing of the animation
		float			m_fSpeed;					//Speed of the animation (1 by default)
		INT8			m_isInManager;			//Indicates if the animation is in the manager or not

	protected:
		virtual void draw( sf::RenderTarget& _oTarget, sf::RenderStates _oStates) const;

		//-------------------------------------------------------------------------------------------------
		/// Setting the positions of all the TextureRect of the animation.
		//-------------------------------------------------------------------------------------------------
		void RectPositioning();
		//-------------------------------------------------------------------------------------------------
		/// Updates the frame index of the animation.
		//-------------------------------------------------------------------------------------------------
		void UpdateIndex();
		//-------------------------------------------------------------------------------------------------
		/// Removes the file name at the end of the path to the anm file to obtain its directory.
		/// @param [in,out]	_file	: The path to the anm file.
		//-------------------------------------------------------------------------------------------------
		void RemoveFileName( std::string& _file );


		/////////////////MEMBER VARIABLES/////////////////

		sf::Texture*	m_texture;
		sf::Sprite		m_sprite;
		sf::Vector2i	m_rect;						//Size of the textureRects of the animation
		float			m_fDeltaTime;				//Delta time between each frame
		sf::Clock		m_clock;					//Internal animation timer
		int				m_iFirstCol;				//Pixel position of the first column of the animation (left of the animation)
		int				m_iFirstRow;				//Pixel position of the first line of the animation (top of the animation)
		int				m_iNbrCol;					//Number of columns in the animation spritesheet
		int				m_iNbrFrames;				//Number of frames in the animation
		int				m_iCurrentIndex;			//Current index in the spritesheet
		INT8			m_bMultipleAnims;			//Multiple animation on the same spritesheet (true) or not
		State			m_eState;					//Current status of the animation
		INT8			m_bReverseReading;			//Reading of the spritesheet (false : left to right)
		sf::Vector2i*	m_positions;				//Container of all the textureRects positions
		float			m_fTimer;
		bool			m_bUseUnmodifiedFrameTime;
		bool			m_bVisible;
	};
}

#endif //_ANIMATION_H_
