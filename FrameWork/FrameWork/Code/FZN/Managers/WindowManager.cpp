//------------------------------------------------------------------------
//Author : Philippe OFFERMANN
//Date : 03.10.14
//Description : Graphical elements management
//------------------------------------------------------------------------

#include <Externals/ImGui/imgui.h>
#include <Externals/ImGui/imgui-SFML.h>

#include "FZN/Includes.h"
#include "FZN/Managers/InputManager.h"
#include "FZN/Display/Animation.h"
#include "FZN/Display/Anm2.h"
#include "FZN/Display/ProgressBar.h"
#include "FZN/DataStructure/Vector.h"
#include "FZN/Managers/DataManager.h"
#include "FZN/Managers/WindowManager.h"
#include "FZN/Tools/Tools.h"

#include <iostream>
#include <filesystem>
#include <sstream>


FZN_EXPORT fzn::WindowManager* g_pFZN_WindowMgr = nullptr;

namespace fzn
{
	//=========================================================
	///======================CUSTOMWINDOW======================
	//=========================================================

	CustomWindow::CustomWindow( int _iIndex, int _width, int _height, sf::Uint32 _style /*= sf::Style::Close*/, sf::String _title /*= "FaZoN"*/, const sf::ContextSettings& settings /*= sf::ContextSettings()*/ )
	: m_pWindow( nullptr )
	, m_iIndex( _iIndex )
	, m_iWidth( _width > 0 ? _width : 1280 )
	, m_iHeight( _height > 0 ? _height : 720 )
	, m_fFrameTime( 0.f )
	, m_fFramerate( 0 )
	, m_iDesiredFPS( 0 )
	, m_iLastTick( 0 )
	, m_bClipCursor( false )
	, m_bCheckInputEvents( false )
	, m_bMustClose( false )
	, m_oClearColor( sf::Color( 0, 0, 0 ) )
	, m_uStyle( _style )
	, m_sTitle( _title )
	, m_oContext( settings )
	{
		m_oClock.restart();

		m_pWindow = new sf::RenderWindow( sf::VideoMode( m_iWidth, m_iHeight ), m_sTitle, m_uStyle, m_oContext );
	}

	void CustomWindow::Update()
	{
		const bool bIsMainWindow = m_iIndex == FZN_MainWindow;
		int iNbCallBacks = 0;
		while( m_pWindow->pollEvent( m_oEvent ) )
		{
			if( bIsMainWindow )
				ImGui::SFML::ProcessEvent( m_oEvent );

			if( m_oEvent.type == sf::Event::Closed )
			{
				m_pWindow->close();
				return;
			}

			if( g_pFZN_InputMgr != nullptr )
				g_pFZN_InputMgr->ManageInputEvent( m_oEvent );

			ProcessEventsCallBacks();
		}

		//Clear();
		m_pWindow->clear( m_oClearColor );

		if( bIsMainWindow )
		{
			g_pFZN_Core->Update();
			const float fFrameTime = m_fFrameTime > 0.f ? m_fFrameTime : 0.016f;
			ImGui::SFML::Update( *m_pWindow, sf::seconds( fFrameTime ) );
		}

		m_oCallbacksHolder.ExecuteCallbacks( DataCallbackType::Update );
		m_oCallbacksHolder.ExecuteCallbacks( DataCallbackType::Display );

		ImGui::SFML::Render( *m_pWindow );

		/*std::ostringstream sFramerate;
		sFramerate << "Frame time : " << m_fFrameTime << " s \nFramerate : " << m_fFramerate << " fps";
		Tools::DrawString( sFramerate.str().c_str(), sf::Vector2f( 0.f, 0.f ), 20, sf::Color::White, m_iIndex );*/

		m_pWindow->display();

		ProcessFramerate();
	}

