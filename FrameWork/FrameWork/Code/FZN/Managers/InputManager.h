//------------------------------------------------------------------------
//Author : Philippe OFFERMANN
//Date : 25.03.14
//Description : Keyboard inputs manager
//------------------------------------------------------------------------

#ifndef _INPUTSMANAGER_H_
#define _INPUTSMANAGER_H_

#include <basetsd.h>
#include <vector>
#include <wtypes.h>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/Joystick.hpp>

#include "FZN/Defines.h"


namespace fzn
{
	class Animation;

	//=========================================================
	//=======================ActionKey===========================
	//=========================================================

	struct ActionKey
	{
		std::string							m_sName = "";
		int									m_iCategory = 0;
		std::vector< sf::Keyboard::Key >	m_oKeys;
		std::vector< sf::Mouse::Button >	m_oMouseButtons;
		std::vector< INT8 >					m_oJoystickButtons;
		std::vector< sf::Joystick::Axis >	m_oJoystickAxes;
	};


	//=========================================================
	//======================InputManager=========================
	//=========================================================

	class FZN_EXPORT InputManager : public sf::NonCopyable
	{
	public:
		enum InputSystems
		{
			EventSystem,		//Poll events on the window.
			ScanSystem,			//Scan the wanted inputs.
			nbInputSystems
		};

		//Possible states of the keys
		enum Status
		{
			Pressed,
			Down,
			Released,
			Up,
			nbStates
		};

		enum BindType
		{
			eKey,
			eMouseButton,
			eJoystickButton,
			eJoystickAxis,
			eNbTypes,
		};

		struct ActionKeyBindReplacementInfo
		{
			void Reset()
			{
				m_sActionKey = "";
				m_iBindIndex = -1;
				m_uBindTypeMask = 0;
				m_bSameCategoryOnly = true;
			}

			std::string m_sActionKey = "";
			int			m_iBindIndex = -1;
			sf::Uint8	m_uBindTypeMask = 0;
			bool		m_bSameCategoryOnly = true;
		};

		//=========================================================
		//======================JoystickInfo===========================
		//=========================================================

		struct JoystickInfo
		{
			InputManager::Status states[ sf::Joystick::ButtonCount ];	//Array containing the state of each joystick button
			INT8 basicStates[ sf::Joystick::ButtonCount ];				//Array containing all button basic states (up or down)
			float axes[ sf::Joystick::AxisCount ];						//Array containing the state of each joystick axis
			float basicAxes[ sf::Joystick::AxisCount ];					//Array containing all axis basic states 
			sf::Joystick::Identification* informations = nullptr;		//Joystick identification (name, vendorID and productID)
			float defaultAxisValues[ sf::Joystick::AxisCount ];			//Default values for each axis (can differ with the product)
			float axesDeadZone = 10.f;									//Value under which positions will not be taken into consideration (abs value)
			INT8 axisCount = 0;											//How many axes are on the controller
			INT8 buttonCount = 0;										//How many buttons are on the controller
			INT8 ID = -1;												//Joystick ID
			bool isConnected = false;									//The joystick is connected (true) or not
		};


		/////////////////CONSTRUCTOR / DESTRUCTOR/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Default constructor
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		InputManager();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Destructor
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		~InputManager();


		/////////////////OTHER FUNCTIONS/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Key and button states update
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void Update();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Manages a given event to set the right button or key in the arrays
		//Parameter : Event to handle
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void ManageInputEvent( sf::Event& _event );
		void OnEvent();


		/////////////////KEYBOARD/////////////////

		/////////////////KEY STATE TESTS/////////////////
		/////////////////TESTS ON ANY KEY/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Single press test (transition from Up to Down)
		//Parameter : Concerned key
		//Return value : The key is pressed (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		bool IsKeyPressed( sf::Keyboard::Key _key );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Key press test (maintained donw)
		//Parameter : Concerned key
		//Return value : The key is down (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		bool IsKeyDown( sf::Keyboard::Key _key );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Release test (transition from Down to Up)
		//Parameter : Concerned key
		//Return value : The key is released (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		bool IsKeyReleased( sf::Keyboard::Key _key );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Key not pressed test (staying Up)
		//Parameter : Concerned key
		//Return value : The key is up (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		bool IsKeyUp( sf::Keyboard::Key _key );
		Status GetKeyState( sf::Keyboard::Key _oKey );

