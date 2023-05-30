//------------------------------------------------------------------------
/// Author : Philippe OFFERMANN
/// Date : 25.09.18
/// Description : The Binding Of Isaac Rebirth animation class
//------------------------------------------------------------------------

#include "FZN/Includes.h"
#include "FZN/Managers/AnimManager.h"
#include "FZN/Managers/AudioManager.h"
#include "FZN/Managers/DataManager.h"
#include "FZN/Managers/WindowManager.h"
#include <tinyXML2/tinyxml2.h>
#include "FZN/Display/Anm2.h"

#define AnimFrameTime ( 1.f / m_iFPS )

namespace fzn
{
	const std::string Anm2::ANIMATION_START = "AnimationStart";
	const std::string Anm2::ANIMATION_END	= "AnimationEnd";


	sf::Color Anm2::LayerInfo::GetCurrentColorOverlay( float _fSpeedRatio ) const
	{
		const FrameInfo& oCurrentFrameInfo = m_oFrames[ m_iFrameIndex ];

		if( oCurrentFrameInfo.m_bInterpolate && m_iFrameIndex + 1 < (int)m_oFrames.size() )
		{
			const float fDuration = oCurrentFrameInfo.m_fDuration * _fSpeedRatio;

			const FrameInfo& oNextFrame = m_oFrames[ m_iFrameIndex + 1 ];

			sf::Vector2f vNewPosition = fzn::Math::Interpolate( 0.f, fDuration, oCurrentFrameInfo.m_vPosition, oNextFrame.m_vPosition, m_fTimer );
			sf::Vector2f vNewOrigin = fzn::Math::Interpolate( 0.f, fDuration, oCurrentFrameInfo.m_vOrigin, oNextFrame.m_vOrigin, m_fTimer );
			sf::Vector2f vNewScale = fzn::Math::Interpolate( 0.f, fDuration, oCurrentFrameInfo.m_vScale, oNextFrame.m_vScale, m_fTimer );
			float fNewRotation = fzn::Math::Interpolate( 0.f, fDuration, oCurrentFrameInfo.m_fRotation, oNextFrame.m_fRotation, m_fTimer );

			sf::Uint8 uNewRed = ( sf::Uint8 )fzn::Math::Interpolate( 0.f, fDuration, (float)oCurrentFrameInfo.m_oColorOverlay.r, (float)oNextFrame.m_oColorOverlay.r, m_fTimer );
			sf::Uint8 uNewGreen = ( sf::Uint8 )fzn::Math::Interpolate( 0.f, fDuration, (float)oCurrentFrameInfo.m_oColorOverlay.g, (float)oNextFrame.m_oColorOverlay.g, m_fTimer );
			sf::Uint8 uNewBlue = ( sf::Uint8 )fzn::Math::Interpolate( 0.f, fDuration, (float)oCurrentFrameInfo.m_oColorOverlay.b, (float)oNextFrame.m_oColorOverlay.b, m_fTimer );
			sf::Uint8 uNewAlpha = ( sf::Uint8 )fzn::Math::Interpolate( 0.f, fDuration, (float)oCurrentFrameInfo.m_oColorOverlay.a, (float)oNextFrame.m_oColorOverlay.a, m_fTimer );

			return sf::Color( uNewRed, uNewGreen, uNewBlue, uNewAlpha );
		}
		
		return oCurrentFrameInfo.m_oColorOverlay;
	}


	Anm2::Anm2()
		: Animation()
		, m_bIsProcessingTriggers( false )
		, m_bReplaceTriggers( false )
		, m_fDuration( 0.f )
		, m_fSpeedRatio( 1.f )
		, m_iFPS ( 0 )
		, m_fPreviousTimer( 0.f )
		, m_sName( "" )
		, m_bResetAtEnd( false )
		, m_fRotation( 0.f )
		, m_vFlippedScale( { 1.f, 1.f } )
		, m_fScaleRatio( 1.f )
	{
		m_iCurrentIndex = -1;

		m_oAnimationStartTrigger.m_sName	= ANIMATION_START;
		m_oAnimationEndTrigger.m_sName		= ANIMATION_END;

		m_oTriggersBuffer.clear();
		m_oAnimationStartTriggerBuffer.Reset();
		m_oAnimationEndTriggerBuffer.Reset();
	}

	Anm2::Anm2( const Anm2& _oAnm )
		: m_bIsProcessingTriggers( false )
		, m_bReplaceTriggers( false )
	{
		*this = _oAnm;

		m_iCurrentIndex = -1;
	}

	Anm2::Anm2( tinyxml2::XMLElement* _pContent, tinyxml2::XMLElement* _pAnimation, const std::string& _sPath )
		: Anm2()
	{
		LoadFromXMLNode( _pContent, _pAnimation, _sPath );
	}

	Anm2::~Anm2()
	{
	}

	bool Anm2::IsValid() const
	{
		if( m_oBaseTextures.empty() || m_oLayers.empty() && m_oSockets.empty() )
			return false;

		return true;
	}

	Anm2& Anm2::operator=( const Anm2& _animation )
	{
		Animation::operator=( _animation );

		m_oRootAnimation			= _animation.m_oRootAnimation;
		m_oLayers					= _animation.m_oLayers;
		m_oSockets					= _animation.m_oSockets;

		_ReplaceTriggersOnAnimationChange( &_animation );
		
		m_oBaseTextures				= _animation.m_oBaseTextures;
		
		m_fDuration					= _animation.m_fDuration;
		m_fSpeedRatio				= _animation.m_fSpeedRatio;
		m_iFPS						= _animation.m_iFPS;
		m_fTimer					= _animation.m_fTimer;
		m_fPreviousTimer			= _animation.m_fPreviousTimer;

		m_sName						= _animation.m_sName;
		m_vPosition					= _animation.m_vPosition;
		m_fRotation					= _animation.m_fRotation;

		m_bResetAtEnd				= _animation.m_bResetAtEnd;

		m_vFlippedScale				= _animation.m_vFlippedScale;
		m_fScaleRatio				= _animation.m_fScaleRatio;

		return *this;
	}

	bool Anm2::ChangeAnimation( const std::string& _sAnimatedObject, const std::string& _sAnimation, const ChangeAnimationSettingsMask& _uSettings )
	{
		return ChangeAnimation( g_pFZN_DataMgr->GetAnm2( _sAnimatedObject, _sAnimation ), _uSettings );
	}

	bool Anm2::ChangeAnimation( const Anm2* _pAnimation, const ChangeAnimationSettingsMask& _uSettings /*= 0*/ )
	{
		if( _pAnimation == nullptr )
			return false;

		const float fTimer = m_fTimer;
		const State eState = m_eState;

		Animation::operator=( *_pAnimation );

		m_oRootAnimation			= _pAnimation->m_oRootAnimation;

		_ReplaceTriggersOnAnimationChange( _pAnimation );

		if( Tools::MaskHasFlagRaised( _uSettings, (sf::Uint8)ChangeAnimationSettings::eKeepTextures ) == false )
			m_oBaseTextures = _pAnimation->m_oBaseTextures;

		m_iFPS			= _pAnimation->m_iFPS;
		m_sName			= _pAnimation->m_sName;
		m_bResetAtEnd	= _pAnimation->m_bResetAtEnd;
		m_vFlippedScale = _pAnimation->m_vFlippedScale;

		if( Tools::MaskHasFlagRaised( _uSettings, (sf::Uint8)ChangeAnimationSettings::eOverrideScaleRatio ) )
			m_fScaleRatio = _pAnimation->m_fScaleRatio;

		// This implies that the current animation and the new one have equivalent layers. (ex: The left and right rotation of the same animation)
		if( Tools::MaskHasFlagRaised( _uSettings, (sf::Uint8)ChangeAnimationSettings::eKeepLayersTimers ) )
		{
			if( m_oLayers.size() != _pAnimation->m_oLayers.size() || m_oSockets.size() != _pAnimation->m_oSockets.size() )
			{
				m_oLayers = _pAnimation->m_oLayers;
				m_oSockets = _pAnimation->m_oSockets;
				m_fDuration = _pAnimation->m_fDuration;
				m_fSpeedRatio = _pAnimation->m_fSpeedRatio;
				m_fPreviousTimer = _pAnimation->m_fPreviousTimer;
			}
			else
			{
				m_fTimer = fTimer;
				m_eState = eState;

				for( int iLayer = 0; iLayer < (int)m_oLayers.size(); ++iLayer )
					_ChangeAnimationLayerSetup( m_oLayers[ iLayer ], _pAnimation->m_oLayers[ iLayer ] );

				for( int iSocket = 0; iSocket < (int)m_oSockets.size(); ++iSocket )
					_ChangeAnimationLayerSetup( m_oSockets[ iSocket ], _pAnimation->m_oSockets[ iSocket ] );
			}
		}
		else
		{
			m_oLayers = _pAnimation->m_oLayers;
			m_oSockets = _pAnimation->m_oSockets;
			m_fDuration = _pAnimation->m_fDuration;
			m_fSpeedRatio = _pAnimation->m_fSpeedRatio;

			if( Tools::MaskHasFlagRaised( _uSettings, (sf::Uint8)ChangeAnimationSettings::eKeepTextures ) )
			{
				for( LayerInfo& oLayer : m_oLayers )
				{
					if( oLayer.m_iBaseTextureId < (int)m_oBaseTextures.size() )
						oLayer.m_oSprite.setTexture( *m_oBaseTextures[ oLayer.m_iBaseTextureId ].second );
				}
			}
		}

		if( Tools::MaskHasFlagRaised( _uSettings, (sf::Uint8)ChangeAnimationSettings::eKeepPosition ) )
			SetPosition( m_vPosition );
		else
			m_vPosition = _pAnimation->m_vPosition;

		if( Tools::MaskHasFlagRaised( _uSettings, (sf::Uint8)ChangeAnimationSettings::eKeepRotation ) )
			SetRotation( m_fRotation );
		else
			m_fRotation = _pAnimation->m_fRotation;

		return true;
	}

