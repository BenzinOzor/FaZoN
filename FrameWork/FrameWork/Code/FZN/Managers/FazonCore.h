//------------------------------------------------------------------------
//Author : Philippe OFFERMANN
//Date : 1.11.14
//Description : Core management of the main classes
//------------------------------------------------------------------------


#ifndef _ENGINE_H_
#define _ENGINE_H_

class FazonCore;



#define DATAPATH(path)		g_pFZN_Core->GetDataPath( path ).c_str()					//Path to the datas
#define USINGCRYPTEDFILES	g_pFZN_Core->IsUsingCryptedData()

#include <SFML/Graphics/Rect.hpp>
#include <vector>
#include <Windows.h>
#include <queue>

#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Vector2.hpp>

#include "FZN/Defines.h"
#include "FZN/Managers/WindowManager.h"
#include "FZN/Tools/Event.h"
#include "FZN/Tools/DataCallback.h"

namespace sf
{
	class Thread;
}

namespace fzn
{
	class AIManager;
	class AnimManager;
	class AudioManager;
	class DataManager;
	class InputManager;
	class MessageManager;
	class SteeringManager;
	typedef void(*CallBack)(void*);					//Function pointer


	class FZN_EXPORT FazonCore : public sf::NonCopyable
	{
	public:
		enum CoreModules : INT8
		{
			InputModule,
			DataModule,
			AnimModule,
			WindowModule,
			AudioModule,
			SteeringModule,
			MessageModule,
			CoreModulesCount
		};

		enum CoreModuleFlags_ : uint8_t
		{
			CoreModuleFlags_InputModule		= 1 << CoreModules::InputModule,
			CoreModuleFlags_DataModule		= 1 << CoreModules::DataModule,
			CoreModuleFlags_AnimModule		= 1 << CoreModules::AnimModule,
			CoreModuleFlags_WindowModule	= 1 << CoreModules::WindowModule,
			CoreModuleFlags_AudioModule		= 1 << CoreModules::AudioModule,
			CoreModuleFlags_SteeringModule	= 1 << CoreModules::SteeringModule,
			CoreModuleFlags_MessageModule	= 1 << CoreModules::MessageModule,
		};

		using CoreModulesMask = uint8_t;
		static constexpr uint8_t CoreModulesMask_Game			= CoreModuleFlags_InputModule | CoreModuleFlags_DataModule | CoreModuleFlags_AnimModule | CoreModuleFlags_WindowModule | CoreModuleFlags_AudioModule;
		static constexpr uint8_t CoreModulesMask_Application	= CoreModuleFlags_DataModule | CoreModuleFlags_WindowModule;

		struct ProjectDesc
		{
			std::string		m_sName{ "" };

			FZNProjectType	m_eProjectType{ FZNProjectType::COUNT_PROJECT_TYPES };
			CoreModulesMask m_uModules{ m_eProjectType == FZNProjectType::Application ? CoreModulesMask_Application : CoreModulesMask_Game };
			std::string		m_sSaveFolderName{ m_sName };
			bool			m_bUseCryptedData{ false };
			bool			m_bUseFMOD{ false };
			std::string		m_sDataFolderPath{ "../../Data/" };
		};

		/////////////////SINGLETON MANAGEMENT/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Creation and access on the singleton
		//Return value : FazonCore unic instance
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		static FazonCore* CreateInstance();
		static FazonCore* CreateInstance( const ProjectDesc& _rDesc );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Destruction of the singleton
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		static void DestroyInstance();


		/////////////////CORE HANDLING/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Update of the engine components
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void Init( const ProjectDesc& _oDesc );
		void InitModule(CoreModules eModule);
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Update of the engine components
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void Update();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Update of the engine components
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void Deinit();
		void DeinitModule(CoreModules eModule);
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Updates the game loop
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void GameLoop();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Quits the application
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void QuitApplication();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Activates the steering manager
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void ActivateSteeringManager();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Deactivates the steering manager
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void DeactivateSteeringManager();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Activates the ai and message managers
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void ActivateAIModule();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Deactivates the ai and message managers
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void DeactivateAIModule();


		/////////////////CONSOLE HANDLING/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Hides the application console
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void HideConsole();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Shows the application console
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void ShowConsole();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Sets the application console title
		//Parameter : New title
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void ConsoleTitle(const char* _title);
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Sets the application console position
		//Parameter : x and y positions
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void SetConsolePosition( const sf::Vector2i& _position );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Sets the application console position
		//Parameter 1 : x position
		//Parameter 2 : y position
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void SetConsolePosition(int _x, int _y);
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Sets the application console size
		//Parameter : Width and height
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void SetConsoleSize(sf::Vector2i& _size);
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Sets the application console size
		//Parameter 1 : New width
		//Parameter 2 : New height
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void SetConsoleSize(int _width,int _height);