		/////////////////ACTIONKEYS TESTS/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Single press test (transition from Up to Down)
		//Parameter : Concerned actionKey
		//Return value : The actionKey is pressed (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		bool IsKeyPressed( const char* _actionKey );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Key press test (maintained down)
		//Parameter : Concerned actionKey
		//Return value : The actionKey is down (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		bool IsKeyDown( const char* _actionKey );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Release test (transition from Down to Up)
		//Parameter : Concerned actionKey
		//Return value : The actionKey is released (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		bool IsKeyReleased( const char* _actionKey );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Key not pressed test (staying Up)
		//Parameter : Concerned actionKey
		//Return value : The actionKey is up (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		bool IsKeyUp( const char* _actionKey );
		Status GetKeyState( const char* _actionKey );


		/////////////////ACCESSORS/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on the last entered character
		//Return value : Last character (if there is one)
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		char GetLastChar();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Indicate if any key of the keyboard has been pressed
		//Return value : A key has been pressed (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		INT8 IsKeyboardHit();


		/////////////////MOUSE/////////////////

		/////////////////BUTTON STATE TESTS/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Single press test (transition from Up to Down)
		//Parameter : Concerned button
		//Return value : The button is pressed (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		INT8 IsMousePressed( sf::Mouse::Button _button );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Button press test (maintained donw)
		//Parameter : Concerned button
		//Return value : The button is down (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		INT8 IsMouseDown( sf::Mouse::Button _button );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Release test (transition from Down to Up)
		//Parameter : Concerned button
		//Return value : The button is released (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		INT8 IsMouseReleased( sf::Mouse::Button _button );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Button not pressed test (staying Up)
		//Parameter : Concerned button
		//Return value : The button is up (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		INT8 IsMouseUp( sf::Mouse::Button _button );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Checks if the mouse wheel is scrolling up
		//Return value : The mouse wheel goes up (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		INT8 IsMouseWheelGoingUp();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Checks if the mouse wheel is scrolling down
		//Return value : The mouse wheel goes down (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		INT8 IsMouseWheelGoingDown();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on the mouse wheel delta
		//Return value : return status Down (delta < 0) or Up (delta > 0), returns "nbStates" if delta = 0
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		Status GetMouseWheelDirection();


		/////////////////OTHER FUNCTIONS/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Checks the mouse position compared to the game window
		//Return value : The mouse is in the window (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		INT8 IsMouseInScreen();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Checks the mouse position compared to the game window
		//Parameter : Window in which the test has to be done
		//Return value : The mouse is in the window (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		INT8 IsMouseInScreen( sf::RenderWindow& _window );


		/////////////////MOUSEOVER TESTS/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Checks the mouseover on a sprite
		//Parameter : Concerned sprite
		//Return value : The mouse is over the sprite (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		INT8 IsMouseOver( sf::Sprite& _sprite );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Checks the mouseover on a text
		//Parameter : Concerned text
		//Return value : The mouse is over the text (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		INT8 IsMouseOver( sf::Text& _text );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Checks the mouseover on an animation
		//Parameter : Concerned animation
		//Return value : The mouse is over the animation (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		INT8 IsMouseOver( Animation& _animation );


		/////////////////ACCESSORS/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Indicates if any button of the mouse has been pressed
		//Return value : A button has been pressed (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		INT8 IsMouseHit();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Indicates if the mouse has moved since the last update
		//Return value : The mouse has moved (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		INT8 HasMouseMoved();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Checks if the mouse wheel has been moved
		//Return value : The mouse wheel has moved (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		INT8 HasMouseWheelMoved();


		/////////////////JOYSTICK/////////////////

