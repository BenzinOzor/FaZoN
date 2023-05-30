//------------------------------------------------------------------------
/// Author : Philippe OFFERMANN
/// Date : 25.03.14
/// Description : Custom animation class
//------------------------------------------------------------------------

#include "FZN/Includes.h"
#include "FZN/Managers/AnimManager.h"
#include "FZN/Managers/DataManager.h"
#include "FZN/Managers/WindowManager.h"
#include <tinyXML2/tinyxml2.h>
#include "FZN/Display/Animation.h"
#include <iostream>

namespace fzn
{
	//-------------------------------------------------------------------------------------------------
	/// Default constructor.
	//-------------------------------------------------------------------------------------------------
	Animation::Animation()
	{
		m_texture = nullptr;
		m_clock.restart();
		m_iCurrentIndex = 0;
		m_bReverseReading = FALSE;
		m_eState = Playing;
		m_bLoop = TRUE;
		m_isInManager = FALSE;
		m_positions = nullptr;
		m_fSpeed = 1.f;
		m_fTimer = 0.f;
		m_bUseUnmodifiedFrameTime = false;
		m_fDeltaTime = 0.f;
		m_iFirstCol = 0;
		m_iFirstRow = 0;
		m_iNbrCol = 0;
		m_iNbrFrames = 0;
		m_bMultipleAnims = false;
		m_bVisible = true;

		if( g_pFZN_Core->IsModuleCreated( FazonCore::AnimModule ) && g_pFZN_AnimMgr->Add( this ) )
			m_isInManager = TRUE;
	}

	//-------------------------------------------------------------------------------------------------
	/// Constructor.
	/// @param	_name	: Name of the animation to load.
	/// @param	_path	: Path to the anm file.
	//-------------------------------------------------------------------------------------------------
	Animation::Animation( const std::string& _szName, const std::string& _szFile ) : Animation()
	{
		LoadFromFile( _szName, _szFile );
	}

	//-------------------------------------------------------------------------------------------------
	/// Copy constructor.
	/// @param	_animation	: The animation to copy.
	//-------------------------------------------------------------------------------------------------
	Animation::Animation( const Animation& _animation )
	{
		if( _animation.m_positions == nullptr )
			return;

		m_texture = _animation.m_texture;
		m_sprite = _animation.m_sprite;
		m_rect = _animation.m_rect;
		m_fDeltaTime = _animation.m_fDeltaTime;
		m_clock = _animation.m_clock;
		m_iNbrFrames = _animation.m_iNbrFrames;
		m_iCurrentIndex = _animation.m_iCurrentIndex;
		m_eState = _animation.m_eState;
		m_bReverseReading = _animation.m_bReverseReading;
		m_bLoop = _animation.m_bLoop;
		m_iFirstCol = _animation.m_iFirstCol;
		m_iFirstRow = _animation.m_iFirstRow;
		m_iNbrCol = _animation.m_iNbrCol;
		m_iNbrFrames = _animation.m_iNbrFrames;
		m_fTimer = _animation.m_fTimer;
		m_bUseUnmodifiedFrameTime = _animation.m_bUseUnmodifiedFrameTime;
		m_bVisible = _animation.m_bVisible;

		m_sprite.setTexture( *m_texture );


		if( m_positions != nullptr )
			delete[] m_positions;

		m_positions = new sf::Vector2i[m_iNbrFrames];

		memcpy( m_positions, _animation.m_positions, m_iNbrFrames * sizeof( sf::Vector2i ) );
	}

	//-------------------------------------------------------------------------------------------------
	/// Function call operator.
	/// @param	_animation	: The animation to copy.
	//-------------------------------------------------------------------------------------------------
	void Animation::operator()( const Animation& _animation )
	{
		if( _animation.m_positions == nullptr )
			return;

		m_texture = _animation.m_texture;
		m_sprite = _animation.m_sprite;
		m_rect = _animation.m_rect;
		m_fDeltaTime = _animation.m_fDeltaTime;
		m_clock = _animation.m_clock;
		m_iNbrFrames = _animation.m_iNbrFrames;
		m_iCurrentIndex = _animation.m_iCurrentIndex;
		m_eState = _animation.m_eState;
		m_bReverseReading = _animation.m_bReverseReading;
		m_bLoop = _animation.m_bLoop;
		m_iFirstCol = _animation.m_iFirstCol;
		m_iFirstRow = _animation.m_iFirstRow;
		m_iNbrCol = _animation.m_iNbrCol;
		m_iNbrFrames = _animation.m_iNbrFrames;
		m_fTimer = _animation.m_fTimer;
		m_bUseUnmodifiedFrameTime = _animation.m_bUseUnmodifiedFrameTime;
		m_bVisible = _animation.m_bVisible;

		m_sprite.setTexture( *m_texture );

		if( m_positions != nullptr )
			delete[] m_positions;

		m_positions = new sf::Vector2i[m_iNbrFrames];

		memcpy( m_positions, _animation.m_positions, m_iNbrFrames * sizeof( sf::Vector2i ) );
	}