	void CustomWindow::ProcessFramerate()
	{
		sf::Int64 iTick = g_pFZN_Core->GetGlobalTime().asMicroseconds();
		
		if( m_iDesiredFPS > 0 )
		{
			sf::Int64 iDelta					= iTick - m_iLastTick;
			const sf::Int64 iTargetFrameTime	= 1000000 / m_iDesiredFPS;
			const float fSleepTime				= static_cast<float>( iTargetFrameTime - iDelta );

			if( fSleepTime > 0 )
			{
				timeBeginPeriod( g_pFZN_WindowMgr->m_uMinSupportedPeriod );

				Sleep( static_cast<DWORD>( fSleepTime * 0.001f ) );

				timeEndPeriod( g_pFZN_WindowMgr->m_uMinSupportedPeriod );

				iTick = g_pFZN_Core->GetGlobalTime().asMicroseconds();
			}
		}

		m_fFrameTime	= ( iTick - m_iLastTick ) * 0.000001f;
		m_fFramerate	= 1.f / m_fFrameTime;
		m_iLastTick		= iTick;
	}

	bool CustomWindow::ClockFramerate()
	{
		if( m_iDesiredFPS == 0 )
		{
			sf::Int64 iTick = g_pFZN_Core->GetGlobalTime().asMicroseconds();
			m_fFrameTime	= ( iTick - m_iLastTick ) * 0.000001f;
			m_fFramerate	= 1.f / m_fFrameTime;
			m_iLastTick		= iTick;
			return true;
		}

		if( m_oClock.getElapsedTime().asMicroseconds() < 1000000 / m_iDesiredFPS )
			return false;

		m_oClock.restart();

		sf::Int64 iTick = g_pFZN_Core->GetGlobalTime().asMicroseconds();
		m_fFrameTime	= ( iTick - m_iLastTick ) * 0.000001f;
		m_fFramerate	= 1.f / m_fFrameTime;
		m_iLastTick		= iTick;

		return true;
	}

	void CustomWindow::ProcessEventsCallBacks()
	{
		m_oCallbacksHolder.ExecuteCallbacks( DataCallbackType::Event );
	}


	void CustomWindow::SetSize( const sf::Vector2u& _vSize )
	{
		m_iWidth = _vSize.x;
		m_iHeight = _vSize.y;

		sf::View view = m_pWindow->getDefaultView();
		view.reset( sf::FloatRect( 0.f, 0.f, (float)m_iWidth, (float)m_iHeight ) );
		m_pWindow->setView( view );
		m_pWindow->setSize( _vSize );
	}

	void CustomWindow::SetTitle( const std::string& _sTitle )
	{
		m_sTitle = _sTitle;
		m_pWindow->setTitle( m_sTitle );
	}

	bool CustomWindow::IsFullScreen() const
	{
		return ( m_uStyle & sf::Style::Fullscreen ) != 0;
	}

	void CustomWindow::ToggleFullScreen( bool _bFullScreen )
	{
		unsigned int uWidth = 0;
		unsigned int uHeight = 0;

   		if( _bFullScreen )
		{
			m_uStyle |= sf::Style::Fullscreen;

			RECT desktop;
			// Get a handle to the desktop window
			const HWND hDesktop = GetDesktopWindow();
			// Get the size of screen to the variable desktop
			GetWindowRect( hDesktop, &desktop );
			// The top left corner will have coordinates (0,0)
			// and the bottom right corner will have coordinates
			// (horizontal, vertical)
			uWidth = desktop.right;
			uHeight = desktop.bottom;
		}
		else
		{
			m_uStyle &= ~sf::Style::Fullscreen;

			uWidth = m_iWidth;
			uHeight = m_iHeight;
		}

		if( uWidth == 0 || uHeight == 0 )
		{
			FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "Invalid window size (%u * %u)", uWidth, uHeight );
			return;
		}

		sf::View oView = m_pWindow->getView();
		m_pWindow->create( sf::VideoMode( uWidth, uHeight ), m_sTitle, m_uStyle, m_oContext );
		m_pWindow->setView( oView );

		if( m_oIcon.getPixelsPtr() != nullptr )
			m_pWindow->setIcon( 32, 32, m_oIcon.getPixelsPtr() );

		Event oEvent( Event::eToggleFullScreen );

		oEvent.m_oFullScreen.m_iWindowIndex = m_iIndex;
		oEvent.m_oFullScreen.m_bIsFullScreen = _bFullScreen;