	void Anm2::LoadFromXMLNode( tinyxml2::XMLElement* _pContent, tinyxml2::XMLElement* _pAnimation, const std::string& _sPath )
	{
		if( _pContent == nullptr || _pAnimation == nullptr )
			return;

		//FZN_LOG( true, DBG_MSG_COL_PURPLE, "=======================================================" );
		//FZN_LOG( true, DBG_MSG_COL_PURPLE, "Retrieveing informations on animation \"%s\" from XML", _pAnimation->Attribute( "Name" ) );
		
		m_sName = Tools::XMLStringAttribute( _pAnimation, "Name" );

		tinyxml2::XMLNode* pParent = _pAnimation->Parent()->Parent();
		tinyxml2::XMLElement* pInfo = pParent->FirstChildElement( "Info" );

		if( pInfo != nullptr )
			m_iFPS = pInfo->IntAttribute( "Fps" );

		m_fDuration = _pAnimation->IntAttribute( "FrameNum" ) * AnimFrameTime;
		m_bLoop = _pAnimation->BoolAttribute( "Loop" );

		_GetRootAnimationInformations( _pAnimation );
		_GetLayersInformations( _pContent, _pAnimation, _sPath );
		_GetSocketsInformations( _pContent, _pAnimation );
		_GetTriggersInformations( _pContent, _pAnimation );

		_Init();

		//FZN_LOG( true, DBG_MSG_COL_PURPLE, "=======================================================" );
	}

	void Anm2::Stop()
	{
		m_eState = Animation::Stopped;

		for( LayerInfo& oLayer : m_oLayers )
		{
			oLayer.m_fTimer			= 0.f;
			oLayer.m_iFrameIndex	= 0;
		}

		for( LayerInfo& oSocket : m_oSockets )
		{
			oSocket.m_fTimer		= 0.f;
			oSocket.m_iFrameIndex	= 0;
		}

		m_fTimer = 0.f;
		m_fPreviousTimer = 0.f;

		_Init();
	}

	void Anm2::Play( bool bReset /*= true*/ )
	{
		m_eState = Animation::Playing;
		
		for( LayerInfo& oLayer : m_oLayers )
		{
			oLayer.m_fTimer = 0.f;

			if( bReset )
				oLayer.m_iFrameIndex = 0;
		}

		for( LayerInfo& oSocket : m_oSockets )
		{
			oSocket.m_fTimer = 0.f;

			if( bReset )
				oSocket.m_iFrameIndex = 0;
		}

		if( bReset )
		{
			m_fTimer = 0.f;
			m_fPreviousTimer = 0.f;
		}
	}

	void Anm2::PlayThenPause( bool bReset /*= true */ )
	{
		Play( bReset );
		m_bLoop = false;
		m_bResetAtEnd = false;
	}

	void Anm2::PlayThenStop( bool bReset /*= true*/ )
	{
		Play( bReset );
		m_bLoop = false;
		m_bResetAtEnd = true;
	}

	void Anm2::Update()
	{
		if( m_oLayers.empty() && m_oSockets.empty() )
			return;

		m_fPreviousTimer = m_fTimer;

		if( m_eState == Animation::Playing )
		{
			bool bRestart = false;

			m_fTimer += m_bUseUnmodifiedFrameTime ? UnmodifiedFrameTime : FrameTime;

			if( m_fTimer >= m_fDuration * m_fSpeedRatio )
			{
				if( m_bLoop )
				{
					bRestart			= true;
					m_fTimer			-= m_fDuration * m_fSpeedRatio;
					m_fPreviousTimer	= m_fTimer;
				}
				else
				{
					bRestart = m_bResetAtEnd;
					m_eState = m_bResetAtEnd ? Animation::Stopped : Animation::Paused;
				}

				_ProcessTriggerContents( m_oAnimationEndTrigger );
				_ProcessTriggersBuffers();
			}
			
			_UpdateLayerInfoVector( m_oLayers, bRestart );
			_UpdateLayerInfoVector( m_oSockets, bRestart );
			_UpdateTriggers( m_fPreviousTimer == 0.f || ( bRestart && m_bLoop ) );
		}
	}

	void Anm2::SetPosition( sf::Vector2f _vPosition )
	{
		m_vPosition = _vPosition;

		const sf::Vector2f vOffsetedPosition = m_oRootAnimation.m_vPosition + _vPosition;

		for( LayerInfo& oLayer : m_oLayers )
			oLayer.m_oSprite.setPosition( vOffsetedPosition + oLayer.m_oFrames[ oLayer.m_iFrameIndex ].m_vPosition );

		for( LayerInfo& oSocket : m_oSockets )
			oSocket.m_oSprite.setPosition( vOffsetedPosition + oSocket.m_oFrames[ oSocket.m_iFrameIndex ].m_vPosition );
	}

	void Anm2::Rotate( float _fAngleOffset )
	{
		SetRotation( m_fRotation + _fAngleOffset );
	}

	void Anm2::SetRotation( float _fRotation )
	{
		if( m_oLayers.empty() && m_oSockets.empty() )
			return;

		m_fRotation = _fRotation;

		for( LayerInfo& oLayer : m_oLayers )
			oLayer.m_oSprite.setRotation( m_fRotation + oLayer.m_oFrames[ oLayer.m_iFrameIndex ].m_fRotation );

		for( LayerInfo& oSocket : m_oSockets )
			oSocket.m_oSprite.setRotation( m_fRotation + oSocket.m_oFrames[ oSocket.m_iFrameIndex ].m_fRotation );
	}

	float Anm2::GetRotation() const
	{
		return m_fRotation;
	}

	void Anm2::SetFrame( int _iFrameIndex, const std::string& _sLayer, bool _bAdaptAnimationToLayer /*= false*/ )
	{
		if( _sLayer.empty() )
			return;

		float fAnimationTimer = -1.f;

		for( LayerInfo& oLayer : m_oLayers )
		{
			if( oLayer.m_sName == _sLayer && _iFrameIndex < (int)oLayer.m_oFrames.size() )
			{
				const FrameInfo& oCurrentFrameInfo = oLayer.m_oFrames[ oLayer.m_iFrameIndex ];
				oLayer.m_oSprite.setPosition( oLayer.m_oSprite.getPosition() - oCurrentFrameInfo.m_vPosition );

				oLayer.m_iFrameIndex = _iFrameIndex;
				fAnimationTimer = _GetLayerDurationToIndex( oLayer, _iFrameIndex );

				_UpdateLayerInfos( oLayer );
				break;
			}
		}

		if( _bAdaptAnimationToLayer == false || fAnimationTimer < 0.f )
			return;

		auto GetLayerFrameIndexForTimer = []( const LayerInfo& _rLayer, float _fTimer ) -> int
		{
			float fPrevDuration = 0.f;
			float fDuration = 0.f;

			for( int iFrame = 0; iFrame < _rLayer.m_oFrames.size(); ++iFrame )
			{
				fDuration += _rLayer.m_oFrames[ iFrame ].m_fDuration;

				if( fPrevDuration <= _fTimer && _fTimer < fDuration )
					return iFrame;

				fPrevDuration = fDuration;
			}

			return 0;
		};

		auto AdaptLayerVectorToNewTimer = [&]( LayerInfoVector& _rLayers, float _fTimer )
		{
			for( LayerInfo& oLayer : _rLayers )
			{
				const FrameInfo& oCurrentFrameInfo = oLayer.m_oFrames[ oLayer.m_iFrameIndex ];
				oLayer.m_oSprite.setPosition( oLayer.m_oSprite.getPosition() - oCurrentFrameInfo.m_vPosition );

				oLayer.m_iFrameIndex = GetLayerFrameIndexForTimer( oLayer, fAnimationTimer );

				_UpdateLayerInfos( oLayer );
			}
		};

		AdaptLayerVectorToNewTimer( m_oLayers, fAnimationTimer );
		AdaptLayerVectorToNewTimer( m_oSockets, fAnimationTimer );
	}

	void Anm2::SetLayerVisible( const std::string& _sLayer, bool _bVisible )
	{
		for( LayerInfo& oLayer : m_oLayers )
		{
			if( oLayer.m_sName == _sLayer )
				oLayer.m_bVisibilityOverride = _bVisible;
		}
	}

	int Anm2::GetCurrentFrame( const std::string& _sLayer /*= "" */ )
	{
		for( LayerInfo& oLayer : m_oLayers )
		{
			if( oLayer.m_sName == _sLayer )
				return oLayer.m_iFrameIndex;
		}

		for( LayerInfo& oSocket : m_oSockets )
		{
			if( oSocket.m_sName == _sLayer )
				return oSocket.m_iFrameIndex;
		}

		return 0;
	}