	//-------------------------------------------------------------------------------------------------
	/// Assignment operator.
	/// @param	_animation	: The animation to copy.
	/// @return	A copy of the given Animation.
	//-------------------------------------------------------------------------------------------------
	Animation& Animation::operator=( const Animation& _animation )
	{
		/*if( &_animation == nullptr || _animation.m_positions == nullptr )
			return *this;*/

		m_texture = _animation.m_texture;
		m_sprite = _animation.m_sprite;
		m_rect = _animation.m_rect;
		m_fDeltaTime = _animation.m_fDeltaTime;
		m_clock = _animation.m_clock;
		m_iNbrFrames = _animation.m_iNbrFrames;
		m_iCurrentIndex = _animation.m_iCurrentIndex;
		m_eState = _animation.m_eState;
		m_bReverseReading = _animation.m_bReverseReading;
		m_bLoop = _animation.m_bLoop;
		m_iFirstCol = _animation.m_iFirstCol;
		m_iFirstRow = _animation.m_iFirstRow;
		m_iNbrCol = _animation.m_iNbrCol;
		m_iNbrFrames = _animation.m_iNbrFrames;
		m_fTimer = _animation.m_fTimer;
		m_bUseUnmodifiedFrameTime = _animation.m_bUseUnmodifiedFrameTime;
		m_bVisible = _animation.m_bVisible;

		if( m_texture != nullptr )
			m_sprite.setTexture( *m_texture );

		if( m_positions != nullptr )
			delete[] m_positions;

		if( _animation.m_positions != nullptr && m_iNbrFrames > 0 )
		{
			m_positions = new sf::Vector2i[ m_iNbrFrames ];
			memcpy( m_positions, _animation.m_positions, m_iNbrFrames * sizeof( sf::Vector2i ) );
		}

		return *this;
	}

	//-------------------------------------------------------------------------------------------------
	/// Destructor.
	//-------------------------------------------------------------------------------------------------
	Animation::~Animation()
	{
		if( m_isInManager && g_pFZN_AnimMgr->Remove( this ) )
			m_isInManager = FALSE;

		if( m_positions != nullptr )
			delete[] m_positions;
	}


	//=========================================================
	///=========================PUBLIC==========================
	//=========================================================

	void Animation::LoadFromFile( const std::string& _name, const std::string& _path )
	{
		m_positions = nullptr;
		m_iFirstCol = 0;
		m_iFirstRow = 0;

		std::string texturePath, textureFile, animName;

		animName = _name;

		tinyxml2::XMLDocument infoAnim;

		if( g_pFZN_DataMgr->LoadXMLFile( infoAnim, _path ) )
		{
			FZN_COLOR_LOG( DBG_MSG_COL_RED, "Failure : %s", infoAnim.ErrorName() );
			return;
		}

		tinyxml2::XMLElement* Animation = infoAnim.FirstChildElement( "Animation" );

		UINT8 animFound = FALSE;

		do
		{
			if( Animation == nullptr )
			{
				FZN_COLOR_LOG( DBG_MSG_COL_RED, "Animation creation failure : \"%s\" not found in infoAnim file at \"%s\"", _name.c_str(), _path.c_str() );
				return;
			}

			if( Tools::XMLStringAttribute( Animation, "Name" ) == animName )
				animFound = TRUE;
			else
				Animation = Animation->NextSiblingElement();
		} while( !animFound );

		tinyxml2::XMLElement* Frames = Animation->FirstChildElement( "Frames" );

		texturePath = _path;
		RemoveFileName( texturePath );
		textureFile = Tools::XMLStringAttribute( Animation, "ImagePath" );
		texturePath += textureFile;
		textureFile = textureFile.substr( 0, textureFile.find_last_of( "." ) );

		m_bLoop = Animation->BoolAttribute( "Loop" );
		m_bReverseReading = Animation->BoolAttribute( "ReverseReading" );

		m_fDeltaTime = Frames->FloatAttribute( "Time" );
		m_rect.x = Frames->IntAttribute( "Width" );
		m_rect.y = Frames->IntAttribute( "Height" );
		m_iNbrFrames = Frames->IntAttribute( "Count" );
		m_iNbrCol = Frames->IntAttribute( "Column" );

		m_texture = g_pFZN_DataMgr->LoadTexture( textureFile, texturePath );
		m_sprite.setTexture( *m_texture );

		RectPositioning();

		m_clock.restart();
		m_iCurrentIndex = 0;
		m_eState = Playing;
		m_fSpeed = 1.f;
		m_sprite.setTextureRect( sf::IntRect( m_positions[0].x, m_positions[0].y, m_rect.x, m_rect.y ) );
	}

