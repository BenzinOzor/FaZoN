//------------------------------------------------------------------------
//Author : Philippe OFFERMANN
//Date : 03.10.14
//Description : Graphical elements management
//------------------------------------------------------------------------


#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_


#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/WindowStyle.hpp>

#include <FZN/Managers/FazonCore.h>

namespace sf
{
	class RenderWindow;
}

namespace fzn
{
	class Animation;
	class Anm2;
	class ProgressBar;

	//=========================================================
	///======================CUSTOMWINDOW======================
	//=========================================================

	class CustomWindow
	{
	public :
		CustomWindow( int _iIndex, int _width, int _height, sf::Uint32 _style = sf::Style::Close, sf::String _title = "FaZoN", const sf::ContextSettings& settings = sf::ContextSettings() );

		void Update();
		void ProcessFramerate();
		bool ClockFramerate();
		void ProcessEventsCallBacks();

		void SetSize( const sf::Vector2u& _vSize );
		void SetTitle( const std::string& _sTitle );
		bool IsFullScreen() const;
		void ToggleFullScreen( bool _bFullScreen );
		void SetIcon( const std::string& _sIconPath );

		sf::RenderWindow*	m_pWindow;
		sf::Event			m_oEvent;
		CallBacksVector		m_oCallBacks[FazonCore::CallBacks::CB_Nb];			//Array of 3 Lists containing callBacks structures.

		unsigned int		m_iIndex;				//Index of the window in Graphics.
		unsigned int		m_iWidth;				//Width of the window (read only).
		int					m_iHeight;				//Height of the window (read only).
		float				m_fFrameTime;			//Delta time between each frame in seconds.
		float				m_fFramerate;
		int					m_iDesiredFPS;
		sf::Int64			m_iLastTick;
		bool				m_bClipCursor;
		bool				m_bCheckInputEvents;
		bool				m_bMustClose;
		sf::Uint32			m_uStyle;
		std::string			m_sTitle;
		sf::ContextSettings	m_oContext;

		sf::Clock			m_oClock;

		sf::Color			m_oClearColor;

		sf::Image			m_oIcon;
	};


	//=========================================================
	///=====================WINDOW MANAGER=====================
	//=========================================================

	class FZN_EXPORT WindowManager : public sf::NonCopyable
	{
		friend class FazonCore;
		friend class CustomWindow;

	public:
		/////////////////CONSTRUCTOR / DESTRUCTOR/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Default constructor
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		WindowManager();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Destructor
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		~WindowManager();


		/////////////////WINDOWS MANAGEMENT/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Window management, updates and display functions (tests the main window to keep going)
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void Update();

		void ProcessEventsCallBacks();


		/////////////////OTHER FUNCTIONS/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Creates and adds a new window to the windows array
		//Return value : Index of the created window
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		int AddWindow( sf::Uint32 _style = sf::Style::Close );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Creates and adds a new window to the windows array
		//Parameter 1 : Desired width for the window
		//Parameter 2 : Desired height for the window
		//Parameter 3 : Desired style for the window (None, Titlebar, Resize, Close, Fullscreen)
		//Parameter 4 : Title of the window
		//Parameter 5 : Position of the window
		//Return value : Index of the created window
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		int AddWindow( int _width, int _height, sf::Uint32 _style = sf::Style::Close, sf::String _title = "FaZoN", sf::Vector2i _position = { -1, -1 } );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Closes the main window
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void Close();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Closes a window by index
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void Close( int id );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Sets the position of a window chosen by its index
		//Parameter 1 : Desired position of the window
		//Parameter 2 : Index of the window
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void SetWindowPosition( sf::Vector2i _position, int _id = -1 );
		void SetWindowClearColor( const sf::Color& _oColor, int _id = -1 );


