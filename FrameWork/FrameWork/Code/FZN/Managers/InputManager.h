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
#include "FZN/DataStructure/Variant.h"


namespace fzn
{
	class Animation;

	static constexpr sf::Joystick::Axis JoystickAxisCount{ static_cast<sf::Joystick::Axis>( sf::Joystick::AxisCount ) };
	enum JoystickAxisDirection : uint8_t
	{
		JoystickAxisDirection_Full,
		JoystickAxisDirection_Positive,
		JoystickAxisDirection_Negative,
		JoystickAxisDirection_Count
	};

	using Keys = std::vector< sf::Keyboard::Key >;

	//=========================================================
	//========================ActionKey========================
	//=========================================================

	struct ActionKey
	{
		struct KeyBind
		{
			bool operator==( const KeyBind& _other ) const
			{
				return m_modifiers == _other.m_modifiers && m_key == _other.m_key;
			}

			Keys m_modifiers;
			sf::Keyboard::Key m_key{ sf::Keyboard::Unknown };
		};
		struct AxisInput
		{
			bool operator==( const AxisInput& _other ) const
			{
				return m_axis == _other.m_axis && m_axis_direction == _other.m_axis_direction;
			}

			sf::Joystick::Axis		m_axis				{ JoystickAxisCount };
			JoystickAxisDirection	m_axis_direction	{ JoystickAxisDirection_Full };
		};

		using BindInput = fzn::Variant< KeyBind, sf::Mouse::Button, uint32_t, AxisInput >;
		using Binds = std::vector< BindInput >;

		std::string			m_sName;
		int					m_iCategory{ 0 };
		bool				m_bFullAxis{ true };	// The action is using the complete axis (true) or only half of it.
		Binds				m_oKeyboardBinds;
		Binds				m_oControllerBinds;
	};
	using ActionKeys = std::vector< ActionKey >;


	//=========================================================
	//======================InputManager=======================
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
		enum StatusFlag
		{
			StatusFlag_Pressed		= 1 << Status::Pressed,
			StatusFlag_Down			= 1 << Status::Down,
			StatusFlag_Released		= 1 << Status::Released,
			StatusFlag_Up			= 1 << Status::Up
		};
		typedef sf::Uint8 StatusMask;

		enum BindType
		{
			eKey,
			eMouseButton,
			eJoystickButton,
			eJoystickAxis,
			eNbTypes,
		};
		enum BindTypeFlag
		{
			BindTypeFlag_Key				= 1 << BindType::eKey,
			BindTypeFlag_MouseButton		= 1 << BindType::eMouseButton,
			BindTypeFlag_JoystickButton		= 1 << BindType::eJoystickButton,
			BindTypeFlag_JoystickAxis		= 1 << BindType::eJoystickAxis,

			BindTypeFlag_KeyboardMouse		= BindTypeFlag_Key | BindTypeFlag_MouseButton,
			BindTypeFlag_Controller			= BindTypeFlag_JoystickButton | BindTypeFlag_JoystickAxis,
			BindTypeFlag_All				= BindTypeFlag_KeyboardMouse | BindTypeFlag_Controller
		};
		typedef sf::Uint8 BindTypeMask;

		struct ActionKeyBindReplacementInfo
		{
			void reset()
			{
				m_action_key = nullptr;
				m_action_key_name.clear();
				m_bind_index = 0;
				m_mask = 0;
				m_same_category_only = true;
			}