	//-------------------------------------------------------------------------------------------------
	/// Animation update (index and rect positionning management).
	//-------------------------------------------------------------------------------------------------
	void Animation::Update()
	{
		if( m_fSpeed < 0.f )
			m_fSpeed = 1.f;

		if( m_positions != nullptr )
		{
			m_fTimer += m_bUseUnmodifiedFrameTime ? UnmodifiedFrameTime : FrameTime;
			if( m_eState == State::Playing && m_fTimer >= m_fDeltaTime /** ( 1 / m_fSpeed )*/ )
			{
				UpdateIndex();

				int x = m_positions[m_iCurrentIndex].x;
				int y = m_positions[m_iCurrentIndex].y;
				m_sprite.setTextureRect( sf::IntRect( x, y, m_rect.x, m_rect.y ) );
				m_clock.restart();
				m_fTimer -= m_fDeltaTime;
			}
			else if( m_eState == State::Paused )
				m_fTimer = 0.f;
			else if( m_eState == State::Stopped )
				m_fTimer = 0.f;
		}
	}

	//-------------------------------------------------------------------------------------------------
	/// Animation flip on the x coordinate
	//-------------------------------------------------------------------------------------------------
	void Animation::FlipX()
	{
		if( m_positions != nullptr )
		{
			sf::Vector2f scale( m_sprite.getScale().x, m_sprite.getScale().y );

			m_sprite.setTextureRect( sf::IntRect( sf::Vector2i( 0, 0 ), sf::Vector2i( m_texture->getSize() ) ) );

			if( scale.x == 1.f )
			{
				m_sprite.setScale( -1.f, scale.y );
				m_sprite.setOrigin( m_sprite.getOrigin().x + m_rect.x, m_sprite.getOrigin().y );
			}
			else if( scale.x == -1.f )
			{
				m_sprite.setScale( 1.f, scale.y );
				m_sprite.setOrigin( m_sprite.getOrigin().x - m_rect.x, m_sprite.getOrigin().y );
			}

			RectPositioning();
			int x = m_positions[m_iCurrentIndex].x;
			int y = m_positions[m_iCurrentIndex].y;
			m_sprite.setTextureRect( sf::IntRect( x, y, m_rect.x, m_rect.y ) );
		}
	}

	//-------------------------------------------------------------------------------------------------
	/// Animation flip on the x coordinate.
	/// @param	_iFlip	: Transformation to apply (0 : inversion / 1 : Force positive scale / -1 : Force negative scale).
	//-------------------------------------------------------------------------------------------------
	void Animation::FlipX( int _iFlip )
	{
		if( m_positions != nullptr )
		{
			sf::Vector2f scale( m_sprite.getScale().x, m_sprite.getScale().y );

			m_sprite.setTextureRect( sf::IntRect( sf::Vector2i( 0, 0 ), sf::Vector2i( m_texture->getSize() ) ) );

			if( scale.x == 1.f && ( _iFlip == -1 || _iFlip == 0 ) )
			{
				m_sprite.setScale( -1.f, scale.y );
				m_sprite.setOrigin( m_sprite.getOrigin().x + m_rect.x, m_sprite.getOrigin().y );
			}
			else if( scale.x == -1.f && ( _iFlip == 1 || _iFlip == 0 ) )
			{
				m_sprite.setScale( 1.f, scale.y );
				m_sprite.setOrigin( m_sprite.getOrigin().x - m_rect.x, m_sprite.getOrigin().y );
			}

			RectPositioning();
			int x = m_positions[m_iCurrentIndex].x;
			int y = m_positions[m_iCurrentIndex].y;
			m_sprite.setTextureRect( sf::IntRect( x, y, m_rect.x, m_rect.y ) );
		}
	}


