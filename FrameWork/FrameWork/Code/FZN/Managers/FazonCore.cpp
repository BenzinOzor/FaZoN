//------------------------------------------------------------------------
//Author : Philippe OFFERMANN
//Date : 1.11.14
//Description : Core management of the main classes
//------------------------------------------------------------------------

#include <direct.h>
#include <filesystem>
#include <io.h>

#include <ExternalWrapper/EWFileEncrypter.h>

#include "FZN/Includes.h"
#include "FZN/Managers/InputManager.h"
#include "FZN/Managers/DataManager.h"
#include "FZN/Managers/AnimManager.h"
//#include "FZN/Managers/WindowManager.h"
#include "FZN/Managers/AudioManager.h"
#include "FZN/Managers/SteeringManager.h"
#include "FZN/Managers/MessageManager.h"
#include "FZN/Managers/AIManager.h"
#include "FZN/Managers/FazonCore.h"
#include "FZN/Tools/Callbacks.h"

#include <ShlObj_core.h>

FZN_EXPORT fzn::FazonCore* g_pFZN_Core = nullptr;

namespace fzn
{
	FazonCore* FazonCore::m_singleton = nullptr;


	/////////////////CONSTRUCTOR / DESTRUCTOR/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Default constructor, creation of the engine's singletons
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	FazonCore::FazonCore()
	{
		m_sProjectName = "FaZoN";
		m_bUseCryptedData = false;

		m_sDataPath = "../../Data/";
		m_sSaveFolderName = "FaZoN_App";

		m_sSaveFolderPath = "";
		m_eProjectType = FZNProjectType::COUNT_PROJECT_TYPES;

		m_iActivatedModulesNbr = 0;

		m_pInputManager = nullptr;
		m_pDataManager = nullptr;
		m_pAnimManager = nullptr;
		m_pWindowManager = nullptr;
		m_pAudioManager = nullptr;
		m_pSteeringManager = nullptr;
		m_pAIManager = nullptr;
		m_pMessageManager = nullptr;

		m_bExitApp = false;

		m_console = GetConsoleWindow();

		srand( (unsigned)time( nullptr ) );

		g_pFZN_Core = this;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Destructor, destruction of the engine's singletons
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	FazonCore::~FazonCore()
	{
		Deinit();

		g_pFZN_Core = nullptr;
	}


	//=========================================================
	//==========================PUBLIC==========================
	//=========================================================

	/////////////////SINGLETON MANAGEMENT/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Creation and access on the singleton
	//Return value : FazonCore unic instance
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	FazonCore* FazonCore::CreateInstance()
	{
		if( m_singleton == nullptr )
			m_singleton = new FazonCore;

		return m_singleton;
	}
	FazonCore* FazonCore::CreateInstance( const ProjectDesc& _rDesc )
	{
		CreateInstance();

		m_singleton->Init( _rDesc );

		return m_singleton;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Destruction of the singleton
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void FazonCore::DestroyInstance()
	{
		if( m_singleton != nullptr )
		{
			delete m_singleton;
			m_singleton = nullptr;
		}
	}


	/////////////////CORE HANDLING/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Update of the engine components
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void FazonCore::Init( const ProjectDesc& _oDesc )
	{
		m_sProjectName		= _oDesc.m_sName;
		m_bUseCryptedData	= _oDesc.m_bUseCryptedData;
		m_eProjectType		= _oDesc.m_eProjectType;

		if( m_bUseCryptedData )
			EWFileEncrypter::InitKeys( m_sProjectName );

		if( _oDesc.m_sDataFolderPath.empty() == false )
			SetDataFolder( _oDesc.m_sDataFolderPath.c_str() );

		if( _oDesc.m_sSaveFolderName.empty() == false )
			SetSaveFolderName( _oDesc.m_sSaveFolderName );

		FZN_LOG( "FaZoN Core Init:" );
		FZN_LOG( "Project name: %s", m_sProjectName.c_str() );
		FZN_LOG( "Data folder: %s", GetDataFolder().c_str() );
		FZN_LOG( "Save folder name: %s", m_sSaveFolderName.c_str() );
		FZN_LOG( "Save folder path: %s", GetSaveFolderPath().c_str() );
		FZN_LOG( "Use crypted data: %s", m_bUseCryptedData ? "Yes" : "No" );

		_CreateSaveFolder();

		if( m_pInputManager == nullptr )
		{
			m_pInputManager = new InputManager;
			m_iActivatedModulesNbr++;
		}

		if( m_pDataManager == nullptr )
		{
			m_pDataManager = new DataManager;
			m_iActivatedModulesNbr++;
		}

		if( m_pAnimManager == nullptr )
		{
			m_pAnimManager = new AnimManager;
			m_iActivatedModulesNbr++;

			//m_oThread = new sf::Thread( &AnimManager::Update, m_pAnimManager );
		}

		if( m_pWindowManager == nullptr )
		{
			m_pWindowManager = new WindowManager;
			m_iActivatedModulesNbr++;
		}

		if( m_pAudioManager == nullptr )
		{
			m_pAudioManager = new AudioManager( _oDesc.m_bUseFMOD );
			m_iActivatedModulesNbr++;
		}
	}

	void FazonCore::InitModule( CoreModules eModule )
	{
		switch( eModule )
		{
		case InputModule:
			if( m_pInputManager == nullptr )
			{
				m_pInputManager = new InputManager;
				m_iActivatedModulesNbr++;
			}
			break;
		case DataModule:
			if( m_pDataManager == nullptr )
			{
				m_pDataManager = new DataManager;
				m_iActivatedModulesNbr++;
			}
			break;
		case AnimModule:
			if( m_pAnimManager == nullptr )
			{
				m_pAnimManager = new AnimManager;
				m_iActivatedModulesNbr++;
			}
			break;
		case WindowModule:
			if( m_pWindowManager == nullptr )
			{
				m_pWindowManager = new WindowManager;
				m_iActivatedModulesNbr++;

				if( m_pInputManager != nullptr )
					m_pInputManager->SetInputSystem( InputManager::EventSystem );
			}
			break;
		case AudioModule:
			if( m_pAudioManager == nullptr )
			{
				m_pAudioManager = new AudioManager;
				m_iActivatedModulesNbr++;
			}
			break;
		case SteeringModule:
			if( m_pSteeringManager == nullptr )
			{
				m_pSteeringManager = new SteeringManager;
				m_iActivatedModulesNbr++;
			}
			break;
		case MessageModule:
			if( m_pMessageManager == nullptr )
			{
				m_pMessageManager = new MessageManager;
				m_iActivatedModulesNbr++;
			}
			break;
		default:
			break;
		};
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Update of the engine components
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void FazonCore::Update()
	{
		_ManageEvents();

		if( m_iActivatedModulesNbr == 0 )
			return;

		if( m_pInputManager != nullptr )	m_pInputManager->Update();
		if( m_pAnimManager != nullptr )		m_pAnimManager->Update();
		if( m_pAudioManager != nullptr )	m_pAudioManager->Update();

		if( m_pSteeringManager != nullptr )	m_pSteeringManager->Update();
		if( m_pMessageManager != nullptr )	m_pMessageManager->Update();
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Update of the engine components
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void FazonCore::Deinit()
	{
		if( m_iActivatedModulesNbr == 0 )
			return;

		if( m_pInputManager != nullptr )
		{
			delete m_pInputManager;
			m_pInputManager = nullptr;
			if( m_iActivatedModulesNbr > 0 ) m_iActivatedModulesNbr--;
		}

		if( m_pDataManager != nullptr )
		{
			delete m_pDataManager;
			m_pDataManager = nullptr;
			if( m_iActivatedModulesNbr > 0 ) m_iActivatedModulesNbr--;
		}

		if( m_pAnimManager != nullptr )
		{
			/*m_oThread->terminate();

			delete m_oThread;*/

			delete m_pAnimManager;
			m_pAnimManager = nullptr;
			if( m_iActivatedModulesNbr > 0 ) m_iActivatedModulesNbr--;
		}

		if( m_pWindowManager != nullptr )
		{
			delete m_pWindowManager;
			m_pWindowManager = nullptr;
			if( m_iActivatedModulesNbr > 0 ) m_iActivatedModulesNbr--;
		}

		if( m_pAudioManager != nullptr )
		{
			delete m_pAudioManager;
			m_pAudioManager = nullptr;
			if( m_iActivatedModulesNbr > 0 ) m_iActivatedModulesNbr--;
		}

		if( m_pSteeringManager != nullptr )
		{
			delete m_pSteeringManager;
			m_pSteeringManager = nullptr;
			if( m_iActivatedModulesNbr > 0 ) m_iActivatedModulesNbr--;
		}

		if( m_pAIManager != nullptr )
		{
			delete m_pAIManager;
			m_pAIManager = nullptr;
			if( m_iActivatedModulesNbr > 0 ) m_iActivatedModulesNbr--;
		}

		if( m_pMessageManager != nullptr )
		{
			delete m_pMessageManager;
			m_pMessageManager = nullptr;
			if( m_iActivatedModulesNbr > 0 ) m_iActivatedModulesNbr--;
		}
	}

	void FazonCore::DeinitModule( CoreModules eModule )
	{
		if( m_iActivatedModulesNbr == 0 )
			return;

		switch( eModule )
		{
		case InputModule:
			if( m_pInputManager != nullptr )
			{
				delete m_pInputManager;
				m_pInputManager = nullptr;
				m_iActivatedModulesNbr--;
			}
			break;
		case DataModule:
			if( m_pDataManager != nullptr )
			{
				delete m_pDataManager;
				m_pDataManager = nullptr;
				m_iActivatedModulesNbr--;
			}
			break;
		case AnimModule:
			if( m_pAnimManager != nullptr )
			{
				delete m_pAnimManager;
				m_pAnimManager = nullptr;
				m_iActivatedModulesNbr--;
			}
			break;
		case WindowModule:
			if( m_pWindowManager != nullptr )
			{
				if( m_iActivatedModulesNbr > 1 )
				{
					_TransferCallBacks( false );

					if( m_pInputManager != nullptr )
						m_pInputManager->SetInputSystem( InputManager::ScanSystem );
				}

				delete m_pWindowManager;
				m_pWindowManager = nullptr;
				m_iActivatedModulesNbr--;
			}
			break;
		case AudioModule:
			if( m_pAudioManager != nullptr )
			{
				delete m_pAudioManager;
				m_pAudioManager = nullptr;
				m_iActivatedModulesNbr--;
			}
			break;
		case SteeringModule:
			if( m_pSteeringManager != nullptr )
			{
				delete m_pSteeringManager;
				m_pSteeringManager = nullptr;
				m_iActivatedModulesNbr--;
			}
			break;
		case MessageModule:
			if( m_pMessageManager != nullptr )
			{
				delete m_pMessageManager;
				m_pMessageManager = nullptr;
				m_iActivatedModulesNbr--;
			}
			break;
		default:
			break;
		};
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Updates the game loop
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void FazonCore::GameLoop()
	{
		while( !m_bExitApp )
		{
			if( m_pWindowManager != nullptr )
				m_pWindowManager->Update();
			else
			{
				Update();

				m_oCallbacksHolder.ExecuteCallbacks( DataCallbackType::Update );
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Quits the application
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void FazonCore::QuitApplication()
	{
		if( m_iActivatedModulesNbr == 0 )
			return;

		if( m_pWindowManager != nullptr )
			m_pWindowManager->Close();

		m_bExitApp = true;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Activates the steering manager
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void FazonCore::ActivateSteeringManager()
	{
		if( m_pSteeringManager == nullptr )
		{
			m_pSteeringManager = new SteeringManager;
			m_iActivatedModulesNbr++;
		}
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Deactivates the steering manager
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void FazonCore::DeactivateSteeringManager()
	{
		if( m_pSteeringManager != nullptr )
		{
			delete m_pSteeringManager;
			m_pSteeringManager = nullptr;
			if( m_iActivatedModulesNbr > 0 ) m_iActivatedModulesNbr--;
		}
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Activates the ai and message managers
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void FazonCore::ActivateAIModule()
	{
		if( m_pAIManager == nullptr )
		{
			m_pAIManager = new AIManager;
			m_iActivatedModulesNbr++;
		}

		if( m_pMessageManager == nullptr )
		{
			m_pMessageManager = new MessageManager;
			m_iActivatedModulesNbr++;
		}
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Deactivates the ai and message managers
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void FazonCore::DeactivateAIModule()
	{
		if( m_pAIManager != nullptr )
		{
			delete m_pAIManager;
			m_pAIManager = nullptr;
			if( m_iActivatedModulesNbr > 0 ) m_iActivatedModulesNbr--;
		}

		if( m_pMessageManager != nullptr )
		{
			delete m_pMessageManager;
			m_pMessageManager = nullptr;
			if( m_iActivatedModulesNbr > 0 ) m_iActivatedModulesNbr--;
		}
	}


	/////////////////CONSOLE HANDLING/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Hides the application console
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void FazonCore::HideConsole()
	{
		ShowWindow( m_console, SW_HIDE );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Shows the application console
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void FazonCore::ShowConsole()
	{
		ShowWindow( m_console, SW_SHOW );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Sets the application console title
	//Parameter : New title
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void FazonCore::ConsoleTitle( const char* _title )
	{
		SetConsoleTitleA( (LPCSTR)_title );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Sets the application console position
	//Parameter : x and y positions
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void FazonCore::SetConsolePosition( const sf::Vector2i& _position )
	{
		SetWindowPos( m_console, nullptr, _position.x, _position.y, 0, 0, SWP_NOSIZE );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Sets the application console position
	//Parameter 1 : x position
	//Parameter 2 : y position
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void FazonCore::SetConsolePosition( int _x, int _y )
	{
		SetWindowPos( m_console, nullptr, _x, _y, 0, 0, SWP_NOSIZE );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Sets the application console size
	//Parameter : Width and height
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void FazonCore::SetConsoleSize( sf::Vector2i& _size )
	{
		SetWindowPos( m_console, nullptr, 0, 0, _size.x, _size.y, SWP_NOMOVE );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Sets the application console size
	//Parameter 1 : New width
	//Parameter 2 : New height
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void FazonCore::SetConsoleSize( int _width, int _height )
	{
		SetWindowPos( m_console, nullptr, 0, 0, _width, _height, SWP_NOMOVE );
	}


	/////////////////OTHER FUNCTIONS/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Displays a kind message for the user
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void FazonCore::GreetingMessage()
	{
		char* userName = new char[20];
		DWORD size = 20;

		GetUserNameA( userName, &size );

		int length = strlen( userName );
		float fLen = length * 0.5f;
		int nbSpaces = 31 - (int)fLen;
		std::string spaces = "\0";

		for( DWORD i = 0 ; i < size ; ++i )
			userName[i] = toupper( userName[i] );

		for( int i = 0 ; i < nbSpaces ; ++i )
			spaces += " ";

		SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 2 );
		printf( "\n" );
		printf( "================================================================================\n" );
		printf( "%sHI %s AND WELCOME ON\n", spaces.c_str(), userName );
		printf( "                          _____ _____ _____ _____ _   _  \n" );
		printf( "       + +    + +   + +  | ____| __  |___  | __  | \\ | |  + +   + +    + +\n" );
		printf( "       + +    + +   + +  | __| | __  |/ __/||__| |  \\| |  + +   + +    + +\n" );
		printf( "       + +    + +   + +  |_|   |_| |_|_____|_____|_|\\__|  + +   + +    + +\n" );
		printf( "\n" );
		printf( "                                HAVE A GOOD TIME !\n" );
		printf( "\n================================================================================\n" );
		SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 15 );

		FZN_LOG( "" );
		FZN_LOG( "================================================================================" );
		FZN_LOG( "%sHI %s AND WELCOME ON", spaces.c_str(), userName );
		FZN_LOG( "                          _____ _____ _____ _____ _   _  " );
		FZN_LOG( "       + +    + +   + +  | ____| __  |___  | __  | \\ | |  + +   + +    + +" );
		FZN_LOG( "       + +    + +   + +  | __| | __  |/ __/||__| |  \\| |  + +   + +    + +" );
		FZN_LOG( "       + +    + +   + +  |_|   |_| |_|_____|_____|_|\\__|  + +   + +    + +" );
		FZN_LOG( "" );
		FZN_LOG( "                                HAVE A GOOD TIME !" );
		FZN_LOG( "================================================================================\n" );
	}


	void FazonCore::OnFirstWindowAdded()
	{
		_TransferCallBacks( true );
	}

	/////////////////ACCESSORS / MUTATORS/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on the input manager
	//Return value : Input manager
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	InputManager* FazonCore::GetInputManager()
	{
		return m_pInputManager;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on the data manager
	//Return value : Data manager
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	DataManager* FazonCore::GetDataManager()
	{
		return m_pDataManager;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on the animation manager
	//Return value : Animation manager
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	AnimManager* FazonCore::GetAnimManager()
	{
		return m_pAnimManager;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on the window manager
	//Return value : Window manager
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	WindowManager* FazonCore::GetWindowManager()
	{
		return m_pWindowManager;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on the audio manager
	//Return value : Audio manager
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	AudioManager * FazonCore::GetAudioManager()
	{
		return m_pAudioManager;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on the steering manager
	//Return value : Steering manager
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	SteeringManager* FazonCore::GetSteeringManager()
	{
		return m_pSteeringManager;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on the AI manager
	//Return value : AI manager
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	AIManager* FazonCore::GetAIManager()
	{
		return m_pAIManager;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on the message manager
	//Return value : Message manager
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	MessageManager* FazonCore::GetMessageManager()
	{
		return m_pMessageManager;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on the application global timer
	//Return value : Elapsed time
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	sf::Time FazonCore::GetGlobalTime()
	{
		return m_GlobalTime.getElapsedTime();
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on the indicator of the members creation
	//Return value : The components are created (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	int FazonCore::GetComponentsNumber()
	{
		return m_iActivatedModulesNbr;
	}

	bool FazonCore::IsModuleCreated( CoreModules eModule ) const
	{
		switch( eModule )
		{
		case InputModule:
			return m_pInputManager != nullptr;
		case DataModule:
			return m_pDataManager != nullptr;
		case AnimModule:
			return m_pAnimManager != nullptr;
		case WindowModule:
			return m_pWindowManager != nullptr;
		case AudioModule:
			return m_pAudioManager != nullptr;
		case SteeringModule:
			return m_pSteeringManager != nullptr;
		case MessageModule:
			return m_pMessageManager != nullptr;
		default:
			return FALSE;
		};
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on the indicator of the steering manager creation
	//Return value : The steering manager is created (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	INT8 FazonCore::IsSteeringManagerActivated()
	{
		return m_pSteeringManager != nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on the indicator of the ai and message managers creation
	//Return value : The ai and message managers are created (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	INT8 FazonCore::IsAIModuleActivated()
	{
		return m_pAIManager != nullptr && m_pMessageManager != nullptr;
	}

	bool FazonCore::IsUsingFMOD() const
	{
		if( m_pAudioManager == nullptr )
			return false;

		return m_pAudioManager->IsUsingFMOD();
	}

	const std::string& FazonCore::GetProjectName() const
	{
		return m_sProjectName;
	}

	bool FazonCore::IsUsingCryptedData() const
	{
		return m_bUseCryptedData;
	}

	std::string& FazonCore::GetDataFolder()
	{
		return m_sDataPath;
	}

	void FazonCore::SetDataFolder( const char* _path )
	{
		m_sDataPath = _path;
	}

	std::string FazonCore::GetDataPath( const char* _path )
	{
		std::string sDataPath = m_sDataPath;

		sDataPath += _path;

		return sDataPath;
	}

	void FazonCore::SetSaveFolderName( const std::string& _sFolderName )
	{
		m_sSaveFolderName = _sFolderName;
	}

	const std::string& FazonCore::GetSaveFolderName() const
	{
		return m_sSaveFolderName;
	}

	std::string FazonCore::GetSaveFolderPath() const
	{
		return m_sSaveFolderPath + "/" + m_sSaveFolderName;
	}

	bool FazonCore::FileExists( const std::string& _sFilePath ) const
	{
		return std::filesystem::exists( _sFilePath );
	}

	void FazonCore::PushEvent( const Event& _oEvent )
	{
		m_oEvents.push( _oEvent );
	}

	void FazonCore::PushEvent( void* _pUserData )
	{
		if( _pUserData == nullptr )
			return;

		Event oEvent( Event::Type::eUserEvent );
		oEvent.m_pUserData = _pUserData;

		m_oEvents.push( oEvent );
	}

	const fzn::Event& FazonCore::GetEvent() const
	{
		return m_oCurrentEvent;
	}

	void FazonCore::_TransferCallBacks( bool _bToWindowManager )
	{
		if( m_pWindowManager == nullptr )
			return;

		if( _bToWindowManager )
		{
			m_pWindowManager->m_oWindows[ m_pWindowManager->m_iMainWindow ]->m_oCallbacksHolder = m_oCallbacksHolder;
		}
		else if( m_pWindowManager->m_oWindows.empty() == false )
		{
			m_oCallbacksHolder = m_pWindowManager->m_oWindows[ m_pWindowManager->m_iMainWindow ]->m_oCallbacksHolder;
		}
	}

	void FazonCore::_CreateSaveFolder()
	{
		if( m_sSaveFolderName.empty() )
			return;

		FZN_LOG( "Creating save folder..." );

		if( m_sSaveFolderPath.empty() )
		{
			CHAR   pDocumentPath[MAX_PATH];
			HRESULT hr = SHGetFolderPathA( NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, pDocumentPath );

			if( hr == 0 )
			{
				std::string sDocumentPath = pDocumentPath;

				Tools::ConvertSlashesInWindowsPath( sDocumentPath );

				m_sSaveFolderPath = sDocumentPath;
			}
		}

		if( m_eProjectType >= FZNProjectType::COUNT_PROJECT_TYPES )
		{
			FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "Project type not valid, please set it in the project description. Using \"Application\" by default." );
			m_eProjectType = FZNProjectType::Application;
		}
		
		switch( m_eProjectType )
		{
		case FZNProjectType::Game:
			m_sSaveFolderPath += "/My Games/FaZoN Games";
			break;
		case FZNProjectType::Application:
		default:
			m_sSaveFolderPath += "/FaZoN Apps";
			break;
		};

		std::string sCompletePath = GetSaveFolderPath();
		for( unsigned int iChar = 0; iChar < sCompletePath.size(); ++iChar )
		{
			if( sCompletePath[ iChar ] == '/' )
			{
				std::string sFolderPath = sCompletePath.substr( 0, iChar );
				_CreateFolder( sFolderPath );
			}
		}

		_CreateFolder( sCompletePath );
	}

	void FazonCore::_CreateFolder( const std::string& _sPath )
	{
		errno_t iResult = _access_s( _sPath.c_str(), 0 );

		if( iResult == ENOENT )
		{
			if( _mkdir( _sPath.c_str() ) != 0 )
			{
				char sError[255];
				strerror_s( sError, 254, errno );
				FZN_LOG( "mkdir error on %s: %s", _sPath.c_str(), sError );
			}
		}
		else if( iResult == EACCES )
			FZN_LOG( "Can't access %s", _sPath.c_str() );
	}

	void FazonCore::_ManageEvents()
	{
		while( m_oEvents.empty() == false )
		{
			m_oCurrentEvent = m_oEvents.front();
			m_oEvents.pop();

			if( m_pWindowManager != nullptr )
				m_pWindowManager->ProcessEventsCallBacks();
			else
				m_oCallbacksHolder.ExecuteCallbacks( DataCallbackType::Event );

			if( m_oCurrentEvent.m_eType == Event::eUserEvent )
				CheckNullptrDelete( m_oCurrentEvent.m_pUserData );
		};

		m_oCurrentEvent = Event();
	}

} //namespace fzn
