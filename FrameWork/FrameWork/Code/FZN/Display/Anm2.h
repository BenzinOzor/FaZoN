//------------------------------------------------------------------------
/// Author : Philippe OFFERMANN
/// Date : 25.09.18
/// Description : The Binding Of Isaac Rebirth animation class
//------------------------------------------------------------------------

#ifndef _ANM2_H_
#define _ANM2_H_

#include <vector>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>

#include "FZN/Display/Animation.h"
#include "FZN/Tools/Callbacks.h"

#define Anm2TriggerType( Type, Callback, Object ) new fzn::Member2DynArgCallback< Type, std::string, const fzn::Anm2* >( Callback, Object )

namespace tinyxml2
{
	class XMLElement;
}

namespace fzn
{
	class FZN_EXPORT Anm2 : public Animation
	{
	public:
		enum ChangeAnimationSettings
		{
			eKeepPosition = 1 << 0,
			eKeepRotation = 1 << 1,
			eKeepLayersTimers = 1 << 2,
			eKeepTextures = 1 << 3,
			eOverrideScaleRatio = 1 << 4,
		};

		typedef sf::Uint8 ChangeAnimationSettingsMask;

		typedef Member2DynArgCallbackBase< std::string, const Anm2* >* TriggerCallback;
		
		struct TriggerContent
		{
			TriggerContent( const TriggerContent& _oContent )
				: m_pCallback( _oContent.m_pCallback )
				, m_sSound( _oContent.m_sSound )
				, m_bRemoveCallbackWhenCalled( _oContent.m_bRemoveCallbackWhenCalled )
			{}
			TriggerContent( TriggerCallback _pCallback, bool _bRemoveCallbackWhenCalled = false )
				: m_pCallback( _pCallback )
				, m_sSound( "" )
				, m_bRemoveCallbackWhenCalled( _bRemoveCallbackWhenCalled )
			{}
			TriggerContent( const std::string& _sSound, bool _bRemoveCallbackWhenCalled = false )
				: m_pCallback( nullptr )
				, m_sSound( _sSound )
				, m_bRemoveCallbackWhenCalled( _bRemoveCallbackWhenCalled )
			{
			}

			bool IsValid() const
			{
				return m_pCallback != nullptr || m_sSound.empty() == false;
			}

			TriggerCallback	m_pCallback;
			std::string		m_sSound;
			bool			m_bRemoveCallbackWhenCalled;
		};
		typedef std::vector< TriggerContent > TriggerContentVector;

		struct Trigger
		{
			Trigger()
			{
				Reset();
			}
			Trigger( const std::string& _sName, float _fTime )
				: m_sName( _sName )
				, m_fTime( _fTime )
			{
				m_oContent.clear();
			}

			~Trigger()
			{
			}

			void Reset()
			{
				m_sName = "";
				m_fTime = -1.f;
				m_oContent.clear();
			}

			bool IsValid() const
			{
				return m_sName.empty() == false && m_fTime >= 0.f;
			}

			std::string				m_sName;
			float					m_fTime;
			TriggerContentVector	m_oContent;
		};

		struct FrameInfo
		{
			FrameInfo()
			{
				Reset();
			}

			void Reset()
			{
				m_vPosition		= sf::Vector2f( 0.f, 0.f );
				m_oFrameRect	= sf::Rect<int>( 0, 0, 0, 0 );
				m_vScale		= sf::Vector2f( 0.f, 0.f );
				m_vOrigin		= sf::Vector2f( 0.f, 0.f );
				m_oColor		= sf::Color( 255, 255, 255, 255 );
				m_oColorOverlay = sf::Color( 0, 0, 0, 0 );
				m_fRotation		= 0.f;
				m_fDuration		= 0.f;
				//m_fSpeedRatio	= 1.f;
				m_bVisible		= true;
				m_iNbFrames		= 0;
				m_bInterpolate	= false;
			}

			sf::Vector2f	m_vPosition;	//Offset position of the animation.
			sf::Rect<int>	m_oFrameRect;	//Position and size of the rectangle of the current frame on the spritesheet.
			sf::Vector2f	m_vScale;
			sf::Vector2f	m_vOrigin;
			sf::Color		m_oColor;
			sf::Color		m_oColorOverlay;
			float			m_fRotation;
			float			m_fDuration;	//Duration of the frame in ms.
			//float			m_fSpeedRatio;
			bool			m_bVisible;
			int				m_iNbFrames;
			bool			m_bInterpolate;
		};