	int Animation::GetFlipX() const
	{
		if( m_sprite.getScale().x < 0.f )
			return -1;

		return 1;
	}

	//-------------------------------------------------------------------------------------------------
	/// Pauses the Animation.
	//-------------------------------------------------------------------------------------------------
	void Animation::Pause()
	{
		m_eState = State::Paused;
	}

	//-------------------------------------------------------------------------------------------------
	/// Stops the Animation (index is set back to 0).
	//-------------------------------------------------------------------------------------------------
	void Animation::Stop()
	{
		m_eState = State::Stopped;
		m_iCurrentIndex = 0;
		m_fTimer = 0.f;
	}

	//-------------------------------------------------------------------------------------------------
	/// Plays the Animation.
	//-------------------------------------------------------------------------------------------------
	void Animation::Play( bool bReset /*= true*/ )
	{
		m_eState = State::Playing;
		m_fTimer = 0.f;
		m_bLoop = true;

		if( bReset )
		{
			m_iCurrentIndex = 0;
			m_clock.restart();
		}
	}

	//-------------------------------------------------------------------------------------------------
	/// Plays the Animation.
	/// @param	_reset	: If true, plays the animation from the beginning
	//-------------------------------------------------------------------------------------------------
	void Animation::Play( INT8 _reset )
	{
		m_eState = State::Playing;
		m_fTimer = 0.f;
		if( _reset )
		{
			m_iCurrentIndex = 0;
			m_sprite.setTextureRect( sf::IntRect( m_positions[0].x, m_positions[0].y, m_rect.x, m_rect.y ) );
			m_clock.restart();
		}
	}

	//-------------------------------------------------------------------------------------------------
	/// Indicates whether or not the animation is finished.
	/// @return	If true, the animation is finished.
	//-------------------------------------------------------------------------------------------------
	INT8 Animation::Ended()
	{
		if( m_iCurrentIndex + 1 == m_iNbrFrames )
			return TRUE;
		else return FALSE;
	}

	//-------------------------------------------------------------------------------------------------
	/// Gets the current state of the animation (Playing, Paused, Stopped).
	/// @return	The animation state.
	//-------------------------------------------------------------------------------------------------
	Animation::State Animation::GetStatus()
	{
		return m_eState;
	}

	bool Animation::IsPlaying() const
	{
		return m_eState == State::Playing;
	}

	bool Animation::IsPaused() const
	{
		return m_eState == State::Paused;
	}

	bool Animation::IsStopped() const
	{
		return m_eState == State::Stopped;
	}

	//-------------------------------------------------------------------------------------------------
	/// Gets the current sprite of the animation (cuted frame).
	/// @return	The animation sprite.
	//-------------------------------------------------------------------------------------------------
	sf::Sprite Animation::GetSprite() const
	{
		return m_sprite;
	}

	//-------------------------------------------------------------------------------------------------
	/// Sets the position of the animation.
	/// @param [in]	_position	: The wanted position.
	//-------------------------------------------------------------------------------------------------
	void Animation::SetPosition( const sf::Vector2f& _pos )
	{
		m_sprite.setPosition( _pos );
	}

	//-------------------------------------------------------------------------------------------------
	/// Gets the position of the animation.
	/// @return	The current position.
	//-------------------------------------------------------------------------------------------------
	sf::Vector2f Animation::GetPosition() const
	{
		return m_sprite.getPosition();
	}

	//-------------------------------------------------------------------------------------------------
	/// Sets the origin of the animation.
	/// @param [in]	_origin	: The wanted origin.
	//-------------------------------------------------------------------------------------------------
	void Animation::SetOrigin( sf::Vector2f& _origin )
	{
		m_sprite.setOrigin( _origin );
	}

	//-------------------------------------------------------------------------------------------------
	/// Gets the origin of the animation.
	/// @return	The current origin.
	//-------------------------------------------------------------------------------------------------
	sf::Vector2f Animation::GetOrigin()
	{
		return m_sprite.getOrigin();
	}

