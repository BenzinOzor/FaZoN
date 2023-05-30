#include "FZN/Display/AnimatedProgressBar.h"
#include "FZN/Managers/WindowManager.h"


namespace fzn
{
	AnimatedProgressBar::AnimatedProgressBar()
		: Anm2()
		, m_pGaugeLayer( nullptr )
		, m_pDeltaLayer( nullptr )
		, m_fCurrentValue( 0.f )
		, m_fDeltaMaxValue( 0.f )
		, m_fMaxValue( 0.f )
		, m_fTimer( 0.f )
		, m_bDecreaseDelta( false )
		, m_bWaitingForDelay( false )
		, m_fDeltaSpeed( 1.f )
		, m_fDecreaseDelay( 1.f )
		, m_bDisplayDelta( true )
		, m_eDirection( ProgressBar::Direction::eLeftToRight )
	{
	}

	AnimatedProgressBar::AnimatedProgressBar( const Anm2& _oAnimation )
		: AnimatedProgressBar()
	{
		Anm2::operator=( _oAnimation );
	}

	AnimatedProgressBar::AnimatedProgressBar( const AnimatedProgressBar& _oProgressBar )
	{
		AnimatedProgressBar::operator=( _oProgressBar );
	}

	AnimatedProgressBar::~AnimatedProgressBar()
	{
	}

	bool AnimatedProgressBar::IsValid() const
	{
		return m_pGaugeLayer != nullptr;
	}

	void AnimatedProgressBar::Update()
	{
		Anm2::Update();

		if( m_eState == Animation::Stopped && m_pGaugeLayer != nullptr && m_pDeltaLayer != nullptr )
		{
			if( m_bDecreaseDelta )
			{
				sf::IntRect oDeltaRect = m_pDeltaLayer->m_oSprite.getTextureRect();

				if( oDeltaRect.width >= m_pGaugeLayer->m_oSprite.getTextureRect().width )
				{
					oDeltaRect.width -= (int)ceil( FrameTime * 40.f * m_fDeltaSpeed );
					m_pDeltaLayer->m_oSprite.setTextureRect( oDeltaRect );
				}
				else m_bDecreaseDelta = false;
			}
			else if( m_bWaitingForDelay )
			{
				m_fTimer += FrameTime;
				if( m_fTimer >= m_fDecreaseDelay )
				{
					m_bDecreaseDelta = true;
					m_bWaitingForDelay = false;
					m_fTimer = 0.f;
				}
			}
		}

		if( m_eState == Animation::Playing )
			_SetCurrentValueSprites();
	}

	AnimatedProgressBar& AnimatedProgressBar::operator=( const Anm2& _oAnimation )
	{
		Anm2::operator=( _oAnimation );

		return *this;
	}

	AnimatedProgressBar& AnimatedProgressBar::operator=( const AnimatedProgressBar& _oProgressBar )
	{
		Anm2::operator=( _oProgressBar );

		m_pGaugeLayer		= _oProgressBar.m_pGaugeLayer;
		m_pDeltaLayer		= _oProgressBar.m_pDeltaLayer;
		m_oMaxGaugeSize		= _oProgressBar.m_oMaxGaugeSize;

		m_fMaxValue			= _oProgressBar.m_fMaxValue;
		m_fCurrentValue		= _oProgressBar.m_fCurrentValue;
		m_fDeltaMaxValue	= _oProgressBar.m_fDeltaMaxValue;

		m_fTimer			= _oProgressBar.m_fTimer;
		m_bDecreaseDelta	= _oProgressBar.m_bDecreaseDelta;
		m_bWaitingForDelay	= _oProgressBar.m_bWaitingForDelay;
		m_fDeltaSpeed		= _oProgressBar.m_fDeltaSpeed;
		m_fDecreaseDelay	= _oProgressBar.m_fDecreaseDelay;

		m_eDirection		= _oProgressBar.m_eDirection;

		return *this;
	}

