//------------------------------------------------------------------------
//Author : Philippe OFFERMANN
//Date : 10.05.15
//Description : Progress bar that can be used for life or loading bars
//------------------------------------------------------------------------

#include "FZN/Includes.h"
#include "FZN/Managers/WindowManager.h"
#include "FZN/Display/ProgressBar.h"


namespace fzn
{
	/////////////////CONSTRUCTOR / DESTRUCTOR/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Default constructor
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	ProgressBar::ProgressBar()
	: m_vPosition( 0.f, 0.f )
	, m_oForegroundColor( sf::Color::Red )
	, m_oDeltaColor( sf::Color::Yellow )
	, m_fCurrentValue( 0.f )
	, m_fDeltaMaxValue( 0.f )
	, m_fMaxValue( 0.f )
	, m_fTimer( 0.f )
	, m_bDisplayDelta( true )
	, m_bDecreaseDelta( false )
	, m_bWaitingForDelay( false )
	, m_fDeltaSpeed( 1.f )
	, m_fDecreaseDelay( 1.f )
	, m_bVisible( true )
	, m_bUseSprites( false )
	, m_eDirection( Direction::eLeftToRight )
	{
		m_background.setFillColor( sf::Color( 50, 50, 50 ) );
		m_background.setOutlineThickness( 2.f );
		m_background.setOutlineColor( sf::Color( 200, 200, 200 ) );

		m_foreground.setFillColor( m_oForegroundColor );

		m_delta.setFillColor( m_oDeltaColor );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Destructor
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	ProgressBar::~ProgressBar()
	{
	}


	//=========================================================
	//==========================PUBLIC==========================
	//=========================================================

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Updates the progress bar by check the timer for the delta shape
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void ProgressBar::Update()
	{
		if( g_pFZN_Core->IsModuleCreated( FazonCore::WindowModule ) )
		{
			if( m_bUseSprites )
				_UpdateSprites();
			else
				_UpdateShapes();
		}
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Mutator on the position of the bar
	//Parameter : Wanted position
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void ProgressBar::SetPosition( const sf::Vector2f& _vPosition )
	{
		sf::Vector2 vGaugeDelta = m_oGaugeSprite.getPosition() - m_vPosition;
		m_oGaugeSprite.setPosition( _vPosition + vGaugeDelta );

		vGaugeDelta = m_oDeltaSprite.getPosition() - m_vPosition;
		m_oDeltaSprite.setPosition( _vPosition + vGaugeDelta );

		m_vPosition = _vPosition;
		m_oBackgroundSprite.setPosition( m_vPosition );
		m_oFrontPlate.setPosition( m_vPosition );

		m_background.setPosition( m_vPosition );
		m_foreground.setPosition( m_vPosition );
		m_delta.setPosition( m_vPosition );
	}

	sf::Vector2f ProgressBar::GetPosition() const
	{
		return m_vPosition;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Mutator on the size of the bar
	//Parameter : Wanted size
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void ProgressBar::SetSize( const sf::Vector2f& _size )
	{
		m_background.setSize( _size );

		float foregroundSize = m_fCurrentValue / m_fMaxValue;
		float deltaSize = m_fDeltaMaxValue / m_fMaxValue;
		foregroundSize *= _size.x;
		deltaSize *= _size.x;

		m_foreground.setSize( sf::Vector2f( foregroundSize, _size.y ) );
		m_delta.setSize( sf::Vector2f( deltaSize, _size.y ) );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Mutator on the current value of the bar
	//Parameter : Wanted value (>= 0)
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void ProgressBar::SetCurrentValue( float _value )
	{
		if( _value < 0.f || _value == m_fCurrentValue )
			return;

		m_fCurrentValue = _value;

		if( m_bUseSprites )
			_SetCurrentValueSprites();
		else
			_SetCurrentValueShapes();
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Mutator on the maximum value of the bar
	//Parameter : Wanted value (> 1)
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void ProgressBar::SetMaxValue( float _value )
	{
		if( _value < 1.f )
			return;

		m_fMaxValue = _value;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on the current value of the bar
	//Return value : Current value
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	float ProgressBar::GetCurrentValue()
	{
		return m_fCurrentValue;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on the maximum value of the bar
	//Return value : Maximum value
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	float ProgressBar::GetMaxValue()
	{
		return m_fMaxValue;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on the current speed of the delta shape
	//Return value : Current speed
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	float ProgressBar::GetDeltaSpeed()
	{
		return m_fDeltaSpeed;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Mutator on the speed of the delta shape
	//Parameter : Wanted speed (> 0)
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void ProgressBar::SetDeltaSpeed( float _speed )
	{
		if( _speed > 0.f )
			m_fDeltaSpeed = _speed;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on the delay before the delta shape starts to decrease
	//Return value : Decrease delay
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	float ProgressBar::GetDecreaseDelay()
	{
		return m_fDecreaseDelay;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on the delay before the delta shape starts to decrease
	//Parameter : Decrease delay (>= 0)
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void ProgressBar::SetDecreaseDelay( float _delay )
	{
		if( _delay >= 0.0f )
			m_fDecreaseDelay = _delay;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on the background shape
	//Parameter 1 : Background of the bar
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	sf::RectangleShape& ProgressBar::GetBackground()
	{
		return m_background;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on the foreground shape
	//Parameter 1 : Foreground of the bar
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	sf::RectangleShape& ProgressBar::GetForeground()
	{
		return m_foreground;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on the delta shape
	//Parameter 1 : Delta of the bar
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	sf::RectangleShape& ProgressBar::GetDelta()
	{
		return m_delta;
	}

	void ProgressBar::SetBackgroundTexture( const sf::Texture* _pTexture, const sf::Vector2f& _vOrigin /*= { 0.f, 0.f }*/, const sf::IntRect& _oTextureRect /*= sf::IntRect( 0, 0, -1, -1 ) */, const sf::Color _oColor /*= sf::Color::White*/ )
	{
		if( _pTexture == nullptr )
			return;

		m_oBackgroundSprite.setTexture( *_pTexture );

		if( _oTextureRect.width > 0 && _oTextureRect.height > 0 )
			m_oBackgroundSprite.setTextureRect( _oTextureRect );

		m_oBackgroundSprite.setOrigin( _vOrigin );
		m_oBackgroundSprite.setColor( _oColor );

		if( m_oBackgroundSprite.getTexture() != nullptr && m_oDeltaSprite.getTexture() != nullptr && m_oGaugeSprite.getTexture() != nullptr )
			m_bUseSprites = true;
	}

	void ProgressBar::SetGaugeTexture( const sf::Texture* _pTexture, const sf::Vector2f& _vOrigin /*= { 0.f, 0.f }*/, const sf::IntRect& _oTextureRect /*= sf::IntRect( 0, 0, -1, -1 ) */, const sf::Color _oColor /*= sf::Color::White*/ )
	{
		if( _pTexture == nullptr )
			return;

		m_oGaugeSprite.setTexture( *_pTexture );
		m_oDeltaSprite.setTexture( *_pTexture );

		if( _oTextureRect.width > 0 && _oTextureRect.height > 0 )
		{
			m_oGaugeSprite.setTextureRect( _oTextureRect );
			m_oDeltaSprite.setTextureRect( _oTextureRect );
			m_oMaxGaugeSize = _oTextureRect;
		}
		else
			m_oMaxGaugeSize = sf::IntRect( 0, 0, (int)_pTexture->getSize().x, (int)_pTexture->getSize().y );

		m_oGaugeSprite.setOrigin( _vOrigin );
		m_oDeltaSprite.setOrigin( _vOrigin );

		m_oGaugeSprite.setColor( _oColor );

		if( m_oBackgroundSprite.getTexture() != nullptr && m_oDeltaSprite.getTexture() != nullptr && m_oGaugeSprite.getTexture() != nullptr )
			m_bUseSprites = true;
	}

	void ProgressBar::SetFrontPlateTexture( const sf::Texture* _pTexture, const sf::Vector2f& _vOrigin /*= { 0.f, 0.f }*/, const sf::IntRect& _oTextureRect /*= sf::IntRect( 0, 0, -1, -1 )*/, const sf::Color _oColor /*= sf::Color::White */ )
	{
		if( _pTexture == nullptr )
			return;

		m_oFrontPlate.setTexture( *_pTexture );

		if( _oTextureRect.width > 0 && _oTextureRect.height > 0 )
			m_oFrontPlate.setTextureRect( _oTextureRect );

		m_oFrontPlate.setOrigin( _vOrigin );
		m_oFrontPlate.setColor( _oColor );

		if( m_oBackgroundSprite.getTexture() != nullptr && m_oDeltaSprite.getTexture() != nullptr && m_oGaugeSprite.getTexture() != nullptr )
			m_bUseSprites = true;
	}

	void ProgressBar::SetDeltaColor( const sf::Color _oColor )
	{
		m_oDeltaSprite.setColor( _oColor );
	}

	void ProgressBar::SetVisible( bool _bVisible )
	{
		m_bVisible = _bVisible;
	}

	bool ProgressBar::IsVisible() const
	{
		return m_bVisible;
	}

	void ProgressBar::SetDirection( const Direction& _eDirection )
	{
		m_eDirection = _eDirection;
	}

	fzn::ProgressBar::Direction ProgressBar::GetDirection() const
	{
		return m_eDirection;
	}

	void ProgressBar::draw( sf::RenderTarget& _oTarget, sf::RenderStates _oStates ) const
	{
		if( m_bVisible == false )
			return;
		
		if( m_bUseSprites )
		{
			_oTarget.draw( m_oBackgroundSprite );

			if( m_bDisplayDelta )
				_oTarget.draw( m_oDeltaSprite );

			_oTarget.draw( m_oGaugeSprite );

			if( m_oFrontPlate.getTexture() != nullptr )
				_oTarget.draw( m_oFrontPlate );

			return;
		}

		_oTarget.draw( m_background,	_oStates );

		if( m_bDisplayDelta )
			_oTarget.draw( m_delta,			_oStates );

		_oTarget.draw( m_foreground,	_oStates );
	}

	void ProgressBar::_UpdateShapes()
	{
		if( m_bDecreaseDelta )
		{
			sf::Vector2f deltaSize = m_delta.getSize();

			if( deltaSize.x >= m_foreground.getSize().x )
			{
				deltaSize.x -= FrameTime * 40.f * m_fDeltaSpeed;
				m_delta.setSize( deltaSize );
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

	void ProgressBar::_UpdateSprites()
	{
		if( m_bDecreaseDelta )
		{
			sf::IntRect oDeltaRect = m_oDeltaSprite.getTextureRect();

			if( oDeltaRect.width >= m_oGaugeSprite.getTextureRect().width )
			{
				oDeltaRect.width -= (int)ceil( FrameTime * 40.f * m_fDeltaSpeed );
				m_oDeltaSprite.setTextureRect( oDeltaRect );
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

	void ProgressBar::_SetCurrentValueShapes()
	{
		float deltaX = m_foreground.getSize().x;

		sf::Vector2f fMaxSize = m_background.getSize();
		float foregroundSize = m_fCurrentValue / m_fMaxValue;
		
		foregroundSize *= fMaxSize.x;
		m_foreground.setSize( sf::Vector2f( foregroundSize, fMaxSize.y ) );

		if( m_bDisplayDelta == false || m_fCurrentValue == m_fMaxValue )
			return;

		if( !m_bDecreaseDelta && !m_bWaitingForDelay )
		{
			m_bWaitingForDelay = true;
			m_delta.setSize( sf::Vector2f( deltaX, fMaxSize.y ) );
		}
	}

	void ProgressBar::_SetCurrentValueSprites()
	{
		sf::IntRect oForegroundRect = m_oGaugeSprite.getTextureRect();
		sf::IntRect oDeltaRect = oForegroundRect;

		if( _IsProgressingHorizontally() )
		{
			oForegroundRect.width = (int)( m_fCurrentValue / m_fMaxValue * m_oMaxGaugeSize.width );

			if( m_eDirection == Direction::eRightToLeft )
			{
				oForegroundRect.left = m_oMaxGaugeSize.width - oForegroundRect.width;
				m_oGaugeSprite.setPosition( m_vPosition + sf::Vector2f( 0.f, m_oMaxGaugeSize.width - oForegroundRect.width ) );
			}
		}
		else
		{
			oForegroundRect.height = (int)( m_fCurrentValue / m_fMaxValue * m_oMaxGaugeSize.height );

			if( m_eDirection == Direction::eBottomToTop )
			{
				oForegroundRect.top = ( m_oMaxGaugeSize.height - oForegroundRect.height ) + m_oMaxGaugeSize.top;
				m_oGaugeSprite.setPosition( m_vPosition + sf::Vector2f( 0.f, m_oMaxGaugeSize.height - oForegroundRect.height ) );
			}
		}

		m_oGaugeSprite.setTextureRect( oForegroundRect );

		if( m_bDisplayDelta == false || m_fCurrentValue == m_fMaxValue )
			return;

		if( !m_bDecreaseDelta && !m_bWaitingForDelay )
		{
			m_bWaitingForDelay = true;
			m_oDeltaSprite.setTextureRect( oDeltaRect );
		}
	}

	bool ProgressBar::_IsProgressingHorizontally() const
	{
		return m_eDirection <= Direction::eRightToLeft;
	}

} //namespace fzn