		/////////////////OTHER FUNCTIONS/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Displays a kind message for the user
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void GreetingMessage();
		void OnFirstWindowAdded();


		/////////////////ACCESSORS / MUTATORS/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on the input manager
		//Return value : Input manager
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		InputManager* GetInputManager();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on the data manager
		//Return value : Data manager
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		DataManager* GetDataManager();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on the animation manager
		//Return value : Animation manager
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		AnimManager* GetAnimManager();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on the window manager
		//Return value : Window manager
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		WindowManager* GetWindowManager();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on the audio manager
		//Return value : Audio manager
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		AudioManager* GetAudioManager();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on the steering manager
		//Return value : Steering manager
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		SteeringManager* GetSteeringManager();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on the AI manager
		//Return value : AI manager
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		AIManager* GetAIManager();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on the message manager
		//Return value : Message manager
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		MessageManager* GetMessageManager();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on the application global timer
		//Return value : Elapsed time
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		sf::Time GetGlobalTime();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on the indicator of the members creation
		//Return value : The components are created (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		int GetComponentsNumber();
		bool IsModuleCreated( CoreModules eModule ) const;
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on the indicator of the steering manager creation
		//Return value : The steering manager is created (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		INT8 IsSteeringManagerActivated();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on the indicator of the ai and message managers creation
		//Return value : The ai and message managers are created (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		INT8 IsAIModuleActivated();
		bool IsUsingFMOD() const;

		const std::string& GetProjectName() const;
		bool IsUsingCryptedData() const;

		std::string& GetDataFolder();
		void SetDataFolder(const char* _path);
		std::string GetDataPath(const char* _path);

		void SetSaveFolderName( const std::string& _sFolderName );
		const std::string& GetSaveFolderName() const;
		std::string GetSaveFolderPath() const;
		bool FileExists( const std::string& _sFilePath ) const;

		template< typename CallbackType >
		void AddCallback( CallbackType* _pObject, typename DataCallback<CallbackType>::CallbackFct _pFct, DataCallbackType _eCallbackType, int _iPriority = 0, int _iWindow = -1 )
		{
			if( m_pWindowManager != nullptr )
				m_pWindowManager->AddCallback< CallbackType >( _pObject, _pFct, _eCallbackType, _iPriority, _iWindow );
			else
				m_oCallbacksHolder.AddCallback< CallbackType >( _pObject, _pFct, _eCallbackType, _iPriority );
		}

		template< typename CallbackType >
		void RemoveCallback( CallbackType* _pObject, typename DataCallback<CallbackType>::CallbackFct _pFct, DataCallbackType _eCallbackType, int _iWindow = -1 )
		{
			if( m_pWindowManager != nullptr )
				m_pWindowManager->RemoveCallback< CallbackType >( _pObject, _pFct, _eCallbackType, _iWindow );
			else
				m_oCallbacksHolder.RemoveCallback< CallbackType >( _pObject, _pFct, _eCallbackType );
		}

		void PushEvent( const Event& _oEvent );
		void PushEvent( void* _pUserData );
		const Event& GetEvent() const;


		sf::Thread*				m_oThread;
	private :
		/////////////////CONSTRUCTOR / DESTRUCTOR/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Default constructor, creation of the engine's singletons
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		FazonCore();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Destructor, destruction of the engine's singletons
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		~FazonCore();

		void _TransferCallBacks( bool _bToWindowManager );

		void _CreateSaveFolder();
		void _CreateFolder( const std::string& _sPath );
		void _ManageEvents();

		/////////////////MEMBER VARIABLES/////////////////

		static FazonCore* m_singleton;							//Unic instance
		
		std::string				m_sProjectName;
		FZNProjectType			m_eProjectType;
		bool					m_bUseCryptedData;

		InputManager*			m_pInputManager;
		DataManager*			m_pDataManager;
		AnimManager*			m_pAnimManager;
		WindowManager*			m_pWindowManager;
		AudioManager*			m_pAudioManager;
		SteeringManager*		m_pSteeringManager;
		AIManager*				m_pAIManager;
		MessageManager*			m_pMessageManager;
		
		DataCallbacksHolder		m_oCallbacksHolder;
		INT8					m_bExitApp;

		int						m_iActivatedModulesNbr;		//Number of modules created.

		sf::Clock				m_GlobalTime;					//Application global timer.

		HWND					m_console;						//Application console window handle.

		std::string				m_sDataPath;

		std::string				m_sSaveFolderName;
		std::string				m_sSaveFolderPath;

		std::queue< Event >		m_oEvents;
		Event					m_oCurrentEvent;
	};
} //namespace fzn

extern FZN_EXPORT fzn::FazonCore* g_pFZN_Core;

#endif //_ENGINE_H_