	void AnimatedProgressBar::SetGaugeSprites( const std::string& _sGaugeLayer, const std::string& _sDeltaLayer /*= ""*/ )
	{
		LayerInfoVector::iterator it = _FindLayer( _sGaugeLayer );

		if( it != m_oLayers.end() && it->m_oFrames.empty() == false )
		{
			m_pGaugeLayer = it._Ptr;
			m_oMaxGaugeSize = m_pGaugeLayer->m_oFrames[ 0 ].m_oFrameRect;
		}

		if( _sDeltaLayer.empty() )
			return;

		it = _FindLayer( _sDeltaLayer );

		if( it != m_oLayers.end() && it->m_oFrames.empty() == false )
			m_pDeltaLayer = it._Ptr;
	}

	void AnimatedProgressBar::SetMaxValue( float _fValue )
	{
		m_fMaxValue = _fValue;
	}

	void AnimatedProgressBar::SetCurrentValue( float _fValue, bool _bForceUpdate /*= false*/ )
	{
		if( _fValue < 0.f || _fValue == m_fCurrentValue && _bForceUpdate == false )
			return;

		m_fCurrentValue = _fValue;

		_SetCurrentValueSprites();
	}

	void AnimatedProgressBar::DisplayDelta( bool _bDisplay )
	{
		m_bDisplayDelta = _bDisplay;
	}

	void AnimatedProgressBar::SetDeltaSpeed( float _fSpeed )
	{
		if( _fSpeed > 0.f )
			m_fDeltaSpeed = _fSpeed;
	}

	void AnimatedProgressBar::SetDecreaseDelay( float _fDelay )
	{
		if( _fDelay >= 0.f )
			m_fDecreaseDelay = _fDelay;
	}

	void AnimatedProgressBar::SetDirection( const ProgressBar::Direction & _eDirection )
	{
		m_eDirection = _eDirection;
	}

	float AnimatedProgressBar::GetCurrentValue() const
	{
		return m_fCurrentValue;
	}

	float AnimatedProgressBar::GetMaxValue() const
	{
		return m_fMaxValue;
	}

	void AnimatedProgressBar::_SetCurrentValueSprites()
	{
		if( m_pGaugeLayer == nullptr )
			return;

		sf::IntRect oForegroundRect = m_pGaugeLayer->m_oSprite.getTextureRect();
		sf::IntRect oDeltaRect = oForegroundRect;

		if( _IsProgressingHorizontally() )
		{
			oForegroundRect.width = (int)( m_fCurrentValue / m_fMaxValue * m_oMaxGaugeSize.width );

			if( m_eDirection == ProgressBar::Direction::eRightToLeft )
			{
				oForegroundRect.left = m_oMaxGaugeSize.width - oForegroundRect.width;
				m_pGaugeLayer->m_oSprite.setPosition( m_vPosition + sf::Vector2f( 0.f, (float)( m_oMaxGaugeSize.width - oForegroundRect.width ) ) );
			}
		}
		else
		{
			oForegroundRect.height = (int)( m_fCurrentValue / m_fMaxValue * m_oMaxGaugeSize.height );

			if( m_eDirection == ProgressBar::Direction::eBottomToTop )
			{
				oForegroundRect.top = ( m_oMaxGaugeSize.height - oForegroundRect.height ) + m_oMaxGaugeSize.top;
				m_pGaugeLayer->m_oSprite.setPosition( m_vPosition + sf::Vector2f( 0.f, (float)( m_oMaxGaugeSize.height - oForegroundRect.height ) ) );
			}
		}

		m_pGaugeLayer->m_oSprite.setTextureRect( oForegroundRect );

		if( m_bDisplayDelta == false || m_pDeltaLayer == nullptr || m_fCurrentValue == m_fMaxValue )
			return;

		if( !m_bDecreaseDelta && !m_bWaitingForDelay )
		{
			m_bWaitingForDelay = true;
			m_pDeltaLayer->m_oSprite.setTextureRect( oDeltaRect );
		}
	}

	bool AnimatedProgressBar::_IsProgressingHorizontally() const
	{
		return m_eDirection <= ProgressBar::Direction::eRightToLeft;
	}
}