	void Anm2::SetScale( const sf::Vector2f& _vScale )
	{
		if( m_oLayers.empty() && m_oSockets.empty() )
			return;

		for( LayerInfo& oLayer : m_oLayers )
		{
			oLayer.m_oSprite.setScale( _vScale );
		}

		for( LayerInfo& oSocket : m_oSockets )
		{
			oSocket.m_oSprite.setScale( _vScale );
		}
	}

	void Anm2::SetScaleRatio( float _fScaleRatio )
	{
		if( m_oLayers.empty() && m_oSockets.empty() )
			return;

		m_fScaleRatio = _fScaleRatio;

		for( LayerInfo& oLayer : m_oLayers )
		{
			oLayer.m_oSprite.setScale( oLayer.m_oSprite.getScale() * m_fScaleRatio );
		}

		for( LayerInfo& oSocket : m_oSockets )
		{
			oSocket.m_oSprite.setScale( oSocket.m_oSprite.getScale() * m_fScaleRatio );
		}
	}

	int Anm2::GetFrameCount( const std::string& _sLayer ) const
	{
		for( const LayerInfo& oLayer : m_oLayers )
		{
			if( oLayer.m_sName == _sLayer  )
				return oLayer.m_oFrames.size();
		}

		return -1;
	}

	sf::Vector2f Anm2::GetPosition( const std::string& _sLayerOrSocket ) const
	{
		for( const LayerInfo& oLayer : m_oLayers )
		{
			if( oLayer.m_sName == _sLayerOrSocket  )
				return oLayer.m_oSprite.getPosition();
		}

		for( const LayerInfo& oSocket : m_oSockets )
		{
			if( oSocket.m_sName == _sLayerOrSocket  )
				return oSocket.m_oSprite.getPosition();
		}

		return { 0.f, 0.f };
	}

	sf::Vector2f Anm2::GetPosition() const
	{
		return m_vPosition;
	}

	void Anm2::SetAlpha( sf::Uint8 _uAlpha )
	{
		if( m_oLayers.empty() && m_oSockets.empty() )
			return;

		sf::Color oColor;

		for( LayerInfo& oLayer : m_oLayers )
		{
			oColor = oLayer.m_oSprite.getColor();
			oColor.a = _uAlpha;

			oLayer.m_oSprite.setColor( oColor );
		}

		for( LayerInfo& oSocket : m_oSockets )
		{
			oColor = oSocket.m_oSprite.getColor();
			oColor.a = _uAlpha;

			oSocket.m_oSprite.setColor( oColor );
		}
	}

	void Anm2::SetColor( sf::Color _oColor, bool _bOverrideAlpha /*= false */ )
	{
		if( m_oLayers.empty() && m_oSockets.empty() )
			return;

		sf::Color oColor;

		for( LayerInfo& oLayer : m_oLayers )
		{
			if( _bOverrideAlpha )
			{
				oLayer.m_oSprite.setColor( _oColor );
				continue;
			}

			oColor = oLayer.m_oSprite.getColor();
			oColor.r = _oColor.r;
			oColor.g = _oColor.g;
			oColor.b = _oColor.b;

			oLayer.m_oSprite.setColor( oColor );
		}

		for( LayerInfo& oSocket : m_oSockets )
		{
			if( _bOverrideAlpha )
			{
				oSocket.m_oSprite.setColor( _oColor );
				continue;
			}

			oColor = oSocket.m_oSprite.getColor();
			oColor.r = _oColor.r;
			oColor.g = _oColor.g;
			oColor.b = _oColor.b;

			oSocket.m_oSprite.setColor( oColor );
		}
	}

	void Anm2::SetLayerColor( const std::string& _sLayer, const sf::Color& _oColor, bool _bOverrideAlpha /*= false */ )
	{
		if( m_oLayers.empty() )
			return;

		sf::Color oColor;

		for( LayerInfo& oLayer : m_oLayers )
		{
			if( oLayer.m_sName != _sLayer )
				continue;

			if( _bOverrideAlpha )
			{
				oLayer.m_oSprite.setColor( _oColor );
				continue;
			}

			oColor = oLayer.m_oSprite.getColor();
			oColor.r = _oColor.r;
			oColor.g = _oColor.g;
			oColor.b = _oColor.b;

			oLayer.m_oSprite.setColor( oColor );
		}
	}

	void Anm2::ReplaceSpritesheet( int _iSpritesheetId, const std::string& _sSpritesheet, const std::string& _sSpritesheetPath /*= ""*/, bool _bHandleError /*= true*/ )
	{
		BaseTextures::iterator itTexture = m_oBaseTextures.find( _iSpritesheetId );

		if( itTexture == m_oBaseTextures.end() )
		{
			FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "Spritesheet id %d not found in animation \"%s\".", _iSpritesheetId, m_sName.c_str() );
			return;
		}

		sf::Texture* pTexture = nullptr;
		
		if( _sSpritesheetPath.empty() )
			pTexture = g_pFZN_DataMgr->GetTexture( _sSpritesheet, _bHandleError );
		else
			pTexture = g_pFZN_DataMgr->LoadTexture( _sSpritesheet, _sSpritesheetPath );

		if( pTexture == nullptr )
			return;

		//bool bSmooth = itTexture->second->isSmooth();

		itTexture->second.second = pTexture;
		//itTexture->second->setSmooth( bSmooth );