			ActionKey*		m_action_key{ nullptr };
			std::string		m_action_key_name;
			size_t			m_bind_index{ 0 };
			BindTypeMask	m_mask{ 0 };
			bool			m_same_category_only{ true };
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
			InputManager::Status axisStates[ sf::Joystick::AxisCount ][ JoystickAxisDirection_Count ];	//Array containing the state of each joystick axis (when treated as a button, 0/1 input)
			sf::Joystick::Identification* informations = nullptr;		//Joystick identification (name, vendorID and productID)
			float defaultAxisValues[ sf::Joystick::AxisCount ];			//Default values for each axis (can differ with the product)
			float axesDeadZone = 10.f;									//Value under which positions will not be taken into consideration (abs value)
			INT8 axisCount = 0;											//How many axes are on the controller
			INT8 buttonCount = 0;										//How many buttons are on the controller
			INT8 ID = -1;												//Joystick ID
			bool isConnected = false;									//The joystick is connected (true) or not
			bool m_bNeedCalibration = false;
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
		bool IsKeyPressed( sf::Keyboard::Key _key ) const;
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Key press test (maintained down)
		//Parameter : Concerned key
		//Return value : The key is down (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		bool IsKeyDown( sf::Keyboard::Key _key ) const;
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Release test (transition from Down to Up)
		//Parameter : Concerned key
		//Return value : The key is released (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		bool IsKeyReleased( sf::Keyboard::Key _key ) const;
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Key not pressed test (staying Up)
		//Parameter : Concerned key
		//Return value : The key is up (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		bool IsKeyUp( sf::Keyboard::Key _key ) const;
		Status get_key_state( sf::Keyboard::Key _key ) const;
		bool is_modifier_key( sf::Keyboard::Key _key ) const;

		/////////////////KEY COMBOS/////////////////

		/**
		* @brief Single press test on a keys combo (transition from Up to Down)
		* @param _key_bind : Concerned keys combo
		* @return True if the keys combo is pressed (modifiers pressed/down, key pressed)
		**/
		bool is_key_bind_pressed( const ActionKey::KeyBind& _key_bind ) const;
		/**
		* @brief Press test on a keys combo (maintained down)
		* @param _key_bind : Concerned keys combo
		* @return True if all the modifiers and the key are down
		**/
		bool is_key_bind_down( const ActionKey::KeyBind& _key_bind ) const;
		/**
		* @brief Release test on a keys combo (transition from Down to Up)
		* @param _key_bind : Concerned keys combo
		* @return True if the keys combo is released (one - modifier or key - released, all others down)
		**/
		bool is_key_bind_released( const ActionKey::KeyBind& _key_bind ) const;
		/**
		* @brief Keys combo not pressed test (staying up)
		* @param _key_bind : Concerned keys combo
		* @return True if all the keys are up
		**/
		bool is_key_bind_up( const ActionKey::KeyBind& _key_bind ) const;
		/**
		* @brief Retrieve the state of the keys combo
		* @param _key_bind : Concerned keys combo
		* @return All keys Down: Down | All keys Down/Pressed + one Pressed: Pressed | All keys Up: Up | All keys Up/Released + one Released: Released
		**/
		Status get_key_bind_state( const ActionKey::KeyBind& _key_bind ) const;

		/////////////////ACTIONKEYS TESTS/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Single press test (transition from Up to Down)
		//Parameter : Concerned actionKey
		//Return value : The actionKey is pressed (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		bool IsActionPressed( const char* _action_key, bool _bIgnoreJoystickAxis = false ) const;
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Key press test (maintained down)
		//Parameter : Concerned actionKey
		//Return value : The actionKey is down (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		bool IsActionDown( const char* _action_key, bool _bIgnoreJoystickAxis = false ) const;
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Release test (transition from Down to Up)
		//Parameter : Concerned actionKey
		//Return value : The actionKey is released (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		bool IsActionReleased( const char* _action_key, bool _bIgnoreJoystickAxis = false ) const;
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Key not pressed test (staying Up)
		//Parameter : Concerned actionKey
		//Return value : The actionKey is up (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		bool IsActionUp( const char* _action_key, bool _bIgnoreJoystickAxis = false ) const;
		Status GetActionState( const char* _action_key, bool _bIgnoreJoystickAxis = false ) const;
		float GetActionValue( const char* _action_key ) const;


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
		INT8 IsMousePressed( sf::Mouse::Button _button ) const;
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Button press test (maintained donw)
		//Parameter : Concerned button
		//Return value : The button is down (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		INT8 IsMouseDown( sf::Mouse::Button _button ) const;
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Release test (transition from Down to Up)
		//Parameter : Concerned button
		//Return value : The button is released (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		INT8 IsMouseReleased( sf::Mouse::Button _button ) const;
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Button not pressed test (staying Up)
		//Parameter : Concerned button
		//Return value : The button is up (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		INT8 IsMouseUp( sf::Mouse::Button _button ) const;
		Status get_mouse_button_state( sf::Mouse::Button _button ) const;
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Checks if the mouse wheel is scrolling up
		//Return value : The mouse wheel goes up (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		INT8 IsMouseWheelGoingUp() const;
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Checks if the mouse wheel is scrolling down
		//Return value : The mouse wheel goes down (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		INT8 IsMouseWheelGoingDown() const;
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on the mouse wheel delta
		//Return value : return status Down (delta < 0) or Up (delta > 0), returns "nbStates" if delta = 0
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		Status GetMouseWheelDirection() const;


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
		INT8 IsJoystickButtonPressed( INT8 _id, INT8 _button ) const;
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Button pressed test (maintened down)
		//Parameter 1 : Joystick ID
		//Parameter 2 : Concerned joystick button
		//Return value : The button is pressed (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		INT8 IsJoystickButtonDown( INT8 _id, INT8 _button ) const;
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Release test (transition from Down to Up)
		//Parameter 1 : Joystick ID
		//Parameter 2 : Concerned joystick button
		//Return value : The button is pressed (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		INT8 IsJoystickButtonReleased( INT8 _id, INT8 _button ) const;
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Button not pressed test (staying up)
		//Parameter 1 : Joystick ID
		//Parameter 2 : Concerned joystick button
		//Return value : The button is pressed (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		INT8 IsJoystickButtonUp( INT8 _id, INT8 _button ) const;
		Status get_joystick_button_state( int8_t _id, uint32_t _button ) const;


