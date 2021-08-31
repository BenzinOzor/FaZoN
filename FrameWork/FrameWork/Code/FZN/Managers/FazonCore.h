//------------------------------------------------------------------------
//Author : Philippe OFFERMANN
//Date : 1.11.14
//Description : Core management of the main classes
//------------------------------------------------------------------------


#ifndef _ENGINE_H_
#define _ENGINE_H_

class FazonCore;



#define DATAPATH(path)	g_pFZN_Core->GetDataPath( path ).c_str()					//Path to the datas

#include <SFML/Graphics/Rect.hpp>
#include <vector>
#include <Windows.h>
#include <queue>

#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Vector2.hpp>

#include "FZN/Defines.h"
#include "FZN/Tools/Event.h"

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
	class WindowManager;
	typedef void(*CallBack)(void*);					//Function pointer


	class FZN_EXPORT FazonCore : public sf::NonCopyable
	{
	public:
		//Different possible callBacks
		enum CallBacks : INT8
		{
			CB_Update,
			CB_Display,
			CB_Event,
			CB_Nb
		};

		struct DataCallBack
		{
			DataCallBack() : pData( nullptr ), pFct( nullptr ), m_iPriority( 0 ) {}
			DataCallBack( void* _pData, CallBack _pFct, int _iPriority = 0 ) { pData = _pData; pFct = _pFct; m_iPriority = _iPriority; }

			bool operator==( const DataCallBack& _oCallback ) const
			{
				return pData == _oCallback.pData && pFct == _oCallback.pFct && m_iPriority == _oCallback.m_iPriority;
			}

			void* pData;
			CallBack pFct;
			int m_iPriority;
		};

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

		/////////////////SINGLETON MANAGEMENT/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Creation and access on the singleton
		//Return value : BnGCore unic instance
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		static FazonCore* CreateInstance();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Destruction of the singleton
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		static void DestroyInstance();


		/////////////////CORE HANDLING/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Update of the engine components
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void Init( const std::string& _sDataFolderpath = "", const std::string& _lsSaveFolderName = "", bool _bUseFMOD = false );
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

		std::string& GetDataFolder();
		void SetDataFolder(const char* _path);
		std::string GetDataPath(const char* _path);

		void SetSaveFolderName( const std::string& _sFolderName );
		std::string GetSaveFolderPath() const;
		bool FileExists( const std::string& _sFilePath ) const;

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Adds a callBack function to the chosen vector
		//Parameter 1 : Data pointer (class Type)
		//Parameter 2 : Function pointer
		//Parameter 3 : Chosen vector
		//Return value : Function index in the vector
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		int AddCallBack(void* _pData, CallBack _pFct, CallBacks _eCallBackType = CallBacks::CB_Update, int _iWindow = 0, int _iPriority = 0 );
		void RemoveCallBack( void* _pData, CallBack _pFct, CallBacks _eCallBackType = CallBacks::CB_Update, int _iWindow = 0 );

		static bool				CallbackSorter( const DataCallBack& _CallbackA, const DataCallBack& _CallbackB );

		void PushEvent( const Event& _oEvent );
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
		void _ManageEvents();

		/////////////////MEMBER VARIABLES/////////////////

		static FazonCore* m_singleton;							//Unic instance
		
		InputManager*			m_pInputManager;
		DataManager*			m_pDataManager;
		AnimManager*			m_pAnimManager;
		WindowManager*			m_pWindowManager;
		AudioManager*			m_pAudioManager;
		SteeringManager*		m_pSteeringManager;
		AIManager*				m_pAIManager;
		MessageManager*			m_pMessageManager;
		
		std::vector<DataCallBack>	m_lCallBacks;
		std::vector<DataCallBack>	m_oEventsCallback;
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

	typedef std::vector<FazonCore::DataCallBack> CallBacksVector;
} //namespace fzn

extern FZN_EXPORT fzn::FazonCore* g_pFZN_Core;

#endif //_ENGINE_H_