		/////////////////BUTTONS STATE TESTS/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Single press test (transition from Up to Down)
		//Parameter 1 : Joystick ID
		//Parameter 2 : Concerned joystick button
		//Return value : The button is pressed (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		INT8 IsJoystickButtonPressed( INT8 _id, INT8 _button );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Button pressed test (maintened down)
		//Parameter 1 : Joystick ID
		//Parameter 2 : Concerned joystick button
		//Return value : The button is pressed (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		INT8 IsJoystickButtonDown( INT8 _id, INT8 _button );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Release test (transition from Down to Up)
		//Parameter 1 : Joystick ID
		//Parameter 2 : Concerned joystick button
		//Return value : The button is pressed (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		INT8 IsJoystickButtonReleased( INT8 _id, INT8 _button );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Button not pressed test (staying up)
		//Parameter 1 : Joystick ID
		//Parameter 2 : Concerned joystick button
		//Return value : The button is pressed (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		INT8 IsJoystickButtonUp( INT8 _id, INT8 _button );


		/////////////////STICKS FUNCTIONS/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Indicates if the given axis is stopped (on its default value)
		//Parameter 1 : Joystick ID
		//Parameter 2 : Concerned joystick axis
		//Return value : The axis is stopped (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		INT8 IsJoystickAxisStopped( INT8 _id, sf::Joystick::Axis _axis );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Indicates if the axis corresponding to the given action is stopped (on its default value)
		//Parameter 1 : Joystick ID
		//Parameter 2 : Action
		//Return value : The axis is stopped (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		INT8 IsJoystickAxisStopped( INT8 _id, const char* _action );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on the position of a given axis
		//Parameter 1 : Joystick ID
		//Parameter 2 : Concerned joystick axis
		//Return value : Axis position
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		float GetJoystickAxisPosition( INT8 _id, sf::Joystick::Axis _axis );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on the position of the axis corresponding to an action
		//Parameter 1 : Joystick ID
		//Parameter 2 : Action
		//Return value : Axis position
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		float GetJoystickAxisPosition( INT8 _id, const char* _action );


		/////////////////ACCESSORS / MUTATORS/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Indicates if the joystick is connected
		//Parameter : Joystick ID
		//Return value : The joystick is connected (true) or not (1 if invalid ID)
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		INT8 IsJoystickConnected( INT8 _id );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on the number of buttons of the controller
		//Parameter : Joystick ID
		//Return value : Button count (-1 if invalid ID or joystick not connected)
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		INT8 GetJoystickButtonCount( INT8 _id );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on the number of axes on the controller
		//Parameter : Joystick ID
		//Return value : Axes count (-1 if invalid ID or joystick not connected)
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		INT8 GetJoystickAxisCount( INT8 _id );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on the joystick axis dead zone
		//Parameter : Joystick ID
		//Return value : Axis dead zone (-1.f if invalid ID or joystick not connected)
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		float GetJoystickAxisDeadZone( INT8 _id );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Mutator on the joystick axis dead zone
		//Parameter 1 : Joystick ID
		//Parameter 2 : Axis dead zone
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void SetJoystickAxisDeadZone( INT8 _id, float _deadzone );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on the default value of an axis (when no action is done on the axis)
		//Parameter 1 : Joystick ID
		//Parameter 2 : Joystick axis
		//Return value : Axis default value (-1.f if invalid ID, joystick not connected or invalid axis)
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		float GetJoystickAxisDefaultValue( INT8 _id, sf::Joystick::Axis _axis );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on the joystick identification (name, vendorId,productId)
		//Parameter : Joystick ID
		//Return value : Joystick identification (nullptr if invalid ID or joystick not connected)
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		const sf::Joystick::Identification* GetJoystickIdentification( INT8 _id );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on the joystick name
		//Parameter : Joystick ID
		//Return value : Joystick name (empty string if invalid ID or joystick not connected)
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		const std::string GetJoystickName( INT8 _id );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Indicates if any button on the joystick has been pressed
		//Parameter : Joystick ID
		//Return value : Buttons have been hit (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		INT8 IsJoystickHit();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Indicates if any axis on the joystick has moved
		//Parameter : Joystick ID
		//Return value : An axis has moved (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		INT8 IsJoystickConnected();
		