	//-------------------------------------------------------------------------------------------------
	/// Sets the current frame index of the animation.
	/// @param	_index	: The wanted index.
	//-------------------------------------------------------------------------------------------------
	void Animation::SetCurrentIndex( int _index )
	{
		if( _index >= 0 && _index < m_iNbrFrames )
			m_iCurrentIndex = _index;
	}

	//-------------------------------------------------------------------------------------------------
	/// Gets the current frame index of the animation.
	/// @return	The current frame index.
	//-------------------------------------------------------------------------------------------------
	int Animation::GetCurrentIndex()
	{
		return m_iCurrentIndex;
	}

	void Animation::SetUseUnmodifiedFrameTime( bool _bUnmodified )
	{
		m_bUseUnmodifiedFrameTime = _bUnmodified;
	}


	void Animation::SetAlpha( sf::Uint8 _uAlpha )
	{
		sf::Color oColor = m_sprite.getColor();

		oColor.a = _uAlpha;

		m_sprite.setColor( oColor );
	}

	void Animation::SetColor( sf::Color _oColor, bool _bOverrideAlpha /*= false*/ )
	{
		if( _bOverrideAlpha )
		{
			m_sprite.setColor( _oColor );
			return;
		}

		sf::Color oColor = m_sprite.getColor();
		
		oColor.r = _oColor.r;
		oColor.g = _oColor.g;
		oColor.b = _oColor.b;

		m_sprite.setColor( oColor );
	}

	void Animation::SetVisible( bool _bVisible )
	{
		m_bVisible = _bVisible;
	}

	bool Animation::IsVisible() const
	{
		return m_bVisible;
	}

	//=========================================================
	///==========================PRIVATE=========================
	//=========================================================

	void Animation::draw( sf::RenderTarget& _oTarget, sf::RenderStates _oStates ) const
	{
		if( m_bVisible == false )
			return;

		_oTarget.draw( m_sprite, _oStates );
	}

	//-------------------------------------------------------------------------------------------------
	/// Setting the positions of all the TextureRect of the animation.
	//-------------------------------------------------------------------------------------------------
	void Animation::RectPositioning()
	{
		if( m_iNbrFrames <= 0 )
			return;

		int iCurrentRow = 0, iCurrentCol = 0;
		int iPosX = 0, iPosY = 0;

		if( m_positions != nullptr )
			delete[] m_positions;

		m_positions = new sf::Vector2i[m_iNbrFrames];

		if( !m_bReverseReading )
		{
			for( int i = 0; i < m_iNbrFrames; ++i )
			{
				iPosX = m_iFirstCol + iCurrentCol * m_rect.x;
				iPosY = m_iFirstRow + iCurrentRow * m_rect.y;
				m_positions[i] = sf::Vector2i( iPosX, iPosY );
				iCurrentCol++;
				if( iCurrentCol == m_iNbrCol )
				{
					iCurrentCol = 0;
					iCurrentRow++;
				}
			}
		}
		else
		{
			iCurrentCol = m_iNbrCol - 1;
			for( int i = 0; i < m_iNbrFrames; ++i )
			{
				iPosX = ( m_iFirstCol + iCurrentCol ) * m_rect.x;
				iPosY = ( m_iFirstRow + iCurrentRow ) * m_rect.y;
				m_positions[i] = sf::Vector2i( iPosX, iPosY );
				iCurrentCol--;
				if( iCurrentCol < 0 )
				{
					iCurrentCol = m_iNbrCol - 1;
					iCurrentRow++;
				}
			}
		}
	}

	//-------------------------------------------------------------------------------------------------
	/// Updates the frame index of the animation.
	//-------------------------------------------------------------------------------------------------
	void Animation::UpdateIndex()
	{
		if( m_iCurrentIndex == m_iNbrFrames - 1 )
		{
			if( m_bLoop )
				m_iCurrentIndex = 0;
			else m_eState = State::Stopped;
		}
		else if( m_iCurrentIndex < m_iNbrFrames )
			m_iCurrentIndex++;
	}

	//-------------------------------------------------------------------------------------------------
	/// Removes the file name at the end of the path to the anm file to obtain its directory.
	/// @param [in,out]	_file	: The path to the anm file.
	//-------------------------------------------------------------------------------------------------
	void Animation::RemoveFileName( std::string& _str )
	{
		_str = _str.substr( 0, _str.find_last_of( "/" ) + 1 );
	}
}