		struct LayerInfo
		{
			LayerInfo()
				: m_iBaseTextureId( 0 )
				, m_sName( "" )
				, m_iFrameIndex( 0 )
				, m_fTimer( 0.f )
				, m_iFrameCount( 0 )
				, m_bVisible( false )
				, m_bVisibilityOverride( true )
			{}

			void Reset()
			{
				m_oFrames.clear();
				m_iBaseTextureId = 0;
				m_sName = "";
				m_iFrameIndex = 0;
				m_oClock.restart();
				m_fTimer = 0.f;
				m_iFrameCount = 0;
				m_bVisible = false;
				m_bVisibilityOverride = true;
			}

			bool IsVisible() const
			{
				if( m_bVisibilityOverride == false )
					return false;

				if( m_bVisible == false || m_oFrames.empty() )
					return false;

				return m_oFrames[ m_iFrameIndex ].m_bVisible;
			}

			sf::Vector2f GetPosition() const
			{
				return m_oSprite.getPosition();
			}

			sf::Color GetCurrentColorOverlay( float _fSpeedRatio ) const;

			std::string					m_sName;
			std::vector< FrameInfo >	m_oFrames;
			int							m_iBaseTextureId;			// Base texture index in the vector.
			sf::Sprite					m_oSprite;
			sf::Clock					m_oClock;
			float						m_fTimer;
			int							m_iFrameCount;
			int							m_iFrameIndex;
			bool						m_bVisible;					// Visibility updated by the animation.
			bool						m_bVisibilityOverride;		// Visibility set by user.
		};

		//typedef std::pair< std::string, Vector< FrameInfo > > LayerInfo;
		//The first vector contains all the layers/sockets paired with their name. The second contains all the frames.
		typedef std::vector< LayerInfo >	LayerInfoVector;
		typedef std::vector< Trigger >		TriggerVector;


		Anm2();
		Anm2( const Anm2& _oAnm );
		Anm2( tinyxml2::XMLElement* _pContent, tinyxml2::XMLElement* _pAnimation, const std::string& _sPath );
		~Anm2();

				bool				IsValid() const;

		virtual	Anm2&				operator=( const Anm2& _animation );
				bool				ChangeAnimation( const std::string& _sAnimatedObject, const std::string& _sAnimation, const ChangeAnimationSettingsMask& _uSettings = 0 );
				bool				ChangeAnimation( const Anm2* _pAnimation, const ChangeAnimationSettingsMask& _uSettings = 0 );

				void				LoadFromXMLNode( tinyxml2::XMLElement* _pContent, tinyxml2::XMLElement* _pAnimation, const std::string& _sPath );

		virtual void				Stop() override;
		virtual void				Play( bool bReset = true ) override;
				void				PlayThenPause( bool bReset = true );
				void				PlayThenStop( bool bReset = true );

		virtual void				Update() override;
		virtual void				SetPosition( sf::Vector2f _vPosition );
				void				Rotate( float _fAngleOffset );
		virtual void				SetRotation( float _fRotation );
				float				GetRotation() const;
				void				SetFrame( int _iFrameIndex, const std::string& _sLayer, bool _bAdaptAnimationToLayer = false );
				void				SetLayerVisible( const std::string& _sLayer, bool _bVisible );
				int					GetCurrentFrame( const std::string& _sLayer = "" );
		virtual void				SetScale( const sf::Vector2f& _vScale );
		virtual void				SetScaleRatio( float _fScaleRatio );
				int					GetFrameCount( const std::string& _sLayer ) const;
				sf::Vector2f		GetPosition( const std::string& _sLayerOrSocket ) const;
		virtual sf::Vector2f		GetPosition() const override;
		virtual void				SetAlpha( sf::Uint8 _uAlpha ) override;
		virtual void				SetColor( sf::Color _oColor, bool _bOverrideAlpha = false ) override;
				void				SetLayerColor( const std::string& _sLayer, const sf::Color& _oColor, bool _bOverrideAlpha = false );
		virtual void				ReplaceSpritesheet( int _iSpritesheetId, const std::string& _sSpritesheet, const std::string& _sSpritesheetPath = "", bool _bHandleError = true );
				void				SetAnimationDuration( float _fDuration );
				void				SetAnimationSpeedRatio( float _fSpeedRatio );
				float				GetAnimationDuration() const;
				bool				HasEnded() const;
				float				GetAnimationCurrentTime() const;