		/////////////////WINDOW FUNCTIONS/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Element display on a chosen window
		//Parameter 1 : Element to display
		//Parameter 2 : Window ID
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void Draw( const sf::Drawable& _drawable, int _id = -1 );
		void Draw( const sf::Drawable& _drawable, const sf::RenderStates& _oRenderState );
		void Draw( const sf::Drawable& _oDrawable, sf::RenderTexture& _oRenderTexture, const sf::RenderStates& _oRenderState = sf::RenderStates::Default );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on the size of a chosen window
		//Parameter : Window ID
		//Return value : Size of the window
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		sf::Vector2u GetWindowSize( int _id = -1 );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Mutator on the size of a chosen window
		//Parameter 1 : Chosen size
		//Parameter 2 : Window ID
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void SetWindowSize( const sf::Vector2u& _size, int _id = -1 );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Mutator on the title of a chosen window
		//Parameter 1 : Title to set
		//Parameter 2 : Window ID
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void SetWindowTitle( const sf::String& _title, int _id = -1 );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Mutator on the framerate of a chosen window
		//Parameter 1 : Chosen framerate
		//Parameter 2 : Window ID
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void SetWindowFramerate( int _framerate, int _id = -1 );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Mutator on the icon of a chosen window
		//Parameter 1 : Path to the icon
		//Parameter 2 : Window ID
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void SetIcon( const std::string& _path, int _id = -1 );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on a chosen window
		//Parameter : Window ID
		//Return value : Wanted window
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		sf::RenderWindow* GetWindow( int _id = -1 );
		sf::Event GetWindowEvent( int _id = -1 );
		void ToggleWindowFullScreen( int _id = -1 );
		void ToggleWindowFullScreen( bool _bFullScreen, int _id = -1 );

		void DetermineNewMainWindow();


		/////////////////ACCESSORS / MUTATORS/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on the time of a frame
		//Return value : Time in seconds
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		float GetFrameTimeS();
		float GetUnmodifiedFrameTimeS();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on the main window index
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		int GetMainWindowIndex();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Definition of a new main window by index
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void SetMainWindow( int _newMainWindow );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Returns if the application will close with the last window or not.
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		bool GetCloseWithLastWindow();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Sets if the application has to close with the last window.
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void SetCloseWithLastWindow( bool _bClose );

		void SetTimeFactor( float _fTimeFactor );
		float GetTimeFactor() const;


		/////////////////MEMBER VARIABLES/////////////////

		//INT8 m_bClipCursor;			//Constrain the cursor in the application's window (true) or not
		UINT m_width;				//Width of the window
		UINT m_height;				//Height of the window
		//UINT m_desiredFPS;			//Theorical FPS (to set the framerate on the window)
		//float m_framerate;			//Real framerate calculated from the game loop

	private:

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Adds a callBack function to the chosen vector
		//Parameter 1 : Data pointer (class Type)
		//Parameter 2 : Function pointer
		//Parameter 3 : Chosen vector
		//Return value : Function index in the vector
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		int AddCallBack( void* _pData, CallBack _pFct, FazonCore::CallBacks _eCallBackType, int _iWindow = -1, int _iPriority = 0 );
		void RemoveCallBack( void* _pData, CallBack _pFct, FazonCore::CallBacks _eCallBackType, int _iWindow = -1 );

		void RemoveClosedWindows();


		/////////////////MEMBER VARIABLES/////////////////
		std::vector<CustomWindow*>		m_oWindows;					//Container of custom windows
		//int							m_iNbWindows;				//Number of windows in the array
		int								m_iMainWindow;				//Index of the main window

		//Vector<FazonCore::DataCallBack>	m_oCallBacks[FazonCore::CallBacks::CB_Nb];			//Array of 3 Lists containing callBacks structures
		sf::ContextSettings				m_oContext;					//Window contextSettings (default values, for threading issues)
		//sf::Clock						m_frameClock;				//Frame timer
		//sf::Time						m_frameTime;				//Frame time (seconds)
		//sf::Event						m_event;					//Event on the game window
		bool							m_bCloseAppWithLastWindow;	//Closes the application when the last windows is closed.

		//int m_iTimer;
		//int m_iDesiredFrameTime;
		//float m_fFrameTime;

		UINT m_uMinSupportedPeriod;
		float m_fTimeFactor;
	};
} //namespace fzn

extern FZN_EXPORT fzn::WindowManager* g_pFZN_WindowMgr;

#define FZN_Window				g_pFZN_WindowMgr->GetWindow()				//Accessor on the main window
#define FrameTime				g_pFZN_WindowMgr->GetFrameTimeS()			//Accessor on the time of a frame (seconds)
#define UnmodifiedFrameTime		g_pFZN_WindowMgr->GetUnmodifiedFrameTimeS()	//Accessor on the time of a frame (seconds)
#define W_HEIGHT				g_pFZN_WindowMgr->m_height					//Height of the window
#define W_WIDTH					g_pFZN_WindowMgr->m_width					//Width of the window
#define FZN_MainWindow			g_pFZN_WindowMgr->GetMainWindowIndex()		//Index of the main window

#endif //_GRAPHICS_H_
