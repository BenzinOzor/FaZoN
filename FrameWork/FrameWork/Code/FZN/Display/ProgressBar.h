//------------------------------------------------------------------------
//Author : Philippe OFFERMANN
//Date : 10.05.15
//Description : Progress bar that can be used for life or loading bars
//------------------------------------------------------------------------

#ifndef _PROGRESSBAR_H_
#define _PROGRESSBAR_H_

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>


namespace fzn
{
	class FZN_EXPORT ProgressBar : public sf::Drawable
	{
	public:
		enum Direction
		{
			eLeftToRight,
			eRightToLeft,
			eTopToBottom,
			eBottomToTop,
			eNbDirections,
		};

		/////////////////CONSTRUCTOR / DESTRUCTOR/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Default constructor
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		ProgressBar();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Destructor
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		~ProgressBar();


		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Updates the progress bar by check the timer for the delta shape
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void Update();

		/////////////////ACCESSORS / MUTATORS/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Mutator on the position of the bar
		//Parameter : Wanted position
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void SetPosition( const sf::Vector2f& _vPosition );
		sf::Vector2f GetPosition() const;
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Mutator on the size of the bar
		//Parameter : Wanted size
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void SetSize(const sf::Vector2f& _size);
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Mutator on the current value of the bar
		//Parameter : Wanted value (>= 0)
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void SetCurrentValue(float _value);
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Mutator on the maximum value of the bar
		//Parameter : Wanted value (> 1)
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void SetMaxValue(float _value);
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on the current value of the bar
		//Return value : Current value
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		float GetCurrentValue();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on the maximum value of the bar
		//Return value : Maximum value
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		float GetMaxValue();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on the current speed of the delta shape
		//Return value : Current speed
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		float GetDeltaSpeed();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Mutator on the speed of the delta shape
		//Parameter : Wanted speed (> 0)
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void SetDeltaSpeed(float _delay);
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on the delay before the delta shape starts to decrease
		//Return value : Decrease delay
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		float GetDecreaseDelay();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on the delay before the delta shape starts to decrease
		//Parameter : Decrease delay (>= 0)
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void SetDecreaseDelay(float _speed);
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on the background shape
		//Parameter 1 : Background of the bar
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		sf::RectangleShape& GetBackground();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on the foreground shape
		//Parameter 1 : Foreground of the bar
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		sf::RectangleShape& GetForeground();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on the delta shape
		//Parameter 1 : Delta of the bar
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		sf::RectangleShape& GetDelta();
		
		void SetBackgroundTexture(	const sf::Texture* _pTexture, const sf::Vector2f& _vOrigin = { 0.f, 0.f }, const sf::IntRect& _oTextureRect = sf::IntRect( 0, 0, -1, -1 ), const sf::Color _oColor = sf::Color::White );
		void SetGaugeTexture(		const sf::Texture* _pTexture, const sf::Vector2f& _vOrigin = { 0.f, 0.f }, const sf::IntRect& _oTextureRect = sf::IntRect( 0, 0, -1, -1 ), const sf::Color _oColor = sf::Color::White );
		void SetFrontPlateTexture(	const sf::Texture* _pTexture, const sf::Vector2f& _vOrigin = { 0.f, 0.f }, const sf::IntRect& _oTextureRect = sf::IntRect( 0, 0, -1, -1 ), const sf::Color _oColor = sf::Color::White );
		void SetDeltaColor( const sf::Color _oColor );

		void SetVisible( bool _bVisible );
		bool IsVisible() const;

		void		SetDirection( const Direction& _eDirection );
		Direction	GetDirection() const;

		/////////////////MEMBER VARIABLES/////////////////

		bool m_bDisplayDelta;					//Indicates if the delta shape has to be displayed or not

	private :
		virtual void draw( sf::RenderTarget& _oTarget, sf::RenderStates _oStates ) const;
		
		void _UpdateShapes();
		void _UpdateSprites();

		void _SetCurrentValueShapes();
		void _SetCurrentValueSprites();

		bool _IsProgressingHorizontally() const;


		/////////////////MEMBER VARIABLES/////////////////

		sf::Vector2f		m_vPosition;

		sf::RectangleShape	m_background;			//Background shape (also used for the outline of the bar)
		sf::RectangleShape	m_foreground;			//Foreground shape (moving with the current value)
		sf::RectangleShape	m_delta;				//Delta between two values of the foreground shape
		
		sf::Sprite			m_oBackgroundSprite;
		sf::Sprite			m_oGaugeSprite;
		sf::Sprite			m_oFrontPlate;
		sf::Sprite			m_oDeltaSprite;
		sf::IntRect			m_oMaxGaugeSize;
		
		sf::Color			m_oForegroundColor;
		sf::Color			m_oDeltaColor;

		float				m_fMaxValue;			//Maximum value displayed by the bar
		float				m_fCurrentValue;		//Current value displayed by the bar
		float				m_fDeltaMaxValue;		//Moving point of the delta shape

		float				m_fTimer;				//Timer used for the update of the delta shape
		bool				m_bDecreaseDelta;		//Indicates if the delta shape has to be updated
		bool				m_bWaitingForDelay;		//Indicates if the time is beeing counted before the decrease starts
		float				m_fDeltaSpeed;			//Speed at which the delta shape decreases
		float				m_fDecreaseDelay;		//Delay before the delta bar starts to decrease

		bool				m_bVisible;
		bool				m_bUseSprites;

		Direction			m_eDirection;
	};
} //namespace fzn

#endif //_PROGRESSBAR_H_