		/////////////////STICKS FUNCTIONS/////////////////

		bool is_joystick_axis_pressed( int8_t _id, const ActionKey::AxisInput& _axis_input ) const;
		bool is_joystick_axis_down( int8_t _id, const ActionKey::AxisInput& _axis_input ) const;
		bool is_joystick_axis_released( int8_t _id, const ActionKey::AxisInput& _axis_input ) const;
		bool is_joystick_axis_up( int8_t _id, const ActionKey::AxisInput& _axis_input ) const;
		Status get_joystick_axis_state( int8_t _id, const ActionKey::AxisInput& _axis_input ) const;
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Indicates if the given axis is stopped (on its default value)
		//Parameter 1 : Joystick ID
		//Parameter 2 : Concerned joystick axis
		//Return value : The axis is stopped (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		bool IsJoystickAxisStopped( int8_t _id, sf::Joystick::Axis _axis );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Indicates if the axis corresponding to the given action is stopped (on its default value)
		//Parameter 1 : Joystick ID
		//Parameter 2 : Action
		//Return value : The axis is stopped (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		bool IsJoystickAxisStopped( int8_t _id, const char* _action_key );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on the position of a given axis
		//Parameter 1 : Joystick ID
		//Parameter 2 : Concerned joystick axis
		//Return value : Axis position
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		float GetJoystickAxisPosition( INT8 _id, sf::Joystick::Axis _axis ) const;
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on the position of the axis corresponding to an action
		//Parameter 1 : Joystick ID
		//Parameter 2 : Action
		//Return value : Axis position
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		float GetJoystickAxisPosition( INT8 _id, const char* _action_key );


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
		int GetNumberOfConnectedJoysticks() const;
		
		std::string GetActionGlyphString( std::string_view _action_key_name, bool _keyboard, bool _all_keys ) const;
		CustomBitmapGlyph* GetBitmapGlyph( const std::string& _sActionOrKey, bool _bKeyboard, int _iIndex = 0 ) const;
		std::string GetActionKeyString( const std::string& _sActionOrKey, bool _bKeyboard, int _iIndex = 0, bool _bAddBrackets = false ) const;
		const ActionKeys& GetActionKeys() const;
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on an existing actionKey by its name
		//Parameter : Name
		//Return value : Corresponding actionKey
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		const ActionKey* GetActionKey( const std::string& _sActionKey ) const;
		ActionKey* GetActionKey( const std::string& _sActionKey );
		bool replace_action_key_bind( const std::string& _action_key, BindTypeMask _mask, size_t _index, bool _same_category_only = true );
		bool RemoveActionKeyBind( const std::string& _sActionKey, BindType _eBind, uint32_t _uIndex = 0 );
		bool IsWaitingActionKeyBind() const;
		void cancel_action_key_rebind();
		bool IsWaitingInputForType( const BindType& _eBind ) const;
		void BackupActionKeys();
		void RestoreBackupActionKeys();
		void restore_default_action_keys();
		void SaveCustomActionKeysToFile();