		CustomBitmapGlyph* GetBitmapGlyph( const std::string& _sActionOrKey, int _iIndex = 0 );
		std::string GetActionGlyphString( const std::string& _sAction, bool _bAllKeys );
		std::string GetActionKeyString( const std::string& _sActionOrKey, int _iIndex = 0 );
		const std::vector< ActionKey >& GetActionKeys() const;
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on an existing actionKey by its name
		//Parameter : Name
		//Return value : Corresponding actionKey
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		const ActionKey* GetActionKey( const std::string& _sActionKey ) const;
		ActionKey* GetActionKey( const std::string& _sActionKey );
		bool ReplaceActionKeyBind( const std::string& _sActionKey, const BindType& _eBind, int _iIndex, bool _bSameCategoryOnly = true );
		bool IsWaitingActionKeyBind() const;
		bool IsWaitingInputForType( const BindType& _eBind ) const;
		void ResetActionKeys();
		void SaveCustomActionKeysToFile();

		InputSystems GetInputSystem() const;
		void SetInputSystem( InputSystems eSystem );
		void AddInputToScan( sf::Keyboard::Key _eInput );
		void AddInputToScan( sf::Mouse::Button _eInput );

		/////////////////MEMBER VARIABLES/////////////////

		INT8 m_defaultJoystick;

	private:
		/////////////////ARRAY INITIALISATIONS/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Keyboard initialisation (default states values)
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void KeyboardInit();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Mouse initialisation (default states values)
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void MouseInit();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Joystick initialisation (default states values)
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void JoystickInit();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Single Joystick initialisation (default states values)
		//Parameter : Joystick id
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void JoystickInit( INT8 _id );
		void ReinitAllDevicesStates();


		/////////////////OTHER FUNCTIONS/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Sets the default values of the joystick axes (which can be different from a product to another)
		//Called when the joystick has not been calibrated and one of its buttons is pushed
		//Parameter : Joystick id
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void CalibrateJoystick( INT8 _id );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Keyboard key states and events update
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void UpdateKeyboard();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Mouse button states and events update
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void UpdateMouse();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Joystick states and events update
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void UpdateJoystick();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Scanned keys states update
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void UpdateScannedInputs();

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Set a key to the "Pressed" state, fill the last character and pass the Hit boolean to true
		//Paramater : Concerned key
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void SetKeyPressed( sf::Keyboard::Key _key );

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Reads the XML file containing corresponding keys to each game action
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void LoadActionKeysFromXML( std::vector< ActionKey >& _oActionKeysArray, const std::string& _sXMLPath );
		
		void _AddKeyToActionKey( ActionKey& _oActionKey, const sf::Keyboard::Key& _eKey );
		void _AddMouseButtonToActionKey( ActionKey& _oActionKey, const sf::Mouse::Button& _eMouseButton );
		void _AddJoystickButtonToActionKey( ActionKey& _oActionKey, INT8 _iJoystickButton );
		void _AddJoystickAxisToActionKey( ActionKey& _oActionKey, const sf::Joystick::Axis& _eJoystickAxis );
		
		bool _SetActionKeyBind_Key( const sf::Keyboard::Key& _eKey );
		bool _SetActionKeyBind_MouseButton( const sf::Mouse::Button& _eMouseButton );
		bool _SetActionKeyBind_JoystickButton( INT8 _iJoystickButton );
		bool _SetActionKeyBind_JoystickAxis( const sf::Joystick::Axis& _eJoystickAxis );