		g_pFZN_Core->PushEvent( oEvent );
	}

	void CustomWindow::SetIcon( const std::string& _sIconPath )
	{
		if( _sIconPath.empty() )
			return;
		
		g_pFZN_DataMgr->LoadSfImage( m_oIcon, _sIconPath );

		if( m_oIcon.getPixelsPtr() != nullptr )
			m_pWindow->setIcon( 32, 32, m_oIcon.getPixelsPtr() );
	}

	//=========================================================
	///=====================WINDOW MANAGER=====================
	//=========================================================

	/////////////////CONSTRUCTOR / DESTRUCTOR/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Default constructor
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	WindowManager::WindowManager()
		: m_width( 1024 )
		, m_height( 768 )
		, m_iMainWindow( 0 )
		//, m_bClipCursor( FALSE )
		, m_bCloseAppWithLastWindow( true )
		//, m_iTimer( 0 )
		//, m_iDesiredFrameTime( 0 )
		, m_fTimeFactor( 1.f )
	{
		//m_frameTime = m_frameClock.restart();

		TIMECAPS tc;
		timeGetDevCaps( &tc, sizeof( TIMECAPS ) );
		m_uMinSupportedPeriod = tc.wPeriodMin;

		ImGui::CreateContext();

		g_pFZN_WindowMgr = this;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Destructor
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	WindowManager::~WindowManager()
	{
		for( fzn::CustomWindow* pWindow : m_oWindows )
		{
			CheckNullptrDelete( pWindow );
		}

		g_pFZN_WindowMgr = nullptr;
	}


	//=========================================================
	//==========================PUBLIC==========================
	//=========================================================

	/////////////////DISPLAY MANAGEMENT/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Window management, updates and display functions (tests the main window to keep going)
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void WindowManager::Update()
	{
		sf::Context context;
		int iCallBackSize = 0;

		if( m_oWindows.empty() || m_oWindows[m_iMainWindow] == nullptr )
			return;

		/*ImGui::CreateContext();*/
		ImGui::SFML::Init( *m_oWindows[m_iMainWindow]->m_pWindow );

		while( m_oWindows[m_iMainWindow]->m_pWindow->isOpen() )
		{
			for( fzn::CustomWindow* pWindow : m_oWindows )
				pWindow->Update();
		}

		RemoveClosedWindows();

		if( m_oWindows.empty() && m_bCloseAppWithLastWindow )
		{
			ImGui::SFML::Shutdown();
			g_pFZN_Core->QuitApplication();
		}
	}


	void WindowManager::ProcessEventsCallBacks()
	{
		for( fzn::CustomWindow* pWindow : m_oWindows )
			pWindow->ProcessEventsCallBacks();
	}

	/////////////////OTHER FUNCTIONS/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Creates and adds a new window to the windows array
	//Return value : Index of the created window
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	int WindowManager::AddWindow( sf::Uint32 _style /*= sf::Style::Close*/ )
	{
		return AddWindow( 1280, 720, _style, "FaZoN" );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Creates and adds a new window to the windows array
	//Parameter 1 : Desired width for the window
	//Parameter 2 : Desired height for the window
	//Parameter 3 : Desired style for the window (None, Titlebar, Resize, Close, Fullscreen)
	//Parameter 4 : Title of the window
	//Parameter 5 : Position of the window
	//Return value : Index of the created window
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	int WindowManager::AddWindow( int _width, int _height, sf::Uint32 _style /*= sf::Style::Close*/, sf::String _title /*= "FaZoN"*/, sf::Vector2i _position /*= { -1, -1 }*/ )
	{
		CustomWindow* newWindow = new CustomWindow( m_oWindows.size(), _width, _height, _style, _title, m_oContext );

		if( _position != sf::Vector2i( -1, -1 ) )
			newWindow->m_pWindow->setPosition( _position );

		m_oWindows.push_back( newWindow );

		if( m_oWindows.size() == 1 )
			g_pFZN_Core->OnFirstWindowAdded();

		return newWindow->m_iIndex;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Closes the game main window
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void WindowManager::Close()
	{
		Close( m_iMainWindow );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Closes a window by index
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void WindowManager::Close( int _id )
	{
		if( m_oWindows.empty() || m_oWindows[_id] == nullptr )
			return;

		m_oWindows[_id]->m_pWindow->close();

		if( m_iMainWindow == _id )
			DetermineNewMainWindow();
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Sets the position of a window chosen by its index
	//Parameter 1 : Desired position of the window
	//Parameter 2 : Index of the window
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void WindowManager::SetWindowPosition( sf::Vector2i _position, int _id /*= -1 */)
	{
		if( _id < 0 )
			_id = m_iMainWindow;

		if( m_oWindows.empty() || m_oWindows[_id] == nullptr )
			return;

		m_oWindows[_id]->m_pWindow->setPosition( _position );
	}


	void WindowManager::SetWindowClearColor( const sf::Color& _oColor, int _id /*= -1 */ )
	{
		if( _id < 0 )
			_id = m_iMainWindow;

		if( m_oWindows.empty() || m_oWindows[ _id ] == nullptr )
			return;

		m_oWindows[ _id ]->m_oClearColor = _oColor;
	}

	/////////////////WINDOW FUNCTIONS/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Element display on a chosen window
	//Parameter 1 : Element to display
	//Parameter 2 : Window ID
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void WindowManager::Draw( const sf::Drawable& _drawable, int _id /*= -1 */ )
	{
		if( _id < 0 )
			_id = m_iMainWindow;

		if( m_oWindows.empty() || m_oWindows[_id] == nullptr )
			return;

		m_oWindows[_id]->m_pWindow->draw( _drawable );
	}

	void WindowManager::Draw( const sf::Drawable& _drawable, const sf::RenderStates& _oRenderState )
	{
		if( m_oWindows.empty() || m_oWindows[ m_iMainWindow ] == nullptr )
			return;

		m_oWindows[ m_iMainWindow ]->m_pWindow->draw( _drawable, _oRenderState );
	}

	void WindowManager::Draw( const sf::Drawable& _oDrawable, sf::RenderTexture& _oRenderTexture, const sf::RenderStates& _oRenderState /*= sf::RenderStates::Default */ )
	{
		_oRenderTexture.draw( _oDrawable, _oRenderState );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on the size of a chosen window
	//Parameter : Window ID
	//Return value : Size of the window
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	sf::Vector2u WindowManager::GetWindowSize( int _id /*= -1 */ )
	{
		if( _id < 0 )
			_id = m_iMainWindow;

		if( m_oWindows.empty() || m_oWindows[ _id ] == nullptr )
			return sf::Vector2u( 0, 0 );

		return m_oWindows[_id]->m_pWindow->getSize();
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Mutator on the size of a chosen window
	//Parameter 1 : Chosen size
	//Parameter 2 : Window ID
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void WindowManager::SetWindowSize( const sf::Vector2u& _size, int _id /*= -1 */ )
	{
		if( _id < 0 )
			_id = m_iMainWindow;

		if( m_oWindows.empty() || m_oWindows[ _id ] == nullptr )
			return;

		if( _id == m_iMainWindow )
		{
			m_width = _size.x;
			m_height = _size.y;
		}

		m_oWindows[ _id ]->SetSize( _size );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Mutator on the title of a chosen window
	//Parameter 1 : Title to set
	//Parameter 2 : Window ID
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void WindowManager::SetWindowTitle( const sf::String& _title, int _id /*= -1*/ )
	{
		if( _id < 0 )
			_id = m_iMainWindow;

		if( m_oWindows.empty() || m_oWindows[ _id ] == nullptr )
			return;

		m_oWindows[ _id ]->SetTitle( _title );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Mutator on the framerate of a chosen window
	//Parameter 1 : Chosen framerate
	//Parameter 2 : Window ID
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void WindowManager::SetWindowFramerate( int _framerate, int _id /*= -1*/)
	{
		if( _id < 0 )
			_id = m_iMainWindow;

		if( m_oWindows.empty() || m_oWindows[ _id ] == nullptr )
			return;

		int iWindow = _id >= (int)m_oWindows.size() ? m_iMainWindow : _id;
		m_oWindows[iWindow]->m_iDesiredFPS = _framerate;

		//m_desiredFPS = _framerate;
		//m_iDesiredFrameTime = 1000000.f / m_desiredFPS;
		//m_iTimer = 0;
		//m_oWindows[iWindow]->m_pWindow->setFramerateLimit( _framerate );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Mutator on the icon of a chosen window
	//Parameter 1 : Path to the icon
	//Parameter 2 : Window ID
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void WindowManager::SetIcon( const std::string& _szPath, int _id /*= -1 */ )
	{
		if( _id < 0 )
			_id = m_iMainWindow;

		if( m_oWindows.empty() || m_oWindows[ _id ] == nullptr )
			return;

		m_oWindows[ _id ]->SetIcon( _szPath );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on a chosen window
	//Parameter : Window ID
	//Return value : Wanted window
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	sf::RenderWindow* WindowManager::GetWindow( int _id /*= -1 */ )
	{
		if( _id < 0 )
			_id = m_iMainWindow;

		if( m_oWindows.empty() || m_oWindows[ _id ] == nullptr )
			return nullptr;

		return m_oWindows[_id]->m_pWindow;
	}

	sf::Event WindowManager::GetWindowEvent( int _id /*= -1 */ )
	{
		if( _id < 0 )
			_id = m_iMainWindow;

		if( m_oWindows.empty() || m_oWindows[ _id ] == nullptr )
			return sf::Event();

		return m_oWindows[ _id ]->m_oEvent;
	}

	void WindowManager::ToggleWindowFullScreen( int _id /*= -1 */ )
	{
		if( _id < 0 )
			_id = m_iMainWindow;

		if( m_oWindows.empty() || m_oWindows[ _id ] == nullptr )
			return;

		ToggleWindowFullScreen( !m_oWindows[ _id ]->IsFullScreen() );
	}

	void WindowManager::ToggleWindowFullScreen( bool _bFullScreen, int _id /*= -1 */ )
	{
		if( _id < 0 )
			_id = m_iMainWindow;

		if( m_oWindows.empty() || m_oWindows[ _id ] == nullptr )
			return;

		m_oWindows[ _id ]->ToggleFullScreen( _bFullScreen );
	}

	void WindowManager::DetermineNewMainWindow()
	{
		for( int iWindow = 0 ; iWindow < (int)m_oWindows.size() ; ++ iWindow )
		{
			if( m_oWindows[iWindow]->m_pWindow->isOpen() )
				m_iMainWindow = iWindow;
		}
	}

	sf::Vector2f WindowManager::GetMousePosition( int _iWindowID /*= -1*/ )
	{
		if( const sf::RenderWindow* pWindow = GetWindow( _iWindowID ) )
		{
			return pWindow->mapPixelToCoords( sf::Mouse::getPosition( *pWindow ) );
		}

		return { 0.f, 0.f };
	}

	/////////////////ACCESSORS / MUTATORS/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on the time of a frame
	//Return value : Time in seconds
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	float WindowManager::GetFrameTimeS()
	{
		return GetUnmodifiedFrameTimeS() * m_fTimeFactor;
	}

	float WindowManager::GetUnmodifiedFrameTimeS()
	{
		if( m_iMainWindow >= (int)m_oWindows.size() )
			return 0;

		return m_oWindows[m_iMainWindow]->m_fFrameTime;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on the main window index
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	int WindowManager::GetMainWindowIndex()
	{
		return m_iMainWindow;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Definition of a new main window by index
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void WindowManager::SetMainWindow( int _newMainWindow )
	{
		if( _newMainWindow < 0 || _newMainWindow >= (int)m_oWindows.size() )
			return;

		m_iMainWindow = _newMainWindow;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Returns if the application will close with the last window or not.
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	bool WindowManager::GetCloseWithLastWindow()
	{
		return m_bCloseAppWithLastWindow;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Sets if the application has to close with the last window.
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void WindowManager::SetCloseWithLastWindow( bool _bClose )
	{
		m_bCloseAppWithLastWindow = _bClose;
	}

	void WindowManager::SetTimeFactor( float _fTimeFactor )
	{
		if( _fTimeFactor >= 0.f )
			m_fTimeFactor = _fTimeFactor;
	}

	float WindowManager::GetTimeFactor() const
	{
		return m_fTimeFactor;
	}

	void WindowManager::RemoveClosedWindows()
	{
		std::vector<CustomWindow*>::iterator itWindow = m_oWindows.begin();

		while( itWindow != m_oWindows.end() )
		{
			if( (*itWindow)->m_pWindow->isOpen() == false )
			{
				itWindow = m_oWindows.erase( itWindow );
			}
			else
				itWindow++;
		}
	}

} //namespace fzn