		InputSystems GetInputSystem() const;
		void SetInputSystem( InputSystems eSystem );
		void AddInputToScan( sf::Keyboard::Key _eInput );
		void add_input_to_scan( const ActionKey::KeyBind& _key_bind );
		void AddInputToScan( sf::Mouse::Button _eInput );


		bool IsUsingKeyboard() const;

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
		void UpdateDeviceInUse();

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Set a key to the "Pressed" state, fill the last character and pass the Hit boolean to true
		//Paramater : Concerned key
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void SetKeyPressed( sf::Keyboard::Key _key );
		void SetKeyReleased( sf::Keyboard::Key _key );

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Reads the XML file containing corresponding keys to each game action
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void LoadActionKeysFromXML( ActionKeys& _oActionKeysArray, const std::string& _sXMLPath, bool _bCrypted );
		/**
		* @brief Check if there are differences between default and custom action keys and merge them.
		* This can happen if the custom file is created but new keys were added to the default one in the game/application data folder.
		**/
		void _merge_action_keys();

		void _AddKeyToActionKey( ActionKey& _action_key, sf::Keyboard::Key _key );
		void _add_key_bind_to_action_key( ActionKey& _action_key, const std::string& _key_bind );
		void _AddMouseButtonToActionKey( ActionKey& _action_key, sf::Mouse::Button _mouse_button );
		void _AddJoystickButtonToActionKey( ActionKey& _action_key, uint32_t _joystick_button );
		void _AddJoystickAxisToActionKey( ActionKey& _action_key, const ActionKey::AxisInput& _axis_input );
		void _add_action_keys_to_scan_list();
		void _add_currently_pressed_modifier( sf::Keyboard::Key _key );
		void _remove_currently_pressed_modifier( sf::Keyboard::Key _released_key );
		void _sort_key_bind( Keys& _key_bind );

		const ActionKey* _GetActionKey( const ActionKey::KeyBind& _key ) const;
		const ActionKey* _GetActionKey( sf::Mouse::Button _mouse_button ) const;
		const ActionKey* _GetActionKey( uint32_t _joystick_button ) const;
		const ActionKey* _GetActionKey( const ActionKey::AxisInput& _axis_input ) const;
		void _SendActionsEvent( const ActionKey* _pActionKey, Status _eStatus );

		std::string _GetActionKeyString( const std::string& _sActionOrKey, bool _bKeyboard, int _iIndex = 0 ) const;
		std::string _GetDeviceTag( bool _bKeyboard ) const;

		bool _check_keyboard_key_state( sf::Keyboard::Key _key, Status _status_to_check ) const;
		bool _check_keyboard_key_state_flag( sf::Keyboard::Key _key, StatusMask _state_mask ) const;
		bool _check_mouse_button_state( sf::Mouse::Button _button, Status _status_to_check ) const;
		bool _check_joystick_button_state( int8_t _id, uint32_t _button, Status _status_to_check ) const;
		bool _check_joystick_axis_state( int8_t _id, const ActionKey::AxisInput& _axis_input, Status _status_to_check ) const;


