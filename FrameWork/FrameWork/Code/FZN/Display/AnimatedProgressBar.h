#pragma once

#include "FZN/Display/Anm2.h"
#include "FZN/Display/ProgressBar.h"


namespace fzn
{
	class FZN_EXPORT AnimatedProgressBar : public Anm2
	{
	public:
		AnimatedProgressBar();
		AnimatedProgressBar( const Anm2& _oAnimation );
		AnimatedProgressBar( const AnimatedProgressBar& _oProgressBar );
		~AnimatedProgressBar();

				bool					IsValid() const;

		virtual	AnimatedProgressBar&	operator=( const Anm2& _oAnimation ) override;
				AnimatedProgressBar&	operator=( const AnimatedProgressBar& _oProgressBar );

		virtual void					Update() override;

				void					SetGaugeSprites( const std::string& _sGaugeLayer, const std::string& _sDeltaLayer = "" );

				void					SetMaxValue( float _fValue );
				void					SetCurrentValue( float _fValue, bool _bForceUpdate = false );
				void					DisplayDelta( bool _bDisplay );
				void					SetDeltaSpeed( float _fDelay );
				void					SetDecreaseDelay( float _fSpeed );
				void					SetDirection( const ProgressBar::Direction& _eDirection );

	protected:
				void					_SetCurrentValueSprites();
				bool					_IsProgressingHorizontally() const;

		LayerInfo*						m_pGaugeLayer;
		LayerInfo*						m_pDeltaLayer;
		sf::IntRect						m_oMaxGaugeSize;

		float							m_fMaxValue;			//Maximum value displayed by the bar
		float							m_fCurrentValue;		//Current value displayed by the bar
		float							m_fDeltaMaxValue;		//Moving point of the delta shape

		float							m_fTimer;				//Timer used for the update of the delta shape
		bool							m_bDecreaseDelta;		//Indicates if the delta shape has to be updated
		bool							m_bWaitingForDelay;		//Indicates if the time is beeing counted before the decrease starts
		float							m_fDeltaSpeed;			//Speed at which the delta shape decreases
		float							m_fDecreaseDelay;		//Delay before the delta bar starts to decrease
		bool							m_bDisplayDelta;		//Indicates if the delta shape has to be displayed or not

		ProgressBar::Direction			m_eDirection;
	};
}