		for( LayerInfo& oLayer : m_oLayers )
		{
			if( oLayer.m_iBaseTextureId == _iSpritesheetId )
			{
				oLayer.m_oSprite.setTexture( *itTexture->second.second );
			}
		}
	}

	void Anm2::SetAnimationDuration( float _fDuration )
	{
		if( _fDuration <= 0.f || m_fDuration <= 0.f )
			return;

		SetAnimationSpeedRatio( _fDuration / m_fDuration );
	}

	void Anm2::SetAnimationSpeedRatio( float _fSpeedRatio )
	{
		if( _fSpeedRatio <= 0.f )
			return;

		m_fSpeedRatio = _fSpeedRatio;
	}

	float Anm2::GetAnimationDuration() const
	{
		return m_fDuration * m_fSpeedRatio;
	}

	bool Anm2::HasEnded() const
	{
		return m_eState != State::Playing && m_fTimer >= GetAnimationDuration();
	}

	float Anm2::GetAnimationCurrentTime() const
	{
		return m_fTimer;
	}

	void Anm2::FlipX( int _iFlip /*= 0 */ )
	{
		if( m_vFlippedScale.x > 0.f && ( _iFlip == -1 || _iFlip == 0 ) )
			m_vFlippedScale = { -1.f, 1.f };
		else if( m_vFlippedScale.x < 0.f && ( _iFlip == 1 || _iFlip == 0 ) )
			m_vFlippedScale = { 1.f, 1.f };
		
		_UpdateFlippedScale( m_oLayers );
		_UpdateFlippedScale( m_oSockets );
	}

	int Anm2::GetFlipX() const
	{
		if( m_vFlippedScale.x < 0.f )
			return -1;

		return 1;
	}

	const fzn::Anm2::LayerInfo* Anm2::GetLayer( const std::string& _sName ) const
	{
		for( const LayerInfo& oLayer : m_oLayers )
		{
			if( oLayer.m_sName == _sName )
				return &oLayer;
		}

		return nullptr;
	}

	const fzn::Anm2::LayerInfo* Anm2::GetSocket( const std::string& _sName ) const
	{
		for( const LayerInfo& oSocket : m_oSockets )
		{
			if( oSocket.m_sName == _sName )
				return &oSocket;
		}

		return nullptr;
	}

	sf::Color Anm2::GetLayerCurrentColorOverlay( const std::string& _sName ) const
	{
		LayerInfoVector::const_iterator it = std::find_if( m_oLayers.cbegin(), m_oLayers.cend(), [ _sName ]( const LayerInfo& oLayer ){ return oLayer.m_sName == _sName; } );

		if( it != m_oLayers.cend() )
		{
			return it->GetCurrentColorOverlay( m_fSpeedRatio );
		}

		return sf::Color( 0, 0, 0, 0 );
	}


	//-------------------------------------------------------------------------------------------------
	/// Animation triggers callbacks
	//-------------------------------------------------------------------------------------------------

	void Anm2::AddTriggerCallback( const std::string& _sTrigger, TriggerCallback _pCallback, bool _bRemoveCallbackWhenCalled /*= false*/ )
	{
		if( _sTrigger.empty() || _pCallback == nullptr )
			return;

		if( _sTrigger == ANIMATION_START )
		{
			AddAnimationStartCallback( _pCallback );
			return;
		}
		else if( _sTrigger == ANIMATION_END )
		{
			AddAnimationEndCallback( _pCallback );
			return;
		}

		if( m_bIsProcessingTriggers && m_bReplaceTriggers )
			_AddContentToTriggerVector( m_oTriggersBuffer, TriggerContent( _pCallback, _bRemoveCallbackWhenCalled ), _sTrigger );
		else
			_AddContentToTriggerVector( m_oTriggers, TriggerContent( _pCallback, _bRemoveCallbackWhenCalled ), _sTrigger );
	}

	void Anm2::AddTriggerCallback( int _iFrame, const std::string& _sTrigger, TriggerCallback _pCallback, bool _bRemoveCallbackWhenCalled /*= false*/ )
	{
		if( _sTrigger.empty() || _pCallback == nullptr )
			return;

		const float fTime = _iFrame * AnimFrameTime;

		if( m_bIsProcessingTriggers && m_bReplaceTriggers )
			_AddContentToTriggerVector( m_oTriggersBuffer, TriggerContent( _pCallback, _bRemoveCallbackWhenCalled ), _sTrigger, fTime );
		else
			_AddContentToTriggerVector( m_oTriggers, TriggerContent( _pCallback, _bRemoveCallbackWhenCalled ), _sTrigger, fTime );
	}

	bool Anm2::HasAnimationCallback( const std::string& _sTrigger, const TriggerCallback _pCallback ) const
	{
		if( _sTrigger.empty() || _pCallback == nullptr )
			return false;

		if( _sTrigger == ANIMATION_START )
			return HasAnimationStartCallback( _pCallback );
		else if( _sTrigger == ANIMATION_END )
			return HasAnimationEndCallback( _pCallback );

		return _TriggerVectorHasContent( m_oTriggers, TriggerContent( _pCallback ), _sTrigger );
	}

	void Anm2::RemoveTriggerCallback( const std::string& _sTrigger, const TriggerCallback _pCallback )
	{
		if( _sTrigger.empty() || _pCallback == nullptr )
			return;

		_RemoveContentFromTriggerVector( m_oTriggers, TriggerContent( _pCallback ), _sTrigger );
	}

	void Anm2::AddAnimationStartCallback( TriggerCallback _pCallback, bool _bRemoveCallbackWhenCalled /*= false*/ )
	{
		if( _pCallback == nullptr )
			return;

		if( m_bIsProcessingTriggers && m_bReplaceTriggers )
			_AddContentToTriggerContentVector( m_oAnimationStartTriggerBuffer.m_oContent, TriggerContent( _pCallback, _bRemoveCallbackWhenCalled ) );
		else
			_AddContentToTriggerContentVector( m_oAnimationStartTrigger.m_oContent, TriggerContent( _pCallback ) );
	}

	bool Anm2::HasAnimationStartCallback( const TriggerCallback _pCallback ) const
	{
		if( _pCallback == nullptr )
			return false;

		return _TriggerContentVectorHasContent( m_oAnimationStartTrigger.m_oContent, TriggerContent( _pCallback ) );
	}

	void Anm2::RemoveAnimationStartCallback( const TriggerCallback _pCallback )
	{
		if( _pCallback == nullptr )
			return;

		_RemoveContentFromTriggerContentVector( m_oAnimationStartTrigger.m_oContent, TriggerContent( _pCallback ) );
	}

	void Anm2::AddAnimationEndCallback( TriggerCallback _pCallback, bool _bRemoveCallbackWhenCalled /*= false*/ )
	{
		if( _pCallback == nullptr )
			return;

		if( m_bIsProcessingTriggers && m_bReplaceTriggers )
			_AddContentToTriggerContentVector( m_oAnimationEndTriggerBuffer.m_oContent, TriggerContent( _pCallback, _bRemoveCallbackWhenCalled ) );
		else
			_AddContentToTriggerContentVector( m_oAnimationEndTrigger.m_oContent, TriggerContent( _pCallback ) );
	}

	bool Anm2::HasAnimationEndCallback( const TriggerCallback _pCallback ) const
	{
		if( _pCallback == nullptr )
			return false;

		return _TriggerContentVectorHasContent( m_oAnimationEndTrigger.m_oContent, TriggerContent( _pCallback ) );
	}

	void Anm2::RemoveAnimationEndCallback( const TriggerCallback _pCallback )
	{
		if( _pCallback == nullptr )
			return;

		_RemoveContentFromTriggerContentVector( m_oAnimationEndTrigger.m_oContent, TriggerContent( _pCallback ) );
	}


	//-------------------------------------------------------------------------------------------------
	/// Animation triggers sounds
	//-------------------------------------------------------------------------------------------------

	void Anm2::AddTriggerSound( const std::string& _sTrigger, const std::string& _sSound, bool _bRemoveCallbackWhenCalled /*= false*/ )
	{
		if( _sTrigger.empty() )
			return;

		if( _sTrigger == ANIMATION_START )
		{
			AddAnimationStartSound( _sSound );
			return;
		}
		else if( _sTrigger == ANIMATION_END )
		{
			AddAnimationEndSound( _sSound );
			return;
		}

		if( g_pFZN_AudioMgr->IsSoundValid( _sSound ) == false )
		{
			FZN_COLOR_LOG( fzn::DBG_MSG_COLORS::DBG_MSG_COL_RED, "Trying to add invalid sound \"%s\" to trigger \"%s\" in animation \"%s\".", _sSound.c_str(), _sTrigger.c_str(), m_sName.c_str() );
			return;
		}

		if( m_bIsProcessingTriggers && m_bReplaceTriggers )
			_AddContentToTriggerVector( m_oTriggersBuffer, TriggerContent( _sSound, _bRemoveCallbackWhenCalled ), _sTrigger );
		else
			_AddContentToTriggerVector( m_oTriggers, TriggerContent( _sSound, _bRemoveCallbackWhenCalled ), _sTrigger );
	}

	void Anm2::AddTriggerSounds( const std::string& _sTrigger, const std::vector< std::string >& _oSound, bool _bRemoveCallbackWhenCalled /*= false*/ )
	{
		if( _sTrigger.empty() )
			return;

		if( _sTrigger == ANIMATION_START )
		{
			for( const std::string& sSound : _oSound )
				AddAnimationStartSound( sSound );
			return;
		}
		else if( _sTrigger == ANIMATION_END )
		{
			for( const std::string& sSound : _oSound )
				AddAnimationEndSound( sSound );
			return;
		}

		TriggerVector& oVector = ( m_bIsProcessingTriggers && m_bReplaceTriggers ) ? m_oTriggersBuffer : m_oTriggers;

		for( const std::string& sSound : _oSound )
		{
			if( g_pFZN_AudioMgr->IsSoundValid( sSound ) == false )
			{
				FZN_COLOR_LOG( fzn::DBG_MSG_COLORS::DBG_MSG_COL_RED, "Trying to add invalid sound \"%s\" to trigger \"%s\" in animation \"%s\".", sSound.c_str(), _sTrigger.c_str(), m_sName.c_str() );
				continue;;
			}

			_AddContentToTriggerVector( oVector, TriggerContent( sSound, _bRemoveCallbackWhenCalled ), _sTrigger );
		}
	}

	bool Anm2::HasTriggerSound( const std::string& _sTrigger, const std::string& _sSound ) const
	{
		if( _sTrigger.empty() || g_pFZN_AudioMgr->IsSoundValid( _sSound ) == false )
			return false;

		if( _sTrigger == ANIMATION_START )
			return HasAnimationStartSound( _sSound );
		else if( _sTrigger == ANIMATION_END )
			return HasAnimationEndSound( _sSound );

		return _TriggerVectorHasContent( m_oTriggers, TriggerContent( _sSound ), _sTrigger );
	}

	void Anm2::RemoveTriggerSound( const std::string& _sTrigger, const std::string& _sSound )
	{
		if( _sTrigger.empty() || g_pFZN_AudioMgr->IsSoundValid( _sSound ) == false )
			return;

		_RemoveContentFromTriggerVector( m_oTriggers, TriggerContent( _sSound ), _sTrigger );
	}

	void Anm2::AddAnimationStartSound( const std::string& _sSound, bool _bRemoveCallbackWhenCalled /*= false*/ )
	{
		if( g_pFZN_AudioMgr->IsSoundValid( _sSound ) == false )
		{
			FZN_COLOR_LOG( fzn::DBG_MSG_COLORS::DBG_MSG_COL_RED, "Trying to add invalid sound \"%s\" to START trigger in animation \"%s\".", _sSound.c_str(), m_sName.c_str() );
			return;
		}

		if( m_bIsProcessingTriggers && m_bReplaceTriggers )
			_AddContentToTriggerContentVector( m_oAnimationStartTriggerBuffer.m_oContent, TriggerContent( _sSound, _bRemoveCallbackWhenCalled ) );
		else
			_AddContentToTriggerContentVector( m_oAnimationStartTrigger.m_oContent, TriggerContent( _sSound, _bRemoveCallbackWhenCalled ) );
	}

	bool Anm2::HasAnimationStartSound( const std::string& _sSound ) const
	{
		return _TriggerContentVectorHasContent( m_oAnimationStartTrigger.m_oContent, _sSound );
	}

	void Anm2::RemoveAnimationStartSound( const std::string& _sSound )
	{
		_RemoveContentFromTriggerContentVector( m_oAnimationStartTrigger.m_oContent, _sSound );
	}

	void Anm2::AddAnimationEndSound( const std::string& _sSound, bool _bRemoveCallbackWhenCalled /*= false*/ )
	{
		if( g_pFZN_AudioMgr->IsSoundValid( _sSound ) == false )
		{
			FZN_COLOR_LOG( fzn::DBG_MSG_COLORS::DBG_MSG_COL_RED, "Trying to add invalid sound \"%s\" to END trigger in animation \"%s\".", _sSound.c_str(), m_sName.c_str() );
			return;
		}

		if( m_bIsProcessingTriggers && m_bReplaceTriggers )
			_AddContentToTriggerContentVector( m_oAnimationEndTriggerBuffer.m_oContent, TriggerContent( _sSound, _bRemoveCallbackWhenCalled ) );
		else
			_AddContentToTriggerContentVector( m_oAnimationEndTrigger.m_oContent, TriggerContent( _sSound, _bRemoveCallbackWhenCalled ) );
	}

	bool Anm2::HasAnimationEndSound( const std::string& _sSound ) const
	{
		return _TriggerContentVectorHasContent( m_oAnimationEndTrigger.m_oContent, _sSound );
	}

	void Anm2::RemoveAnimationEndSound( const std::string& _sSound )
	{
		_RemoveContentFromTriggerContentVector( m_oAnimationEndTrigger.m_oContent, _sSound );
	}

	float Anm2::GetTriggerTime( const std::string& _sTrigger ) const
	{
		TriggerVector::const_iterator it = _FindTriggerInTriggerVector( m_oTriggers, _sTrigger );

		if( it != m_oTriggers.cend() )
			return it->m_fTime;

		return -1.f;
	}

	const std::string& Anm2::GetName() const
	{
		return m_sName;
	}

	void Anm2::draw( sf::RenderTarget& _oTarget, sf::RenderStates _oStates ) const
	{
		if( m_bVisible == false || m_oLayers.empty() )
			return;

		const bool bCanOverrideShader = _oStates.shader == nullptr;

		for( const Anm2::LayerInfo& oLayer : m_oLayers )
		{
			if( oLayer.IsVisible() )
			{
				sf::Color oOverlay = oLayer.GetCurrentColorOverlay( m_fSpeedRatio );

				if( oOverlay.a != 0 )
				{
					sf::Shader* pShader = g_pFZN_DataMgr->GetShader( "ColorOverlay" );

					if( pShader != nullptr && bCanOverrideShader )
					{
						pShader->setUniform( "texture", sf::Shader::CurrentTexture );
						pShader->setUniform( "tintColor", sf::Glsl::Vec4( oOverlay ) );
						_oStates.shader = pShader;
					}
				}

				_oTarget.draw( oLayer.m_oSprite, _oStates );
			}
		}
	}

	void Anm2::_GetRootAnimationInformations( tinyxml2::XMLElement* _pAnimation )
	{
		if( _pAnimation == nullptr )
			return;

		tinyxml2::XMLElement* pElement = _pAnimation->FirstChildElement( "RootAnimation" );

		if( pElement == nullptr )
			return;

		_FillFrameInformations( m_oRootAnimation, pElement->FirstChildElement( "Frame" ) );
	}

	void Anm2::_GetLayersInformations( tinyxml2::XMLElement* _pContent, tinyxml2::XMLElement* _pAnimation, const std::string& _sPath )
	{
		if( _pContent == nullptr || _pAnimation == nullptr )
			return;

		tinyxml2::XMLElement* pLayerAnimations	= _pAnimation->FirstChildElement( "LayerAnimations" );
		tinyxml2::XMLElement* pLayers			= _pContent->FirstChildElement( "Layers" );
		tinyxml2::XMLElement* pSpritesheets		= _pContent->FirstChildElement( "Spritesheets" );

		if( pLayerAnimations == nullptr || pLayers == nullptr || pSpritesheets == nullptr )
			return;

		tinyxml2::XMLElement* pCurrentLayerAnimation	= pLayerAnimations->FirstChildElement( "LayerAnimation" );
		tinyxml2::XMLElement* pCurrentFrame				= nullptr;
		tinyxml2::XMLElement* pCurrentLayer				= nullptr;
		tinyxml2::XMLElement* pCurrentSpritesheet		= nullptr;

		FrameInfo oFrameInfo;
		int iLayerId = -1;
		int iSpritesheetId = -1;
		bool bLayerFound = false;
		bool bSpritesheetFound = false;
		std::vector<int> oFoundLayers;
		std::string sSpritesheetPath;
		std::string sSpritesheetName;
		LayerInfo oLayerInfo;

		while( pCurrentLayerAnimation != nullptr )
		{
			oLayerInfo.m_bVisible = pCurrentLayerAnimation->BoolAttribute( "Visible" );
			pCurrentFrame = pCurrentLayerAnimation->FirstChildElement( "Frame" );

			//FZN_LOG( true, DBG_MSG_COL_PURPLE, "----------------------------------------" );
			//FZN_LOG( true, DBG_MSG_COL_PURPLE, "Looking for frames in layer %d", pCurrentLayerAnimation->IntAttribute( "LayerId" ) );

			if( pCurrentFrame != nullptr )
			{
				iLayerId = pCurrentLayerAnimation->IntAttribute( "LayerId" );
				//FZN_LOG( true, DBG_MSG_COL_PURPLE, "\tFound some frames !" );

				///////////////////////////////////////////////////

				//Fill all the frames with their informations.
				while( pCurrentFrame != nullptr )
				{
					_FillFrameInformations( oFrameInfo, pCurrentFrame );
					
					oLayerInfo.m_oFrames.push_back( oFrameInfo );
					oFrameInfo.Reset();
					pCurrentFrame = pCurrentFrame->NextSiblingElement();
					//FZN_LOG( true, DBG_MSG_COL_PURPLE, "\t\tFrame %d pushed in the vector", oLayerInfo.m_oFrames.Size() );
				}

				///////////////////////////////////////////////////
				//FZN_LOG( true, DBG_MSG_COL_PURPLE, "\tRetrieving spritesheet Id from Layer...", pCurrentLayerAnimation->IntAttribute( "LayerId" ) );
				pCurrentLayer = pLayers->FirstChildElement( "Layer" );

				while( pCurrentLayer != nullptr || !bLayerFound )
				{
					if( pCurrentLayer->IntAttribute( "Id" ) == iLayerId )
					{
						oLayerInfo.m_sName = Tools::XMLStringAttribute( pCurrentLayer, "Name" );
						iSpritesheetId = pCurrentLayer->IntAttribute( "SpritesheetId" );

						//FZN_LOG( true, DBG_MSG_COL_PURPLE, "\t\tCorresponding spritesheet Id is %d", iSpritesheetId );
						
						int iBaseTextureIndex = -1;

						for( int iIndex : oFoundLayers )
						{
							if( iIndex == iSpritesheetId )
							{
								iBaseTextureIndex = iIndex;
								break;
							}
						}

						if( iBaseTextureIndex == -1 )
						{
							pCurrentSpritesheet = pSpritesheets->FirstChildElement( "Spritesheet" );

							while( pCurrentSpritesheet != nullptr || !bSpritesheetFound )
							{
								if( pCurrentSpritesheet->IntAttribute( "Id" ) == iSpritesheetId )
								{
									sSpritesheetName = Tools::XMLStringAttribute( pCurrentSpritesheet, "Path" );
									sSpritesheetPath = _sPath;
									RemoveFileName( sSpritesheetPath );
									sSpritesheetPath = sSpritesheetPath + sSpritesheetName;
									sSpritesheetName = Tools::GetFileNameFromPath( sSpritesheetName );
									
									sf::Texture* pSpritesheetTexture;
									pSpritesheetTexture = g_pFZN_DataMgr->LoadTexture( sSpritesheetName, sSpritesheetPath );
									oLayerInfo.m_oSprite.setTexture( *pSpritesheetTexture );

									//FZN_LOG( true, DBG_MSG_COL_PURPLE, "\t\t\tSpritesheet name is \"%s\"", sSpritesheetName.c_str() );
									//FZN_LOG( true, DBG_MSG_COL_PURPLE, "\t\t\tSpritesheet path is \"%s\"", pCurrentSpritesheet->Attribute( "Path" ) );
									//FZN_LOG( true, DBG_MSG_COL_PURPLE, "\t\t\tSpritesheet full path is \"%s\"", sSpritesheetPath.c_str() );
									oFoundLayers.push_back( iSpritesheetId );
									oLayerInfo.m_iBaseTextureId = oFoundLayers.back();
									m_oBaseTextures[ oLayerInfo.m_iBaseTextureId ].first = sSpritesheetName;
									m_oBaseTextures[ oLayerInfo.m_iBaseTextureId ].second = pSpritesheetTexture;
									bSpritesheetFound = true;
								}

								pCurrentSpritesheet = pCurrentSpritesheet->NextSiblingElement();
							}

						}
						else
						{
							oLayerInfo.m_iBaseTextureId = iBaseTextureIndex;
							oLayerInfo.m_oSprite.setTexture( *m_oBaseTextures[iBaseTextureIndex].second );
						}

						bLayerFound = true;
					}

					pCurrentLayer = pCurrentLayer->NextSiblingElement();
				}

				bLayerFound = false;
				bSpritesheetFound = false;
				sSpritesheetPath = _sPath;

				///////////////////////////////////////////////////

				m_oLayers.push_back( oLayerInfo );
				oLayerInfo.Reset();
				//FZN_LOG( true, DBG_MSG_COL_PURPLE, "%d Frames found in layer %d (%s)", m_oLayers.Back().m_oFrames.Size(), iLayerId, m_oLayers.Back().m_sName.c_str() );
			}

			pCurrentLayerAnimation = pCurrentLayerAnimation->NextSiblingElement();
		}
	}

	void Anm2::_GetSocketsInformations( tinyxml2::XMLElement* _pContent, tinyxml2::XMLElement* _pAnimation )
	{
		if( _pContent == nullptr || _pAnimation == nullptr )
			return;

		tinyxml2::XMLElement* pNullAnimations	= _pAnimation->FirstChildElement( "NullAnimations" );
		tinyxml2::XMLElement* pNulls			= _pContent->FirstChildElement( "Nulls" );

		if( pNullAnimations == nullptr || pNulls == nullptr )
			return;

		tinyxml2::XMLElement* pCurrentNullAnimation = pNullAnimations->FirstChildElement( "NullAnimation" );
		tinyxml2::XMLElement* pCurrentFrame			= nullptr;
		tinyxml2::XMLElement* pCurrentNull			= nullptr;

		LayerInfo oSocketInfo;
		FrameInfo oFrameInfo;
		int iSocketId = -1;
		bool bSocketFound = false;

		while( pCurrentNullAnimation != nullptr )
		{
			//FZN_LOG( true, DBG_MSG_COL_PURPLE, "..................................." );
			//FZN_LOG( true, DBG_MSG_COL_PURPLE, "Looking for frames in socket %d", pCurrentNullAnimation->IntAttribute( "NullId" ) );

			pCurrentFrame = pCurrentNullAnimation->FirstChildElement( "Frame" );

			if( pCurrentFrame != nullptr )
			{
				//FZN_LOG( true, DBG_MSG_COL_PURPLE, "\tFound some frames !", oSocketInfo.m_oFrames.Size() );
				iSocketId = pCurrentNullAnimation->IntAttribute( "NullId" );

				while( pCurrentFrame != nullptr )
				{
					_FillFrameInformations( oFrameInfo, pCurrentFrame );

					oSocketInfo.m_oFrames.push_back( oFrameInfo );
					oFrameInfo.Reset();
					pCurrentFrame = pCurrentFrame->NextSiblingElement();
					//FZN_LOG( true, DBG_MSG_COL_PURPLE, "\t\tFrame %d pushed in the vector", oSocketInfo.m_oFrames.Size() );
				}

				pCurrentNull = pNulls->FirstChildElement( "Null" );

				while( pCurrentNull != nullptr || !bSocketFound )
				{
					if( pCurrentNull->IntAttribute( "Id" ) == iSocketId )
					{
						oSocketInfo.m_sName = Tools::XMLStringAttribute( pCurrentNull, "Name" );
						bSocketFound = true;
						//FZN_LOG( true, DBG_MSG_COL_PURPLE, "\tSocket #%d name is \"%s\"", iSocketId, oSocketInfo.m_sName.c_str() );
					}

					pCurrentNull = pCurrentNull->NextSiblingElement();
				}

				m_oSockets.push_back( oSocketInfo );
				bSocketFound = false;
				oSocketInfo.Reset();
				//FZN_LOG( true, DBG_MSG_COL_PURPLE, "%d Frames found in layer %d (%s)", m_oSockets.Back().m_oFrames.Size(), iSocketId, m_oSockets.Back().m_sName.c_str() );
			}

			pCurrentNullAnimation = pCurrentNullAnimation->NextSiblingElement();
		}
	}

	void Anm2::_GetTriggersInformations( tinyxml2::XMLElement* _pContent, tinyxml2::XMLElement* _pAnimation )
	{
		if( _pContent == nullptr || _pAnimation == nullptr )
			return;

		tinyxml2::XMLElement* pAnimationTriggers	= _pAnimation->FirstChildElement( "Triggers" );
		tinyxml2::XMLElement* pEvents				= _pContent->FirstChildElement( "Events" );

		if( pAnimationTriggers == nullptr || pEvents == nullptr )
			return;


		tinyxml2::XMLElement* pXMLTrigger	= pAnimationTriggers->FirstChildElement( "Trigger" );

		while( pXMLTrigger != nullptr )
		{
			Trigger oTrigger;
			const int iEventId = pXMLTrigger->IntAttribute( "EventId" );
			tinyxml2::XMLElement* pEvent	= pEvents->FirstChildElement( "Event" );

			while( pEvent != nullptr )
			{
				if( pEvent->IntAttribute( "Id" ) == iEventId )
				{
					oTrigger.m_sName = Tools::XMLStringAttribute( pEvent, "Name" );
					break;
				}

				pEvent = pEvent->NextSiblingElement();
			}

			oTrigger.m_fTime = pXMLTrigger->IntAttribute( "AtFrame" ) * AnimFrameTime;

			if( oTrigger.IsValid() )
				m_oTriggers.push_back( oTrigger );

			pXMLTrigger = pXMLTrigger->NextSiblingElement();
		}
	}

	void Anm2::_FillFrameInformations( FrameInfo& _oFrame, tinyxml2::XMLElement* _pXMLFrame )
	{
		if( _pXMLFrame == nullptr )
			return;

		_oFrame.m_vPosition.x		= _pXMLFrame->FloatAttribute( "XPosition" );
		_oFrame.m_vPosition.y		= _pXMLFrame->FloatAttribute( "YPosition" );
		_oFrame.m_vOrigin.x			= _pXMLFrame->FloatAttribute( "XPivot" );
		_oFrame.m_vOrigin.y			= _pXMLFrame->FloatAttribute( "YPivot" );
		_oFrame.m_oFrameRect.left	= _pXMLFrame->IntAttribute( "XCrop" );
		_oFrame.m_oFrameRect.top	= _pXMLFrame->IntAttribute( "YCrop" );
		_oFrame.m_oFrameRect.width	= _pXMLFrame->IntAttribute( "Width" );
		_oFrame.m_oFrameRect.height = _pXMLFrame->IntAttribute( "Height" );
		_oFrame.m_vScale.x			= _pXMLFrame->FloatAttribute( "XScale" ) * 0.01f;
		_oFrame.m_vScale.y			= _pXMLFrame->FloatAttribute( "YScale" ) * 0.01f;
		_oFrame.m_fDuration			= _pXMLFrame->FloatAttribute( "Delay" ) * AnimFrameTime;
		_oFrame.m_bVisible			= _pXMLFrame->BoolAttribute( "Visible" );
		_oFrame.m_fRotation			= _pXMLFrame->FloatAttribute( "Rotation" );
		_oFrame.m_bInterpolate		= _pXMLFrame->BoolAttribute( "Interpolated" );

		_oFrame.m_oColor.r			= _pXMLFrame->IntAttribute( "RedTint" );
		_oFrame.m_oColor.g			= _pXMLFrame->IntAttribute( "GreenTint" );
		_oFrame.m_oColor.b			= _pXMLFrame->IntAttribute( "BlueTint" );
		_oFrame.m_oColor.a			= _pXMLFrame->IntAttribute( "AlphaTint" );

		_oFrame.m_oColorOverlay.r	= _pXMLFrame->IntAttribute( "RedOffset" );
		_oFrame.m_oColorOverlay.g	= _pXMLFrame->IntAttribute( "GreenOffset" );
		_oFrame.m_oColorOverlay.b	= _pXMLFrame->IntAttribute( "BleuOffset" );

		if( _oFrame.m_oColorOverlay.r != 0 || _oFrame.m_oColorOverlay.r != 0 || _oFrame.m_oColorOverlay.r != 0 )
			_oFrame.m_oColorOverlay.a = 180;
		else
			_oFrame.m_oColorOverlay.a = 0;


		// Socket Backup
		/*oFrameInfo.m_vPosition.x = pCurrentFrame->FloatAttribute( "XPosition" );
		oFrameInfo.m_vPosition.y = pCurrentFrame->FloatAttribute( "YPosition" );
		oFrameInfo.m_vScale.x = pCurrentFrame->FloatAttribute( "XScale" );
		oFrameInfo.m_vScale.y = pCurrentFrame->FloatAttribute( "YScale" );
		oFrameInfo.m_fDuration = pCurrentFrame->FloatAttribute( "Delay" ) * AnimFrameTime;
		oFrameInfo.m_bVisible = pCurrentFrame->BoolAttribute( "Visible" );*/
	}

	void Anm2::_UpdateIndex( LayerInfo& oLayer )
	{
		if( oLayer.m_iFrameIndex < (int)oLayer.m_oFrames.size() - 1 )
			oLayer.m_iFrameIndex++;
	}

	void Anm2::_ExtractFileName( std::string& _sFile )
	{
		int iSlashIndex = _sFile.find_last_of( "/" );

		if( iSlashIndex == std::string::npos )
			iSlashIndex = _sFile.find_last_of( "\\" );

		if( iSlashIndex == std::string::npos )
			_sFile = _sFile.substr( iSlashIndex + 1 );
			 //return;

		_sFile = _sFile.substr( 0, _sFile.find_last_of( "." ) );
	}

	void Anm2::_Init()
	{
		if( m_oLayers.empty() && m_oSockets.empty() )
			return;

		for( LayerInfo& oLayer : m_oLayers )
			_UpdateLayerInfos( oLayer );

		for( LayerInfo& oSocket : m_oSockets )
			_UpdateLayerInfos( oSocket );
	}

	void Anm2::_UpdateLayerInfoVector( LayerInfoVector& _oVector, bool _bRestart )
	{
		for( LayerInfo& oCurrentLayer : _oVector )
		{
			const FrameInfo& oCurrentFrameInfo = oCurrentLayer.m_oFrames[ oCurrentLayer.m_iFrameIndex ];
			oCurrentLayer.m_fTimer += m_bUseUnmodifiedFrameTime ? UnmodifiedFrameTime : FrameTime;

			const float fDuration = oCurrentFrameInfo.m_fDuration * m_fSpeedRatio;

			if( oCurrentFrameInfo.m_bInterpolate && oCurrentLayer.m_iFrameIndex + 1 < (int)oCurrentLayer.m_oFrames.size() )
			{
				const FrameInfo& oNextFrame = oCurrentLayer.m_oFrames[ oCurrentLayer.m_iFrameIndex + 1 ];

				sf::Vector2f vNewPosition = fzn::Math::Interpolate( 0.f, fDuration, oCurrentFrameInfo.m_vPosition, oNextFrame.m_vPosition, oCurrentLayer.m_fTimer );
				sf::Vector2f vNewOrigin = fzn::Math::Interpolate( 0.f, fDuration, oCurrentFrameInfo.m_vOrigin, oNextFrame.m_vOrigin, oCurrentLayer.m_fTimer );
				sf::Vector2f vNewScale = fzn::Math::Interpolate( 0.f, fDuration, oCurrentFrameInfo.m_vScale, oNextFrame.m_vScale, oCurrentLayer.m_fTimer );
				float fNewRotation = fzn::Math::Interpolate( 0.f, fDuration, oCurrentFrameInfo.m_fRotation, oNextFrame.m_fRotation, oCurrentLayer.m_fTimer );
				
				sf::Uint8 uNewRed	= (sf::Uint8)fzn::Math::Interpolate( 0.f, fDuration, (float)oCurrentFrameInfo.m_oColor.r, (float)oNextFrame.m_oColor.r, oCurrentLayer.m_fTimer );
				sf::Uint8 uNewGreen = (sf::Uint8)fzn::Math::Interpolate( 0.f, fDuration, (float)oCurrentFrameInfo.m_oColor.g, (float)oNextFrame.m_oColor.g, oCurrentLayer.m_fTimer );
				sf::Uint8 uNewBlue	= (sf::Uint8)fzn::Math::Interpolate( 0.f, fDuration, (float)oCurrentFrameInfo.m_oColor.b, (float)oNextFrame.m_oColor.b, oCurrentLayer.m_fTimer );
				sf::Uint8 uNewAlpha = (sf::Uint8)fzn::Math::Interpolate( 0.f, fDuration, (float)oCurrentFrameInfo.m_oColor.a, (float)oNextFrame.m_oColor.a, oCurrentLayer.m_fTimer );

				vNewScale.x *= m_vFlippedScale.x;
				vNewScale.y *= m_vFlippedScale.y;

				if( m_vFlippedScale.x < 0.f )
				{
					float fRect = fzn::Math::Interpolate( 0.f, oCurrentFrameInfo.m_fDuration, (float)oCurrentFrameInfo.m_oFrameRect.width, (float)oNextFrame.m_oFrameRect.width, oCurrentLayer.m_fTimer );
					vNewOrigin.x = fRect - vNewOrigin.x;
				}

				vNewPosition += m_oRootAnimation.m_vPosition;
				fNewRotation += m_oRootAnimation.m_fRotation;

				oCurrentLayer.m_oSprite.setOrigin( vNewOrigin );
				oCurrentLayer.m_oSprite.setScale( vNewScale * m_fScaleRatio );
				oCurrentLayer.m_oSprite.setRotation( m_fRotation + fNewRotation );
				oCurrentLayer.m_oSprite.setPosition( m_vPosition + vNewPosition );
				oCurrentLayer.m_oSprite.setColor( sf::Color( uNewRed, uNewGreen, uNewBlue, uNewAlpha ) );
			}

			if( _bRestart || oCurrentLayer.m_fTimer >= fDuration )
			{
				if( _bRestart )
					oCurrentLayer.m_iFrameIndex = 0;
				else if( oCurrentLayer.m_iFrameIndex < (int)oCurrentLayer.m_oFrames.size() - 1 )
					++oCurrentLayer.m_iFrameIndex;

				_UpdateLayerInfos( oCurrentLayer, _bRestart ? -1.f : fDuration );
			}
			++oCurrentLayer.m_iFrameCount;
		}
	}

	void Anm2::_UpdateLayerInfos( LayerInfo& _oLayer, float _fPreviousFrameDuration /*= -1.f*/ )
	{
		const FrameInfo& oFrameInfo = _oLayer.m_oFrames[ _oLayer.m_iFrameIndex ];
		sf::Vector2f vNewScale( oFrameInfo.m_vScale.x * m_vFlippedScale.x, oFrameInfo.m_vScale.y * m_vFlippedScale.y );
		sf::Vector2f vNewOrigin( oFrameInfo.m_vOrigin );

		if( m_vFlippedScale.x < 0.f )
			vNewOrigin.x = oFrameInfo.m_oFrameRect.width - vNewOrigin.x;

		const sf::Vector2f vNewPosition	= m_oRootAnimation.m_vPosition + oFrameInfo.m_vPosition;
		const float fNewRotation		= m_oRootAnimation.m_fRotation + oFrameInfo.m_fRotation;

		_oLayer.m_oSprite.setTextureRect( oFrameInfo.m_oFrameRect );
		_oLayer.m_oSprite.setColor( oFrameInfo.m_oColor );
		_oLayer.m_oSprite.setOrigin( vNewOrigin );
		_oLayer.m_oSprite.setScale( vNewScale * m_fScaleRatio );
		_oLayer.m_oSprite.setPosition( m_vPosition + vNewPosition );
		_oLayer.m_oSprite.setRotation( m_fRotation + fNewRotation );

		if( _fPreviousFrameDuration >= 0.f )
			_oLayer.m_fTimer -= _fPreviousFrameDuration;
		else
			_oLayer.m_fTimer = 0.f;

		_oLayer.m_iFrameCount = 0;
	}

	void Anm2::_ChangeAnimationLayerSetup( LayerInfo& _oDstLayer, const LayerInfo& _oSrcLayer )
	{
		_oDstLayer.m_sName			= _oSrcLayer.m_sName;
		_oDstLayer.m_oFrames		= _oSrcLayer.m_oFrames;
		_oDstLayer.m_iBaseTextureId = _oSrcLayer.m_iBaseTextureId;
		_oDstLayer.m_oSprite		= _oSrcLayer.m_oSprite;

		const FrameInfo& oFrameInfo = _oDstLayer.m_oFrames[ _oDstLayer.m_iFrameIndex ];
		sf::Vector2f vNewScale( oFrameInfo.m_vScale.x * m_vFlippedScale.x, oFrameInfo.m_vScale.y * m_vFlippedScale.y );
		sf::Vector2f vNewOrigin( oFrameInfo.m_vOrigin );

		if( m_vFlippedScale.x < 0.f )
			vNewOrigin.x = oFrameInfo.m_oFrameRect.width - vNewOrigin.x;

		_oDstLayer.m_oSprite.setTextureRect( oFrameInfo.m_oFrameRect );
		_oDstLayer.m_oSprite.setColor( oFrameInfo.m_oColor );
		_oDstLayer.m_oSprite.setOrigin( vNewOrigin );
		_oDstLayer.m_oSprite.setScale( vNewScale * m_fScaleRatio );
	}

	void Anm2::_UpdateTriggers( bool _bAnimStarting )
	{
		if( _bAnimStarting )
			_ProcessTriggerContents( m_oAnimationStartTrigger );

		for( Trigger& oTrigger : m_oTriggers )
		{
			const float fTargetTime = oTrigger.m_fTime * m_fSpeedRatio;

			if( m_fTimer >= fTargetTime && ( m_fPreviousTimer < fTargetTime || fTargetTime == 0.f && m_fPreviousTimer == 0.f ) )
			{
				_ProcessTriggerContents( oTrigger );
			}
		}

		_ProcessTriggersBuffers();
	}

	void Anm2::_UpdateFlippedScale( LayerInfoVector& _oVector )
	{
		for( LayerInfo& oCurrentLayer : _oVector )
		{
			const FrameInfo& oFrameInfo = oCurrentLayer.m_oFrames[ oCurrentLayer.m_iFrameIndex ];
			sf::Vector2f vNewScale( oFrameInfo.m_vScale.x * m_vFlippedScale.x, oFrameInfo.m_vScale.y * m_vFlippedScale.y );
			sf::Vector2f vNewOrigin( oFrameInfo.m_vOrigin );

			if( m_vFlippedScale.x < 0.f )
			{
				float fRect = (float)oFrameInfo.m_oFrameRect.width;

				if( oFrameInfo.m_bInterpolate && oCurrentLayer.m_iFrameIndex + 1 < (int)oCurrentLayer.m_oFrames.size() )
				{
					const FrameInfo& oNextFrame = oCurrentLayer.m_oFrames[ oCurrentLayer.m_iFrameIndex + 1 ];
					fRect = fzn::Math::Interpolate( 0.f, oFrameInfo.m_fDuration * oFrameInfo.m_fDuration, (float)oFrameInfo.m_oFrameRect.width, (float)oNextFrame.m_oFrameRect.width, oCurrentLayer.m_fTimer );
				}

				vNewOrigin.x = fRect - vNewOrigin.x;
			}

			oCurrentLayer.m_oSprite.setOrigin( vNewOrigin );
			oCurrentLayer.m_oSprite.setScale( vNewScale * m_fScaleRatio );
		}
	}

	float Anm2::_GetLayerTotalDuration( const LayerInfo& _oLayer ) const
	{
		float fTotalDuration = 0.f;

		for( const FrameInfo& oFrame : _oLayer.m_oFrames )
			fTotalDuration += oFrame.m_fDuration;

		return fTotalDuration;
	}

	float Anm2::_GetLayerDurationToIndex( const LayerInfo& _oLayer, int _iFrameIndex ) const
	{
		if( _iFrameIndex >= (int)_oLayer.m_oFrames.size() )
			return _GetLayerTotalDuration( _oLayer );

		float fDuration = 0.f;

		for( int iFrame = 0; iFrame < _iFrameIndex; ++iFrame )
			fDuration += _oLayer.m_oFrames[ iFrame ].m_fDuration;

		return fDuration;
	}

	Anm2::LayerInfoVector::iterator Anm2::_FindLayer( const std::string& _sLayer )
	{
		return std::find_if( m_oLayers.begin(), m_oLayers.end(), [ _sLayer ]( const LayerInfo& oLayer ){ return oLayer.m_sName == _sLayer; } );
	}

	Anm2::LayerInfoVector::iterator Anm2::_FindSocket( const std::string& _sSocket )
	{
		return std::find_if( m_oSockets.begin(), m_oSockets.end(), [ _sSocket ]( const LayerInfo& oLayer ){ return oLayer.m_sName == _sSocket; } );
	}

	void Anm2::_ProcessTriggerContents( Trigger& _oTrigger )
	{
		if( _oTrigger.m_oContent.empty() )
			return;

		m_bIsProcessingTriggers = true;

		TriggerContentVector::iterator itContent = _oTrigger.m_oContent.begin();
		
		while( itContent != _oTrigger.m_oContent.end() )
		{
			if( itContent->m_pCallback != nullptr )
			{
				itContent->m_pCallback->Call( _oTrigger.m_sName, this );
			}
			else
				g_pFZN_AudioMgr->Sound_Play( itContent->m_sSound );

			if( itContent->m_bRemoveCallbackWhenCalled )
				itContent = _oTrigger.m_oContent.erase( itContent );
			else
				++itContent;
		}
	}

	void Anm2::_ProcessTriggersBuffers()
	{
		if( m_bReplaceTriggers )
		{
			m_oTriggers = m_oTriggersBuffer;
			m_oTriggersBuffer.clear();

			m_oAnimationStartTrigger.m_oContent = m_oAnimationStartTriggerBuffer.m_oContent;
			m_oAnimationStartTriggerBuffer.m_oContent.clear();

			m_oAnimationEndTrigger.m_oContent = m_oAnimationEndTriggerBuffer.m_oContent;
			m_oAnimationEndTriggerBuffer.m_oContent.clear();

			m_bReplaceTriggers = false;
		}

		m_bIsProcessingTriggers = false;
	}

	void Anm2::_ReplaceTriggersOnAnimationChange( const Anm2* _pAnimation )
	{
		if( m_bIsProcessingTriggers )
		{
			m_oTriggersBuffer				= _pAnimation->m_oTriggers;
			m_oAnimationStartTriggerBuffer	= _pAnimation->m_oAnimationStartTrigger;
			m_oAnimationEndTriggerBuffer	= _pAnimation->m_oAnimationEndTrigger;

			m_bReplaceTriggers = true;
		}
		else
		{
			m_oTriggers					= _pAnimation->m_oTriggers;
			m_oAnimationStartTrigger	= _pAnimation->m_oAnimationStartTrigger;
			m_oAnimationEndTrigger		= _pAnimation->m_oAnimationEndTrigger;

			m_oTriggersBuffer.clear();
			m_oAnimationStartTriggerBuffer.Reset();
			m_oAnimationEndTriggerBuffer.Reset();
		}
	}

	void Anm2::_AddContentToTriggerVector( TriggerVector& _oVector, const TriggerContent& _oContent, const std::string& _sTrigger, const float _fTime /*= -1.f*/ )
	{
		TriggerVector::iterator itTrigger = _FindTriggerInTriggerVector( _oVector, _sTrigger, _fTime );

		if( itTrigger == _oVector.end() )
		{
			if( _fTime >= 0.f )
			{
				_oVector.push_back( Trigger( _sTrigger, _fTime ) );

				_AddContentToTriggerContentVector( _oVector.back().m_oContent, _oContent );
			}
			else
				FZN_COLOR_LOG( fzn::DBG_MSG_COLORS::DBG_MSG_COL_RED, "Trigger \"%s\" not found in given vector. (\"%s\")", _sTrigger.c_str(), m_sName.c_str() );
		}
		else
			_AddContentToTriggerContentVector( ( *itTrigger ).m_oContent, _oContent );
	}

	bool Anm2::_TriggerVectorHasContent( const TriggerVector& _oVector, const TriggerContent& _oContent, const std::string& _sTrigger ) const
	{
		TriggerVector::const_iterator itTrigger = _FindTriggerInTriggerVector( _oVector, _sTrigger );

		if( itTrigger == _oVector.cend() )
			return false;

		return _TriggerContentVectorHasContent( ( *itTrigger ).m_oContent, _oContent );
	}

	void Anm2::_RemoveContentFromTriggerVector( TriggerVector& _oVector, const TriggerContent& _oContent, const std::string& _sTrigger )
	{
		TriggerVector::iterator itTrigger = _FindTriggerInTriggerVector( _oVector, _sTrigger );

		if( itTrigger == _oVector.cend() )
			return;

		_RemoveContentFromTriggerContentVector( ( *itTrigger ).m_oContent, _oContent );
	}

	Anm2::TriggerVector::iterator Anm2::_FindTriggerInTriggerVector( TriggerVector& _oVector, const std::string& _sTrigger, const float _fTime /*= -1.f */ )
	{
		if( _fTime < 0.f )
		{
			return std::find_if( _oVector.begin(), _oVector.end(), [ _sTrigger ]( const Trigger& oTrigger )
			{
				return oTrigger.m_sName == _sTrigger;
			} );
		}

		return std::find_if( _oVector.begin(), _oVector.end(), [ _sTrigger, _fTime ]( const Trigger& oTrigger )
		{
			return oTrigger.m_sName == _sTrigger && oTrigger.m_fTime == _fTime;
		} );
	}

	Anm2::TriggerVector::const_iterator Anm2::_FindTriggerInTriggerVector( const TriggerVector& _oVector, const std::string& _sTrigger, const float _fTime /*= -1.f */ )
	{
		if( _fTime < 0.f )
		{
			return std::find_if( _oVector.cbegin(), _oVector.cend(), [_sTrigger]( const Trigger& oTrigger )
			{
				return oTrigger.m_sName == _sTrigger;
			} );
		}

		return std::find_if( _oVector.cbegin(), _oVector.cend(), [_sTrigger, _fTime]( const Trigger& oTrigger )
		{
			return oTrigger.m_sName == _sTrigger && oTrigger.m_fTime == _fTime;
		} );
	}

	void Anm2::_AddContentToTriggerContentVector( TriggerContentVector& _oVector, const TriggerContent& _oContent )
	{
		if( _oContent.IsValid() == false )
			return;

		TriggerContentVector::iterator itContent = _FindContentInTriggerContentVector( _oVector, _oContent );

		if( itContent != _oVector.end() )
			return;

		_oVector.push_back( _oContent );
	}

	bool Anm2::_TriggerContentVectorHasContent( const TriggerContentVector& _oVector, const TriggerContent& _oContent ) const
	{
		if( _oContent.IsValid() == false )
			return false;

		TriggerContentVector::const_iterator itContent = _FindContentInTriggerContentVector( _oVector, _oContent );

		return itContent != _oVector.cend();
	}

	void Anm2::_RemoveContentFromTriggerContentVector( TriggerContentVector& _oVector, const TriggerContent& _oContent )
	{
		if( _oContent.IsValid() == false )
			return;

		TriggerContentVector::iterator itContent = _FindContentInTriggerContentVector( _oVector, _oContent );

		if( itContent != _oVector.cend() )
			_oVector.erase( itContent );
	}

	Anm2::TriggerContentVector::iterator Anm2::_FindContentInTriggerContentVector( TriggerContentVector& _oVector, const TriggerContent& _oContent )
	{
		return std::find_if( _oVector.begin(), _oVector.end(), [_oContent]( const TriggerContent& oContent )
		{
			return oContent.m_pCallback == _oContent.m_pCallback && oContent.m_sSound == _oContent.m_sSound;
		} );
	}

	Anm2::TriggerContentVector::const_iterator Anm2::_FindContentInTriggerContentVector( const TriggerContentVector& _oVector, const TriggerContent& _oContent )
	{
		return std::find_if( _oVector.cbegin(), _oVector.cend(), [_oContent]( const TriggerContent& oContent )
		{
			return oContent.m_pCallback == _oContent.m_pCallback && oContent.m_sSound == _oContent.m_sSound;
		} );
	}

}