		/////////////////CHARS MANAGEMENT/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Returns the corresponding character to a key (FR Keyboard)
		//Parameter : Concerned key
		//Return value : Corresponding character
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		char GetFRKeyChar( const sf::Keyboard::Key& _key ) const;
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Returns the corresponding upper character to a key (FR Keyboard)
		//Parameter : Concerned key
		//Return value : Corresponding character
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		char GetFRKeyUpperChar( const sf::Keyboard::Key& _key ) const;
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Returns the corresponding character to a key (EN Keyboard)
		//Parameter : Concerned key
		//Return value : Corresponding character
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		char GetENKeyChar( const sf::Keyboard::Key& _key ) const;
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Returns the corresponding upper character to a key (EN Keyboard)
		//Parameter : Concerned key
		//Return value : Corresponding character
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		char GetENKeyUpperChar( const sf::Keyboard::Key& _key ) const;
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Returns the corresponding key to a string (FR Keyboard)
		//Parameter : Concerned string
		//Return value : Corresponding key
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		sf::Keyboard::Key StringToFRKey( const std::string& _key ) const;
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Returns the corresponding key to a string (EN Keyboard)
		//Parameter : Concerned string
		//Return value : Corresponding key
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		sf::Keyboard::Key StringToENKey( const std::string& _key ) const;
		std::string KeyToString( const sf::Keyboard::Key& _key ) const;
		std::string key_bind_to_string( const ActionKey::KeyBind& _key_bind ) const;
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Returns the corresponding mouse button to a string
		//Parameter : Concerned string
		//Return value : Corresponding mouse button
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		sf::Mouse::Button StringToMouseButton( const std::string& _button ) const;
		std::string MouseButtonToString( const sf::Mouse::Button& _eButton ) const;
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Returns the corresponding joystick button to a string
		//Parameter : Concerned string
		//Return value : Corresponding joystick button
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		INT8 StringToJoystickButton( const std::string& _button ) const;
		std::string JoystickButtonToString( const INT8& _iButton ) const;
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Returns the corresponding joystick axis to a string
		//Parameter : Concerned string
		//Return value : Corresponding joystick axis
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		ActionKey::AxisInput string_to_axis_input( const std::string& _button ) const;
		std::string axis_input_to_string( const ActionKey::AxisInput& _axis_input ) const;


		/////////////////MEMBER VARIABLES/////////////////

		ActionKeys		m_default_action_keys;					// Default action keys read from the xml in the application data.
		ActionKeys		m_oCustomActionKeys;					// Action keys currently in use in the application, with potentially modified binds, read from the XML in the documents folder.
		ActionKeys		m_oBackupActionKeys;					// Backup vector used in situation when we modify the custom bindings, in an options menu for example.
		ActionKeyBindReplacementInfo	m_oActionKeyBindInfo;

		bool							m_bUsingKeyboard;

		/////////////////KEYBOARD/////////////////

		Status m_keyStates[sf::Keyboard::KeyCount];													//Array containing all key states
		INT8 m_keyBasicStates[sf::Keyboard::KeyCount];												//Array containing all key basic states (up or down)
		char m_cLastChar;																			//Last character entered with the keyboard 
		INT8 m_bKeyboardHit;																		//Indicates if any of the keys has been pressed (true) or not
		char( InputManager::*m_pGetKeyChar[2] )( const sf::Keyboard::Key& _key ) const;				//Function pointer to the key char accessor (FR or EN)
		char( InputManager::*m_pGetUpperKeyChar[2] )( const sf::Keyboard::Key& _key ) const;		//Function pointer to the key upper char accessor (FR or EN)
		sf::Keyboard::Key( InputManager::*m_pStringToKey[2] )( const std::string& _key ) const;		//Function pointer to the key upper char accessor (FR or EN)
		HKL m_keyboardLayout;																		//Container of the keyboard layout
		INT8 m_bIsKeyboardFrench;																	//Indicates if the keyboard layout is French (true) or not
		std::vector< sf::Keyboard::Key > m_currently_pressed_modifiers;

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
		JoystickInfo m_joysticks[sf::Joystick::Count];										//Array containing all the informations on the controllers connected*
		static constexpr float JoystickAxisPressedThreshold = 75.f;

		/////////////////ALTERNATIVE INPUT SYSTEM/////////////////

		InputSystems					m_eInputSystem;										//Current input system (receiving events which needs a window or scaning the given keys)
		std::vector< int >				m_lScannedInputs[2];								//Keys that are being scanned if this system is chosen (ex : no window) (indexes : 0 : Keyboard / 1 : Mouse)
	};
} //namespace fzn

extern FZN_EXPORT fzn::InputManager* g_pFZN_InputMgr;

#endif //_INPUTSMANAGER_H_