		/////////////////CHARS MANAGEMENT/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Returns the corresponding character to a key (FR Keyboard)
		//Parameter : Concerned key
		//Return value : Corresponding character
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		char GetFRKeyChar( sf::Keyboard::Key _key );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Returns the corresponding upper character to a key (FR Keyboard)
		//Parameter : Concerned key
		//Return value : Corresponding character
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		char GetFRKeyUpperChar( sf::Keyboard::Key _key );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Returns the corresponding character to a key (EN Keyboard)
		//Parameter : Concerned key
		//Return value : Corresponding character
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		char GetENKeyChar( sf::Keyboard::Key _key );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Returns the corresponding upper character to a key (EN Keyboard)
		//Parameter : Concerned key
		//Return value : Corresponding character
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		char GetENKeyUpperChar( sf::Keyboard::Key _key );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Returns the corresponding key to a string (FR Keyboard)
		//Parameter : Concerned string
		//Return value : Corresponding key
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		sf::Keyboard::Key StringToFRKey( std::string& _key );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Returns the corresponding key to a string (EN Keyboard)
		//Parameter : Concerned string
		//Return value : Corresponding key
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		sf::Keyboard::Key StringToENKey( std::string& _key );
		std::string KeyToString( const sf::Keyboard::Key& _key );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Returns the corresponding mouse button to a string
		//Parameter : Concerned string
		//Return value : Corresponding mouse button
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		sf::Mouse::Button StringToMouseButton( std::string& _button );
		std::string MouseButtonToString( const sf::Mouse::Button& _eButton );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Returns the corresponding joystick button to a string
		//Parameter : Concerned string
		//Return value : Corresponding joystick button
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		INT8 StringToJoystickButton( std::string& _button );
		std::string JoystickButtonToString( INT8 _iButton );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Returns the corresponding joystick axis to a string
		//Parameter : Concerned string
		//Return value : Corresponding joystick axis
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		sf::Joystick::Axis StringToJoystickAxis( std::string& _button );
		std::string JoystickAxisToString( const sf::Joystick::Axis& _eAxis );


		/////////////////MEMBER VARIABLES/////////////////

		std::vector< ActionKey >		m_oDefaultActionKeys;										//Array containing all the action keys of the game
		std::vector< ActionKey >		m_oCustomActionKeys;
		ActionKeyBindReplacementInfo	m_oActionKeyBindInfo;

		/////////////////KEYBOARD/////////////////

		Status m_keyStates[sf::Keyboard::KeyCount];											//Array containing all key states
		INT8 m_keyBasicStates[sf::Keyboard::KeyCount];										//Array containing all key basic states (up or down)
		char m_cLastChar;																	//Last character entered with the keyboard 
		INT8 m_bKeyboardHit;																//Indicates if any of the keys has been pressed (true) or not
		char( InputManager::*m_pGetKeyChar[2] )( sf::Keyboard::Key _key );					//Function pointer to the key char accessor (FR or EN)
		char( InputManager::*m_pGetUpperKeyChar[2] )( sf::Keyboard::Key _key );				//Function pointer to the key upper char accessor (FR or EN)
		sf::Keyboard::Key( InputManager::*m_pStringToKey[2] )( std::string& _key );			//Function pointer to the key upper char accessor (FR or EN)
		HKL m_keyboardLayout;																//Container of the keyboard layout
		INT8 m_bIsKeyboardFrench;															//Indicates if the keyboard layout is French (true) or not 

		/////////////////MOUSE/////////////////

		Status m_buttonStates[sf::Mouse::ButtonCount];										//Array containing the state of each mouse button
		INT8 m_buttonBasicStates[sf::Mouse::ButtonCount];									//Array containing all button basic states (up or down)
		INT8 m_bMouseHit;																	//Indicates if any button has been pressed (true) or not
		INT8 m_bMouseMoved;																	//Indicates if the mouse has moved since the last update
		sf::Vector2i m_oldPos;																//Contains the previous position of the mouse
		INT8 m_bMouseWheelMoved;															//Has the mouse wheel been moved
		int m_mouseWheelEventDelta;															//Mouse wheel delta read from the mousewheel event
		int m_mouseWheelDelta;																//Mouse wheel delta (>0 : up / <0 : down)

		/////////////////JOYSTICK/////////////////

		INT8 m_bJoystickHit;																//Indicates if any button has been pressed (true) or not
		INT8 m_bJoystickMoved;																//Indicates if any axis has been moved (true) or not
		JoystickInfo m_joysticks[sf::Joystick::Count];										//Array containing all the informations on the controllers connected

		/////////////////ALTERNATIVE INPUT SYSTEM/////////////////

		InputSystems					m_eInputSystem;										//Current input system (receiving events which needs a window or scaning the given keys)
		std::vector< int >				m_lScannedInputs[2];								//Keys that are being scanned if this system is chosen (ex : no window) (indexes : 0 : Keyboard / 1 : Mouse)
	};
} //namespace fzn

extern FZN_EXPORT fzn::InputManager* g_pFZN_InputMgr;

void FctInputMgrEvent( void* _pData );

#endif //_INPUTSMANAGER_H_