		//-------------------------------------------------------------------------------------------------
		/// Animation flip on the x coordinate.
		/// @param	_iFlip	: Transformation to apply (0 : inversion / 1 : Force positive scale / -1 : Force negative scale).
		//-------------------------------------------------------------------------------------------------
		virtual void				FlipX( int _iFlip = 0 ) override;
		virtual	int					GetFlipX() const override;
		
		const	LayerInfoVector&	GetLayers() const { return m_oLayers; }
		const	LayerInfoVector&	GetSockets() const { return m_oSockets; }
		const	LayerInfo*			GetLayer( const std::string& _sName ) const;
		const	LayerInfo*			GetSocket( const std::string& _sName ) const;
				sf::Color			GetLayerCurrentColorOverlay( const std::string& _sName ) const;
		
		//-------------------------------------------------------------------------------------------------
		/// Animation triggers callbacks
		//-------------------------------------------------------------------------------------------------
				void				AddTriggerCallback( const std::string& _sTrigger, TriggerCallback _pCallback, bool _bRemoveCallbackWhenCalled = false );
				void				AddTriggerCallback( int _iFrame, const std::string& _sTrigger, TriggerCallback _pCallback, bool _bRemoveCallbackWhenCalled = false );
				bool				HasAnimationCallback( const std::string& _sTrigger, const TriggerCallback _pCallback ) const;
				void				RemoveTriggerCallback( const std::string& _sTrigger, const TriggerCallback _pCallback );

				void				AddAnimationStartCallback( TriggerCallback _pCallback, bool _bRemoveCallbackWhenCalled = false );
				bool				HasAnimationStartCallback( const TriggerCallback _pCallback ) const;
				void				RemoveAnimationStartCallback( const TriggerCallback _pCallback );

				void				AddAnimationEndCallback( TriggerCallback _pCallback, bool _bRemoveCallbackWhenCalled = false );
				bool				HasAnimationEndCallback( const TriggerCallback _pCallback ) const;
				void				RemoveAnimationEndCallback( const TriggerCallback _pCallback );

		//-------------------------------------------------------------------------------------------------
		/// Animation triggers sounds
		//-------------------------------------------------------------------------------------------------
				void				AddTriggerSound( const std::string& _sTrigger, const std::string& _sSound, bool _bRemoveCallbackWhenCalled = false );
				void				AddTriggerSounds( const std::string& _sTrigger, const std::vector< std::string >& _oSound, bool _bRemoveCallbackWhenCalled = false );
				bool				HasTriggerSound( const std::string& _sTrigger, const std::string& _sSound ) const;
				void				RemoveTriggerSound( const std::string& _sTrigger, const std::string& _sSound );

				void				AddAnimationStartSound( const std::string& _sSound, bool _bRemoveCallbackWhenCalled = false );
				bool				HasAnimationStartSound( const std::string& _sSound ) const;
				void				RemoveAnimationStartSound( const std::string& _sSound );

				void				AddAnimationEndSound( const std::string& _sSound, bool _bRemoveCallbackWhenCalled = false );
				bool				HasAnimationEndSound( const std::string& _sSound ) const;
				void				RemoveAnimationEndSound( const std::string& _sSound );

				float				GetTriggerTime( const std::string& _sTrigger ) const;

		const	std::string&		GetName() const;

		static const std::string	ANIMATION_END;
		static const std::string	ANIMATION_START;

	protected :
		typedef std::pair< std::string, sf::Texture* > TextureName;
		typedef std::map< int, TextureName > BaseTextures;

		virtual void				draw( sf::RenderTarget& _oTarget, sf::RenderStates _oStates) const;

