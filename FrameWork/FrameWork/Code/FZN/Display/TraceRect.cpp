//------------------------------------------------------------------------
//Auteur : Philippe OFFERMANN
//Date : 14.10.14
//Description : Rectangle drawing with the mouse
//------------------------------------------------------------------------

#include "FZN/Includes.h"
#include "FZN/Managers/WindowManager.h"
#include "FZN/Managers/InputManager.h"
#include "FZN/Display/TraceRect.h"


namespace fzn
{
	/////////////////CONSTRUCTOR / DESTRUCTOR/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Default constructor
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	TraceRect::TraceRect()
	{
		m_window = FZN_Window;
		m_bActive = FALSE;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Destructor
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	TraceRect::~TraceRect()
	{
	}

	
	/////////////////OTHER FUNCTIONS/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Begin the draw
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void TraceRect::BeginDraw()
	{
		if( !m_bActive && g_pFZN_InputMgr->IsMousePressed( sf::Mouse::Left ) )
		{
			setPosition( m_window->mapPixelToCoords( sf::Mouse::getPosition( *m_window ) ) );
			setSize( m_window->mapPixelToCoords( sf::Mouse::getPosition( *m_window ) ) - getPosition() );
			m_bActive = true;
		}
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Rectangle update (checking buttons states and rect size).
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void TraceRect::UpdateDraw()
	{
		if( m_bActive && g_pFZN_InputMgr->IsMouseDown( sf::Mouse::Left ) )
			setSize( m_window->mapPixelToCoords( sf::Mouse::getPosition( *m_window ) ) - getPosition() );
		else if( m_bActive && g_pFZN_InputMgr->IsMouseUp( sf::Mouse::Left ) )
		{
			m_bActive = false;
			setPosition( -1, -1 );
			setSize( sf::Vector2f( 0.f, 0.f ) );
		}
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Rectangle display (Use g_pFZN_WindowMgr->draw())
	//Parameter 1 : Window in which the rectangle will be displayed
	//Paramètre 2 : Rendering state
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void TraceRect::Draw(sf::RenderWindow& _window, sf::RenderStates _state) const
	{
		if(m_bActive)
			m_window->draw((RectangleShape)*this, _state);
	}

	/////////////////ACCESSORS / MUTATORS/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on the boolean bActive
	//Return value : Active or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	INT8 TraceRect::GetActive()
	{
		return m_bActive;
	}
} //namespace fzn