		void						_GetRootAnimationInformations( tinyxml2::XMLElement* _pAnimation );
		void						_GetLayersInformations( tinyxml2::XMLElement* _pContent, tinyxml2::XMLElement* _pAnimation, const std::string& _sPath );
		void						_GetSocketsInformations( tinyxml2::XMLElement* _pContent, tinyxml2::XMLElement* _pAnimation );
		void						_GetTriggersInformations( tinyxml2::XMLElement* _pContent, tinyxml2::XMLElement* _pAnimation );
		void						_FillFrameInformations( FrameInfo& _oFrame, tinyxml2::XMLElement* _pXMLFrame );

		void						_UpdateIndex( LayerInfo& oLayer );
		void						_ExtractFileName( std::string& _sFile );

		void						_Init();
		void						_UpdateLayerInfoVector( LayerInfoVector& _oVector, bool _bRestart );
		void						_UpdateLayerInfos( LayerInfo& _oLayer, float _fPreviousFrameDuration = -1.f );
		void						_ChangeAnimationLayerSetup( LayerInfo& _oDstLayer, const LayerInfo& _oSrcLayer );
		void						_UpdateTriggers( bool _bAnimStarting );
		void						_UpdateFlippedScale( LayerInfoVector& _oVector );
		float						_GetLayerTotalDuration( const LayerInfo& _oLayer ) const;
		float						_GetLayerDurationToIndex( const LayerInfo& _oLayer, int _iFrameIndex ) const;

		LayerInfoVector::iterator	_FindLayer( const std::string& _sLayer );
		LayerInfoVector::iterator	_FindSocket( const std::string& _sSocket );

		void						_ProcessTriggerContents( Trigger& _oTrigger );
		void						_ProcessTriggersBuffers();
		void						_ReplaceTriggersOnAnimationChange( const Anm2* _pAnimation );

		void						_AddContentToTriggerVector( TriggerVector& _oVector, const TriggerContent& _oContent, const std::string& _sTrigger, const float _fTime = -1.f );
		bool						_TriggerVectorHasContent( const TriggerVector& _oVector, const TriggerContent& _oContent, const std::string& _sTrigger ) const;
		void						_RemoveContentFromTriggerVector( TriggerVector& _oVector, const TriggerContent& _oContent, const std::string& _sTrigger );

		static TriggerVector::iterator			_FindTriggerInTriggerVector( TriggerVector& _oVector, const std::string& _sTrigger, const float _fTime = -1.f );
		static TriggerVector::const_iterator	_FindTriggerInTriggerVector( const TriggerVector& _oVector, const std::string& _sTrigger, const float _fTime = -1.f );

		void						_AddContentToTriggerContentVector( TriggerContentVector& _oVector, const TriggerContent& _oContent );
		bool						_TriggerContentVectorHasContent( const TriggerContentVector& _oVector, const TriggerContent& _oContent ) const;
		void						_RemoveContentFromTriggerContentVector( TriggerContentVector& _oVector, const TriggerContent& _oContent );

		static TriggerContentVector::iterator		_FindContentInTriggerContentVector( TriggerContentVector& _oVector, const TriggerContent& _oContent );
		static TriggerContentVector::const_iterator _FindContentInTriggerContentVector( const TriggerContentVector& _oVector, const TriggerContent& _oContent );

		FrameInfo					m_oRootAnimation;
		LayerInfoVector				m_oLayers;				//List of all the layers. Each index contains all the frames of a layer.
		LayerInfoVector				m_oSockets;				//List of all the sockets.
		TriggerVector				m_oTriggers;
		Trigger						m_oAnimationStartTrigger;
		Trigger						m_oAnimationEndTrigger;

		bool						m_bIsProcessingTriggers;
		bool						m_bReplaceTriggers;
		TriggerVector				m_oTriggersBuffer;
		Trigger						m_oAnimationStartTriggerBuffer;
		Trigger						m_oAnimationEndTriggerBuffer;

		BaseTextures				m_oBaseTextures;

		float						m_fDuration;
		float						m_fSpeedRatio;
		int							m_iFPS;
		sf::Clock					m_oClock;
		float						m_fPreviousTimer;
		std::string					m_sName;
		sf::Vector2f				m_vPosition;
		float						m_fRotation;
		bool						m_bResetAtEnd;
		sf::Vector2f				m_vFlippedScale;
		float						m_fScaleRatio;
	};
}

#endif //_ANM2_H_
