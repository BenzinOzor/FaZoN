//------------------------------------------------------------------------
//Author : Philippe OFFERMANN
//Date : 25.03.14
//Description : Keyboard INPUTSSs manager
//------------------------------------------------------------------------

#include <fstream>
#include <filesystem>
#include <regex>
#include <algorithm>

#include "FZN/Includes.h"
#include "FZN/Managers/FazonCore.h"
#include "FZN/Managers/DataManager.h"
#include "FZN/Managers/WindowManager.h"
#include "FZN/Display/Animation.h"
#include "FZN/DataStructure/Vector.h"
#include <tinyXML2/tinyxml2.h>
#include "FZN/Managers/InputManager.h"


FZN_EXPORT fzn::InputManager* g_pFZN_InputMgr = nullptr;

namespace fzn
{
	static const std::unordered_map< sf::Keyboard::Key, int > modifiers_order{ { sf::Keyboard::Key::LControl, 0 }, { sf::Keyboard::Key::LAlt, 1 }, { sf::Keyboard::Key::LShift, 2 }, { sf::Keyboard::Key::RControl, 3 }, { sf::Keyboard::Key::RAlt, 4 }, { sf::Keyboard::Key::RShift, 5 } };

	/////////////////CONSTRUCTOR / DESTRUCTOR/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Default constructor
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	InputManager::InputManager()
	{
		g_pFZN_Core->AddCallback< InputManager >( this, &InputManager::OnEvent, fzn::DataCallbackType::Event );

		m_bUsingKeyboard = true;

		KeyboardInit();
		MouseInit();

		sf::Joystick::update();
		JoystickInit();

		if( g_pFZN_Core->IsUsingCryptedData() )
			LoadActionKeysFromXML( m_default_action_keys, DATAPATH( "XMLFiles/actionKeys.cfg" ), true );
		else
			LoadActionKeysFromXML( m_default_action_keys, DATAPATH( "XMLFiles/actionKeys.xml" ), false );

		if( g_pFZN_Core->FileExists( g_pFZN_Core->GetSaveFolderPath() + "/actionKeys.xml" ) )
		{
			LoadActionKeysFromXML( m_oCustomActionKeys, g_pFZN_Core->GetSaveFolderPath() + "/actionKeys.xml", false );

			if( m_oCustomActionKeys.empty() )
				m_oCustomActionKeys = m_default_action_keys;
			else
			{
				_merge_action_keys();
				SaveCustomActionKeysToFile();
			}
		}
		else
		{
			m_oCustomActionKeys = m_default_action_keys;
			SaveCustomActionKeysToFile();
		}

		m_oBackupActionKeys = m_oCustomActionKeys;

		m_eInputSystem = g_pFZN_Core->IsModuleCreated( FazonCore::WindowModule ) ? EventSystem : ScanSystem;

		g_pFZN_InputMgr = this;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Destructor
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	InputManager::~InputManager()
	{
		g_pFZN_InputMgr = nullptr;
	}


	//=========================================================
	//==========================PUBLIC==========================
	//=========================================================

	/////////////////OTHER FUNCTIONS/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Key states update
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void InputManager::Update()
	{
		UpdateScannedInputs();
		UpdateKeyboard();
		UpdateMouse();
		UpdateJoystick();

		UpdateDeviceInUse();
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Get the bindings vector corresponding to the given bind_input
	// bind_input: The SFML input type (Keyboard::Key, Mouse::Button, Joystick::Axis or INT8 (Joystick button))
	// _action_key: The action key from which we want to get the bindings vector
	// Return value: A reference to the corresponding bindings vector (keyboard inputs by default)
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	static ActionKey::Binds& get_input_type_binds( ActionKey& _action_key, const ActionKey::BindInput& _input )
	{
		if( _input.is< ActionKey::AxisInput >() || _input.is< uint32_t >() )
			return _action_key.m_oControllerBinds;

		return _action_key.m_oKeyboardBinds;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Add or replace an input in the given action key
	// bind_input: The input type from SFML
	// _action_keys: The action keys vector to handle (keyboard/mouse or joystick inputs)
	// _action_key_infos: The informations about the action key we're handling
	// _input: The SFML input type value (actual value of keyboard key, mouse button, joystick button or joystick axis)
	// Return value: True if the input has been added/replaced in the given action key
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	static bool set_action_key_bind( ActionKeys& _action_keys, InputManager::ActionKeyBindReplacementInfo& _action_key_infos, ActionKey::BindInput _input )
	{
		if( _action_key_infos.m_action_key == nullptr )
			return false;

		// The bindings of the action key we're working on.
		ActionKey::Binds& binds{ get_input_type_binds( *_action_key_infos.m_action_key, _input ) };

		// If the input entered by the player is already in the bindings vector, we ignore it.
		if( std::ranges::find( binds, _input ) != binds.end() )
			return false;

		if( _action_key_infos.m_bind_index >= binds.size() )
			binds.push_back( _input );
		else
			binds[ _action_key_infos.m_bind_index ] = _input;

		for( fzn::ActionKey& action_key : _action_keys )
		{
			if( action_key.m_sName == _action_key_infos.m_action_key->m_sName )
				continue;

			if( _action_key_infos.m_same_category_only && action_key.m_iCategory != _action_key_infos.m_action_key->m_iCategory )
				continue;

			ActionKey::Binds& action_key_binds{ get_input_type_binds( action_key, _input ) };
			if( auto it_bind = std::ranges::find( action_key_binds, _input ); it_bind != action_key_binds.end() )
			{
				action_key_binds.erase( it_bind );
				break;
			}
		};

		_action_key_infos.reset();
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Manages a given event to set the right button or key in the arrays
	//Parameter : Event to handle
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void InputManager::ManageInputEvent( sf::Event& _event )
	{
		if( _event.type == sf::Event::JoystickConnected )
		{
			JoystickInit( _event.joystickConnect.joystickId );
			FZN_LOG( "Joystick connected - %s (ID : %d)", m_joysticks[ _event.joystickConnect.joystickId ].informations->name.toAnsiString().c_str(), _event.joystickConnect.joystickId );
		}
		else if( _event.type == sf::Event::JoystickDisconnected )
		{
			m_joysticks[ _event.joystickConnect.joystickId ].isConnected = FALSE;
			FZN_LOG( "Joystick disconnected - %s (ID : %d)", m_joysticks[ _event.joystickConnect.joystickId ].informations->name.toAnsiString().c_str(), _event.joystickConnect.joystickId );
		}

		// Handling pressed event for eventual key binds before updating basic states so the event used for the bind doesn't trigger anything else.
		if( IsWaitingActionKeyBind() )
		{
			bool bResetEvent = false;
			bool bBindReplaced = false;
			if( IsWaitingInputForType( BindType::eKey ) )
			{
				if( _event.type == sf::Event::KeyPressed )
				{
					if( is_modifier_key( _event.key.code ) )
					{
						_add_currently_pressed_modifier( _event.key.code );
					}
					// Considering the combo done when a non modifier key is pressed.
					else
					{
						bBindReplaced = set_action_key_bind( m_oCustomActionKeys, m_oActionKeyBindInfo, ActionKey::KeyBind{ m_currently_pressed_modifiers, _event.key.code } );
						bResetEvent = true;
						m_currently_pressed_modifiers.clear();
					}
				}
				// If a key is released, we don't want to put it in the current key combo, so we have to remove it.
				else if( _event.type == sf::Event::KeyReleased )
				{
					_remove_currently_pressed_modifier( _event.key.code );
				}
			}
			else if( IsWaitingInputForType( BindType::eMouseButton ) && _event.type == sf::Event::MouseButtonPressed )
			{
				bBindReplaced = set_action_key_bind( m_oCustomActionKeys, m_oActionKeyBindInfo, _event.mouseButton.button );
				bResetEvent = true;
			}
			else if( IsWaitingInputForType( BindType::eJoystickButton ) && _event.type == sf::Event::JoystickButtonPressed )
			{
				bBindReplaced = set_action_key_bind( m_oCustomActionKeys, m_oActionKeyBindInfo, _event.joystickButton.button );
				bResetEvent = true;
			}
			else if( IsWaitingInputForType( BindType::eJoystickAxis ) && _event.type == sf::Event::JoystickMoved && fabs( _event.joystickMove.position ) > 50.f )
			{
				const JoystickAxisDirection direction( [&]()
				{
					if( m_oActionKeyBindInfo.m_action_key->m_bFullAxis )
						return JoystickAxisDirection_Full;

					if( _event.joystickMove.position - m_joysticks[ m_defaultJoystick ].defaultAxisValues[ _event.joystickMove.axis ] > 0 )
						return JoystickAxisDirection_Positive;

					return JoystickAxisDirection_Negative;
				}() );
				bBindReplaced = set_action_key_bind( m_oCustomActionKeys, m_oActionKeyBindInfo, ActionKey::AxisInput{ _event.joystickMove.axis, direction } );
				bResetEvent = true;
			}

			// Resetting so even the code using events won't get this one.
			if( bResetEvent )
				_event.type = sf::Event::Count;

			if( bBindReplaced )
			{
				g_pFZN_Core->PushEvent( Event::Type::eActionKeyBindDone );
			}

			return;
		}

		if( m_eInputSystem == InputSystems::ScanSystem )
			return;

		switch( _event.type )
		{
		case sf::Event::KeyPressed:
			m_keyBasicStates[ _event.key.code ] = TRUE;
			if( is_modifier_key( _event.key.code ) )
				_add_currently_pressed_modifier( _event.key.code );
			break;
		case sf::Event::MouseButtonPressed:
			m_buttonBasicStates[ _event.mouseButton.button ] = TRUE;
			break;
		case sf::Event::MouseWheelMoved:
			if( _event.mouseWheel.delta > 0 )
				m_mouseWheelEventDelta = 1;
			else if( _event.mouseWheel.delta < 0 )
				m_mouseWheelEventDelta = -1;
			break;
		case sf::Event::JoystickButtonPressed:
			m_joysticks[ _event.joystickButton.joystickId ].basicStates[ _event.joystickButton.button ] = TRUE;
			break;
		case sf::Event::JoystickMoved:
			m_joysticks[ _event.joystickButton.joystickId ].basicAxes[ _event.joystickMove.axis ] = m_joysticks[ _event.joystickButton.joystickId ].axes[ _event.joystickMove.axis ];
			m_joysticks[ _event.joystickButton.joystickId ].axes[ _event.joystickMove.axis ] = _event.joystickMove.position;
			break;
		case sf::Event::KeyReleased:
			m_keyBasicStates[ _event.key.code ] = FALSE;
			if( is_modifier_key( _event.key.code ) )
				_remove_currently_pressed_modifier( _event.key.code );
			break;
		case sf::Event::MouseButtonReleased:
			m_buttonBasicStates[ _event.mouseButton.button ] = FALSE;
			break;
		case sf::Event::JoystickButtonReleased:
			m_joysticks[ _event.joystickButton.joystickId ].basicStates[ _event.joystickButton.button ] = FALSE;
			break;
		};
	}

	void InputManager::OnEvent()
	{
		Event oEvent = g_pFZN_Core->GetEvent();

		if( oEvent.m_eType == Event::Type::eToggleFullScreen )
		{
			ReinitAllDevicesStates();
		}
	}


	/////////////////KEY STATE TESTS/////////////////
	/////////////////TESTS ON ANY KEY/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Single press test (transition from Up to Down)
	//Parameter : Concerned key
	//Return value : The key is pressed (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	bool InputManager::IsKeyPressed( sf::Keyboard::Key _key ) const
	{
		return _check_keyboard_key_state( _key, Status::Pressed );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Key press test (maintained down)
	//Parameter : Concerned key
	//Return value : The key is down (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	bool InputManager::IsKeyDown( sf::Keyboard::Key _key ) const
	{
		return _check_keyboard_key_state( _key, Status::Down );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Release test (transition from Down to Up)
	//Parameter : Concerned key
	//Return value : The key is released (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	bool InputManager::IsKeyReleased( sf::Keyboard::Key _key ) const
	{
		return _check_keyboard_key_state( _key, Status::Released );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Key not pressed test (staying Up)
	//Parameter : Concerned key
	//Return value : The key is up (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	bool InputManager::IsKeyUp( sf::Keyboard::Key _key ) const
	{
		return _check_keyboard_key_state( _key, Status::Up );
	}

	InputManager::Status InputManager::get_key_state( sf::Keyboard::Key _key ) const
	{
		if( _key < sf::Keyboard::KeyCount )
			return m_keyStates[ _key ];

		return Status::Up;
	}


	bool InputManager::is_modifier_key( sf::Keyboard::Key _key ) const
	{
		switch( _key )
		{
			case sf::Keyboard::LControl:
			case sf::Keyboard::LShift:
			case sf::Keyboard::LAlt:
			case sf::Keyboard::RControl:
			case sf::Keyboard::RShift:
			case sf::Keyboard::RAlt:
				return true;
			default:
				return false;
		}
	}

	/**
	* @brief Single press test on a keys combo (transition from Up to Down)
	* @param _key_bind : Concerned keys combo
	* @return True if the keys combo is pressed (modifiers pressed/down, key pressed)
	**/
	bool InputManager::is_key_bind_pressed( const ActionKey::KeyBind& _key_bind ) const
	{
		// if the modifiers of the key bind don't correspond to the currently pressed ones, the action key can't be valid.
		if( _key_bind.m_modifiers != m_currently_pressed_modifiers )
			return false;

		for( sf::Keyboard::Key key : _key_bind.m_modifiers )
		{
			if( _check_keyboard_key_state_flag( key, StatusFlag_Released | StatusFlag_Up ) )
				return false;
		}

		return _check_keyboard_key_state( _key_bind.m_key, Status::Pressed );
	}

	/**
	* @brief Press test on a keys combo (maintained down)
	* @param _key_bind : Concerned keys combo
	* @return True if all the modifiers and the key are down
	**/
	bool InputManager::is_key_bind_down( const ActionKey::KeyBind& _key_bind ) const
	{
		// if the modifiers of the key bind don't correspond to the currently pressed ones, the action key can't be valid.
		if( _key_bind.m_modifiers != m_currently_pressed_modifiers )
			return false;

		for( sf::Keyboard::Key key : _key_bind.m_modifiers )
		{
			if( _check_keyboard_key_state( key, Status::Down ) == false )
				return false;
		}

		return _check_keyboard_key_state( _key_bind.m_key, Status::Down );
	}

	/**
	* @brief Release test on a keys combo (transition from Down to Up)
	* @param _key_bind : Concerned keys combo
	* @return True if the keys combo is released (one - modifier or key - released, all others down)
	**/
	bool InputManager::is_key_bind_released( const ActionKey::KeyBind& _key_bind ) const
	{
		uint8_t number_of_keys_down{ 0 };
		uint8_t number_of_keys_released{ 0 };

		for( sf::Keyboard::Key key : _key_bind.m_modifiers )
		{
			if( _check_keyboard_key_state( key, Status::Down ) )
				++number_of_keys_down;

			if( _check_keyboard_key_state( key, Status::Released ) )
				++number_of_keys_released;
		}

		if( _check_keyboard_key_state( _key_bind.m_key, Status::Down ) )
			++number_of_keys_down;

		if( _check_keyboard_key_state( _key_bind.m_key, Status::Released ) )
			++number_of_keys_released;

		if( number_of_keys_down == _key_bind.m_modifiers.size() && number_of_keys_released == 1 )
			return true;

		return false;
	}

	/**
	* @brief Keys combo not pressed test (staying up)
	* @param _key_bind : Concerned keys combo
	* @return True if all the keys are up
	**/
	bool InputManager::is_key_bind_up( const ActionKey::KeyBind& _key_bind ) const
	{
		for( sf::Keyboard::Key key : _key_bind.m_modifiers )
		{
			if( _check_keyboard_key_state( key, Status::Up ) == false )
				return false;
		}

		return true;
	}

	/**
	* @brief Retrieve the state of the keys combo
	* @param _key_bind : Concerned keys combo
	* @return All keys Down: Down | All keys Down/Pressed + one Pressed: Pressed | All keys Up: Up | All keys Down/Released + one Released: Released
	**/
	fzn::InputManager::Status InputManager::get_key_bind_state( const ActionKey::KeyBind& _key_bind ) const
	{
		const size_t total_number_of_keys{ _key_bind.m_modifiers.size() + 1 };
		uint8_t key_states_numbers[ Status::nbStates ]{};

		for( sf::Keyboard::Key key : _key_bind.m_modifiers )
		{
			switch( get_key_state( key ) )
			{
				case Status::Pressed:
				{
					++key_states_numbers[ Status::Pressed ];
					break;
				}
				case Status::Down:
				{
					++key_states_numbers[ Status::Down ];
					break;
				}
				case Status::Released:
				{
					++key_states_numbers[ Status::Released ];
					break;
				}
				case Status::Up:
				{
					++key_states_numbers[ Status::Up ];
					break;
				}
			};
		}

		switch( get_key_state( _key_bind.m_key ) )
		{
			case Status::Pressed:
			{
				++key_states_numbers[ Status::Pressed ];
				break;
			}
			case Status::Down:
			{
				++key_states_numbers[ Status::Down ];
				break;
			}
			case Status::Released:
			{
				++key_states_numbers[ Status::Released ];
				break;
			}
			case Status::Up:
			{
				++key_states_numbers[ Status::Up ];
				break;
			}
		};

		// As soon as one key is up, the combo can't be used so the key bind is considered Up.
		if( key_states_numbers[ Status::Up ] > 0 )
			return Status::Up;

		// If all keys are maintained down, the keys combo is Down.
		if( key_states_numbers[ Status::Down ] == total_number_of_keys )
			return Status::Down;

		// If all the keys are either down or pressed, the combo is pressed. We don't want any other state.
		if( key_states_numbers[ Status::Down ] + key_states_numbers[ Status::Pressed ] == total_number_of_keys )
			return Status::Pressed;

		// If at least one key is released, the combo is released.
		if( key_states_numbers[ Status::Pressed ] == 0 && key_states_numbers[ Status::Released ] > 0 )
			return Status::Released;

		return Status::Up;
	}

	/////////////////ACTIONKEYS TESTS/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Single press test (transition from Up to Down)
	//Parameter : Concerned actionKey
	//Return value : The actionKey is pressed (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	bool InputManager::IsActionPressed( const char* _action_key, bool _bIgnoreJoystickAxis /*= false*/ ) const
	{
		const ActionKey* action_key = GetActionKey( _action_key );

		if( action_key == nullptr )
			return false;

		static const Overloaded pressed_overloads
		{
			[&]( const ActionKey::KeyBind& _key_bind ) -> bool		{ return is_key_bind_pressed( _key_bind ); },
			[&]( sf::Mouse::Button _button ) -> bool				{ return IsMousePressed( _button ); },
			[&]( uint32_t _joystick_button ) -> bool				{ return IsJoystickButtonPressed( m_defaultJoystick, _joystick_button ); },
			[&]( ActionKey::AxisInput _joystick_axis ) -> bool		{ return is_joystick_axis_pressed( m_defaultJoystick, _joystick_axis ); }
		};

		for( const ActionKey::BindInput& bind_input : action_key->m_oKeyboardBinds )
		{
			if( bind_input.visit<bool>( pressed_overloads ) )
				return true;
		}

		if( m_defaultJoystick < 0 || m_joysticks[ m_defaultJoystick ].isConnected == false )
			return false;

		for( const ActionKey::BindInput& bind_input : action_key->m_oControllerBinds )
		{
			if( bind_input.visit< bool >( pressed_overloads ) )
				return true;
		}

		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Key press test (maintained down)
	//Parameter : Concerned actionKey
	//Return value : The actionKey is down (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	bool InputManager::IsActionDown( const char* _action_key, bool _bIgnoreJoystickAxis /*= false*/ ) const
	{
		const ActionKey* action_key = GetActionKey( _action_key );

		if( action_key == nullptr )
			return false;

		static const Overloaded down_overloads
		{
			[&]( const ActionKey::KeyBind& _key_bind ) -> bool	{ return is_key_bind_down( _key_bind ); },
			[&]( sf::Mouse::Button _button ) -> bool			{ return IsMouseDown( _button ); },
			[&]( uint32_t _joystick_button ) -> bool			{ return IsJoystickButtonDown( m_defaultJoystick, _joystick_button ); },
			[&]( ActionKey::AxisInput _joystick_axis ) -> bool	{ return is_joystick_axis_down( m_defaultJoystick, _joystick_axis ); }
		};

		for( const ActionKey::BindInput& bind_input : action_key->m_oKeyboardBinds )
		{
			if( bind_input.visit<bool>( down_overloads ) )
				return true;
		}

		if( m_defaultJoystick < 0 || m_joysticks[ m_defaultJoystick ].isConnected == false )
			return false;

		for( const ActionKey::BindInput& bind_input : action_key->m_oControllerBinds )
		{
			if( bind_input.visit< bool >( down_overloads ) )
				return true;
		}

		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Release test (transition from Down to Up)
	//Parameter : Concerned actionKey
	//Return value : The actionKey is released (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	bool InputManager::IsActionReleased( const char* _action_key, bool _bIgnoreJoystickAxis /*= false*/ ) const
	{
		const ActionKey* action_key = GetActionKey( _action_key );

		if( action_key == nullptr )
			return false;

		static const Overloaded released_overloads
		{
			[&]( const ActionKey::KeyBind& _key_bind ) -> bool	{ return is_key_bind_released( _key_bind ); },
			[&]( sf::Mouse::Button _button ) -> bool			{ return IsMouseReleased( _button ); },
			[&]( uint32_t _joystick_button ) -> bool			{ return IsJoystickButtonReleased( m_defaultJoystick, _joystick_button ); },
			[&]( ActionKey::AxisInput _joystick_axis ) -> bool	{ return is_joystick_axis_released( m_defaultJoystick, _joystick_axis ); }
		};

		for( const ActionKey::BindInput& bind_input : action_key->m_oKeyboardBinds )
		{
			if( bind_input.visit<bool>( released_overloads ) )
				return true;
		}

		if( m_defaultJoystick < 0 || m_joysticks[ m_defaultJoystick ].isConnected == false )
			return false;

		for( const ActionKey::BindInput& bind_input : action_key->m_oControllerBinds )
		{
			if( bind_input.visit< bool >( released_overloads ) )
				return true;
		}

		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Key not pressed test (staying Up)
	//Parameter : Concerned actionKey
	//Return value : The actionKey is up (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	bool InputManager::IsActionUp( const char* _action_key, bool _bIgnoreJoystickAxis /*= false*/ ) const
	{
		const ActionKey* action_key = GetActionKey( _action_key );

		if( action_key == nullptr )
			return false;

		static const Overloaded up_overloads
		{
			[&]( const ActionKey::KeyBind& _key_bind ) -> bool	{ return is_key_bind_up( _key_bind ); },
			[&]( sf::Mouse::Button _button ) -> bool			{ return IsMouseUp( _button ); },
			[&]( uint32_t _joystick_button ) -> bool			{ return IsJoystickButtonUp( m_defaultJoystick, _joystick_button ); },
			[&]( ActionKey::AxisInput _joystick_axis ) -> bool	{ return is_joystick_axis_up( m_defaultJoystick, _joystick_axis ); }
		};

		for( const ActionKey::BindInput& bind_input : action_key->m_oKeyboardBinds )
		{
			if( bind_input.visit<bool>( up_overloads ) )
				return true;
		}

		if( m_defaultJoystick < 0 || m_joysticks[ m_defaultJoystick ].isConnected == false )
			return false;

		for( const ActionKey::BindInput& bind_input : action_key->m_oControllerBinds )
		{
			if( bind_input.visit< bool >( up_overloads ) )
				return true;
		}

		return false;
	}

	InputManager::Status InputManager::GetActionState( const char* _action_key, bool _bIgnoreJoystickAxis /*= false*/ ) const
	{
		const ActionKey* action_key = GetActionKey( _action_key );

		if( action_key == nullptr )
			return Status::Up;

		static const Overloaded state_overloads
		{
			[&]( const ActionKey::KeyBind& _key_bind ) -> Status	{ return get_key_bind_state( _key_bind ); },
			[&]( sf::Mouse::Button _button ) -> Status				{ return get_mouse_button_state( _button ); },
			[&]( uint32_t _joystick_button ) -> Status				{ return get_joystick_button_state( m_defaultJoystick, _joystick_button ); },
			[&]( ActionKey::AxisInput _joystick_axis ) -> Status	{ return get_joystick_axis_state( m_defaultJoystick, _joystick_axis ); }
		};

		for( const ActionKey::BindInput& bind_input : action_key->m_oKeyboardBinds )
		{
			return bind_input.visit< Status >( state_overloads );
		}

		if( m_defaultJoystick < 0 || m_joysticks[ m_defaultJoystick ].isConnected == false )
			return Status::Up;

		for( const ActionKey::BindInput& bind_input : action_key->m_oControllerBinds )
		{
			return bind_input.visit< Status >( state_overloads );
		}

		return Status::Up;
	}

	float InputManager::GetActionValue( const char* _action_key ) const
	{
		const ActionKey* action_key = GetActionKey( _action_key );

		if( action_key == nullptr )
			return 0.f;

		for( const ActionKey::BindInput& bind_input : action_key->m_oControllerBinds )
		{
			if( const ActionKey::AxisInput* axis_input = bind_input.get_if< ActionKey::AxisInput >() )
			{
				const float axis_value = GetJoystickAxisPosition( m_defaultJoystick, axis_input->m_axis );

				if( axis_value != 0.f )
					return axis_value;
			}
		}

		const Status action_state{ GetActionState( _action_key ) };

		if( action_state == Status::Pressed || action_state == Status::Down )
			return 1.f;

		return 0.f;
	}

	/////////////////ACCESSORS/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on the last entered character
	//Return value : Last character
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	char InputManager::GetLastChar()
	{
		return m_cLastChar;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Indicate if any key of the keyboard has been pressed
	//Return value : A key has been pressed (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	INT8 InputManager::IsKeyboardHit()
	{
		return m_bKeyboardHit;
	}

	/////////////////BUTTON STATE TESTS/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Single press test (transition from Up to Down)
	//Parameter : Concerned button
	//Return value : The button is pressed (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	INT8 InputManager::IsMousePressed( sf::Mouse::Button _button ) const
	{
		return _check_mouse_button_state( _button, Status::Pressed );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Button press test (maintained donw)
	//Parameter : Concerned button
	//Return value : The button is down (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	INT8 InputManager::IsMouseDown( sf::Mouse::Button _button ) const
	{
		return _check_mouse_button_state( _button, Status::Down );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Release test (transition from Down to Up)
	//Parameter : Concerned button
	//Return value : The button is released (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	INT8 InputManager::IsMouseReleased( sf::Mouse::Button _button ) const
	{
		return _check_mouse_button_state( _button, Status::Released );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Button not pressed test (staying Up)
	//Parameter : Concerned button
	//Return value : The button is up (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	INT8 InputManager::IsMouseUp( sf::Mouse::Button _button ) const
	{
		return _check_mouse_button_state( _button, Status::Up );
	}

	InputManager::Status InputManager::get_mouse_button_state( sf::Mouse::Button _button ) const
	{
		if( _button < sf::Mouse::ButtonCount )
			return m_buttonStates[ _button ];

		return Status::Up;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Checks if the mouse wheel is scrolling up
	//Return value : The mouse wheel goes up (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	INT8 InputManager::IsMouseWheelGoingUp() const
	{
		if( m_mouseWheelDelta > 0 )
			return TRUE;
		else return FALSE;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Checks if the mouse wheel is scrolling down
	//Return value : The mouse wheel goes down (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	INT8 InputManager::IsMouseWheelGoingDown() const
	{
		if( m_mouseWheelDelta < 0 )
			return TRUE;
		else return FALSE;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on the mouse wheel delta
	//Return value : return status Down (delta < 0) or Up (delta > 0), returns "nbStates" if delta = 0
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	InputManager::Status InputManager::GetMouseWheelDirection() const
	{
		if( m_mouseWheelDelta > 0 )
			return Status::Up;
		else if( m_mouseWheelDelta < 0 )
			return Status::Down;
		else return Status::nbStates;
	}


	/////////////////OTHER FUNCTIONS/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Checks the mouse position compared to the game window
	//Return value : The mouse is in the window (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	INT8 InputManager::IsMouseInScreen()
	{
		sf::IntRect windowRect = sf::IntRect( sf::Vector2i( 0, 0 ), ( sf::Vector2i )FZN_Window->getSize() );

		return windowRect.contains( sf::Mouse::getPosition( *FZN_Window ) );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Checks the mouse position compared to the game window
	//Parameter : Window in which the test has to be done
	//Return value : The mouse is in the window (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	INT8 InputManager::IsMouseInScreen( sf::RenderWindow& _window )
	{
		sf::IntRect windowRect = sf::IntRect( sf::Vector2i( 0, 0 ), ( sf::Vector2i )_window.getSize() );

		return windowRect.contains( sf::Mouse::getPosition( _window ) );
	}


	/////////////////MOUSEOVER TESTS/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Checks the mouseover on a sprite
	//Parameter : Concerned sprite
	//Return value : The mouse is over the sprite (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	INT8 InputManager::IsMouseOver( sf::Sprite& _sprite )
	{
		sf::Vector2i mousePos( FZN_Window->mapPixelToCoords( sf::Mouse::getPosition( *FZN_Window ) ) );
		sf::FloatRect spriteBounds( _sprite.getGlobalBounds() );
		float fSpriteRight = spriteBounds.left + spriteBounds.width;
		float fSpriteBottom = spriteBounds.top + spriteBounds.height;

		return ( mousePos.x >= spriteBounds.left && mousePos.x <= fSpriteRight && mousePos.y >= spriteBounds.top && mousePos.y <= fSpriteBottom );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Checks the mouseover on a text
	//Parameter : Concerned text
	//Return value : The mouse is over the text (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	INT8 InputManager::IsMouseOver( sf::Text& _text )
	{
		sf::Vector2i mousePos( FZN_Window->mapPixelToCoords( sf::Mouse::getPosition( *FZN_Window ) ) );
		sf::FloatRect textBounds( _text.getGlobalBounds() );
		float fTextRight = textBounds.left + textBounds.width;
		float fTextBottom = textBounds.top + textBounds.height;

		return ( mousePos.x >= textBounds.left && mousePos.x <= fTextRight && mousePos.y >= textBounds.top && mousePos.y <= fTextBottom );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Checks the mouseover on an animation
	//Parameter : Concerned animation
	//Return value : The mouse is over the animation (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	INT8 InputManager::IsMouseOver( Animation& _anim )
	{
		sf::Vector2i mousePos( FZN_Window->mapPixelToCoords( sf::Mouse::getPosition( *FZN_Window ) ) );
		sf::FloatRect spriteBounds( _anim.GetSprite().getGlobalBounds() );
		float fSpriteRight = spriteBounds.left + spriteBounds.width;
		float fSpriteBottom = spriteBounds.top + spriteBounds.height;

		return ( mousePos.x >= spriteBounds.left && mousePos.x <= fSpriteRight && mousePos.y >= spriteBounds.top && mousePos.y <= fSpriteBottom );
	}

	/////////////////ACCESSORS/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Indicates if any button of the mouse has been pressed
	//Return value : A button has been pressed (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	INT8 InputManager::IsMouseHit()
	{
		return m_bMouseHit;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Indicates if the mouse has moved since the last update
	//Return value : The mouse has moved (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	INT8 InputManager::HasMouseMoved()
	{
		return m_bMouseMoved;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Checks if the mouse wheel has been moved
	//Return value : The mouse wheel has moved (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	INT8 InputManager::HasMouseWheelMoved()
	{
		return m_bMouseWheelMoved;
	}


	/////////////////JOYSTICK/////////////////

	/////////////////BUTTONS STATE TESTS/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Single press test (transition from Up to Down)
	//Parameter 1 : Joystick ID
	//Parameter 2 : Concerned joystick button
	//Return value : The button is pressed (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	INT8 InputManager::IsJoystickButtonPressed( INT8 _id, INT8 _button ) const
	{
		return _check_joystick_button_state( _id, _button, Status::Pressed );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Button pressed test (maintened down)
	//Parameter 1 : Joystick ID
	//Parameter 2 : Concerned joystick button
	//Return value : The button is pressed (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	INT8 InputManager::IsJoystickButtonDown( INT8 _id, INT8 _button ) const
	{
		return _check_joystick_button_state( _id, _button, Status::Down );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Release test (transition from Down to Up)
	//Parameter 1 : Joystick ID
	//Parameter 2 : Concerned joystick button
	//Return value : The button is pressed (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	INT8 InputManager::IsJoystickButtonReleased( INT8 _id, INT8 _button ) const
	{
		return _check_joystick_button_state( _id, _button, Status::Released );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Button not pressed test (staying up)
	//Parameter 1 : Joystick ID
	//Parameter 2 : Concerned joystick button
	//Return value : The button is pressed (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	INT8 InputManager::IsJoystickButtonUp( INT8 _id, INT8 _button ) const
	{
		return _check_joystick_button_state( _id, _button, Status::Up );
	}

	InputManager::Status InputManager::get_joystick_button_state( int8_t _id, uint32_t _button ) const
	{
		if( _id < 0 || _id > sf::Joystick::Count || !m_joysticks[ _id ].isConnected )
			return Status::Up;

		if( _button < 0 || _button >= m_joysticks[ _id ].buttonCount )
			return Status::Up;

		return m_joysticks[ _id ].states[ _button ];
	}


	/////////////////STICKS FUNCTIONS/////////////////

	bool InputManager::is_joystick_axis_pressed( int8_t _id, const ActionKey::AxisInput& _axis_input ) const
	{
		return _check_joystick_axis_state( _id, _axis_input, Status::Pressed );
	}

	bool InputManager::is_joystick_axis_down( int8_t _id, const ActionKey::AxisInput& _axis_input ) const
	{
		return _check_joystick_axis_state( _id, _axis_input, Status::Down );
	}

	bool InputManager::is_joystick_axis_released( int8_t _id, const ActionKey::AxisInput& _axis_input ) const
	{
		return _check_joystick_axis_state( _id, _axis_input, Status::Released );
	}

	bool InputManager::is_joystick_axis_up( int8_t _id, const ActionKey::AxisInput& _axis_input ) const
	{
		return _check_joystick_axis_state( _id, _axis_input, Status::Up );
	}

	InputManager::Status InputManager::get_joystick_axis_state( int8_t _id, const ActionKey::AxisInput& _axis_input ) const
	{
		if( _id < 0 || _id > sf::Joystick::Count || m_joysticks[ _id ].isConnected || _axis_input.m_axis >= JoystickAxisCount || _axis_input.m_axis_direction >= JoystickAxisDirection_Count )
			return Status::Up;

		return m_joysticks[ _id ].axisStates[ _axis_input.m_axis ][ _axis_input.m_axis_direction ];
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Indicates if the given axis is stopped (on its default value)
	//Parameter 1 : Joystick ID
	//Parameter 2 : Concerned joystick axis
	//Return value : The axis is stopped (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	bool InputManager::IsJoystickAxisStopped( int8_t _id, sf::Joystick::Axis _axis )
	{
		return GetJoystickAxisPosition( _id, _axis ) != 0.f;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Indicates if the axis corresponding to the given action is stopped (on its default value)
	//Parameter 1 : Joystick ID
	//Parameter 2 : Action
	//Return value : The axis is stopped (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	bool InputManager::IsJoystickAxisStopped( int8_t _id, const char* _action_key )
	{
		const ActionKey* action_key = GetActionKey( _action_key );

		if( action_key == nullptr )
			return 0.f;

		for( const ActionKey::BindInput& bind_input : action_key->m_oControllerBinds )
		{
			float axis_value{ bind_input.visit< float >( [&]( auto&& _bind_input ) -> float
				{
					if constexpr( std::is_same_v< decltype( _bind_input ), ActionKey::AxisInput > )
						return GetJoystickAxisPosition( m_defaultJoystick, _bind_input.m_axis );
					else
						return 0.f;
				} ) };

			if( axis_value != 0.f )
				return false;
		}

		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on the position of a given axis
	//Parameter 1 : Joystick ID
	//Parameter 2 : Concerned joystick axis
	//Return value : Axis position
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	float InputManager::GetJoystickAxisPosition( INT8 _id, sf::Joystick::Axis _axis ) const
	{
		if( _id < 0 || _id > sf::Joystick::Count || !m_joysticks[ _id ].isConnected )
			return 0.f;

		if( _axis < 0 || _axis > m_joysticks[ _id ].axisCount )
			return 0.f;

		if( fabs( m_joysticks[ m_defaultJoystick ].axes[ _axis ] - m_joysticks[ m_defaultJoystick ].defaultAxisValues[ _axis ] ) <= m_joysticks[ m_defaultJoystick ].axesDeadZone )
			return 0.f;

		return m_joysticks[ _id ].axes[ _axis ];
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on the position of the axis corresponding to an action
	//Parameter 1 : Joystick ID
	//Parameter 2 : Action
	//Return value : Axis position
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	float InputManager::GetJoystickAxisPosition( INT8 _id, const char* _action_key )
	{
		const ActionKey* action_key = GetActionKey( _action_key );

		if( action_key == nullptr )
			return 0.f;

		for( const ActionKey::BindInput& bind_input : action_key->m_oControllerBinds )
		{
			if( const ActionKey::AxisInput* axis_input = bind_input.get_if< ActionKey::AxisInput >() )
			{
				const float axis_value = GetJoystickAxisPosition( m_defaultJoystick, axis_input->m_axis );

				if( axis_value != 0.f )
					return axis_value;
			}
		}

		return 0.f;
	}


	/////////////////ACCESSORS / MUTATORS/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Indicates if the joystick is connected
	//Parameter : Joystick ID
	//Return value : The joystick is connected (true) or not (1 if invalid ID)
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	INT8 InputManager::IsJoystickConnected( INT8 _id )
	{
		if( _id < 0 || _id > sf::Joystick::Count )
			return -1;

		return m_joysticks[ _id ].isConnected;
	}

	int InputManager::GetNumberOfConnectedJoysticks() const
	{
		int iNumber = 0;

		for( const JoystickInfo& oJoystick : m_joysticks )
		{
			if( oJoystick.isConnected )
				++iNumber;
		}

		return iNumber;
	}

	std::string InputManager::GetActionGlyphString( std::string_view _action_key_name, bool _keyboard, bool _all_keys ) const
	{
		const ActionKey* action_key = GetActionKey( _action_key_name.data() );

		if( action_key == nullptr )
			return {};

		std::string sDevice = _GetDeviceTag( _keyboard );

		if( sDevice.empty() == false )
			sDevice = Tools::Sprintf( ",%s", sDevice.c_str() );

		const ActionKey::Binds& binds = _keyboard ? action_key->m_oKeyboardBinds : action_key->m_oControllerBinds;

		std::string sGlyphName = _GetActionKeyString( _action_key_name.data(), _keyboard );

		if( sGlyphName.empty() )
			return {};

		std::string sRet = Tools::Sprintf( "%%%%%s%s%%%%", sGlyphName.c_str(), sDevice.c_str() );

		if( binds.size() == 1 || _all_keys == false )
			return sRet;

		for( size_t iBind = 1; iBind < binds.size(); ++iBind )
		{
			sGlyphName = _GetActionKeyString( _action_key_name.data(), _keyboard, iBind );

			if( sGlyphName.empty() )
				continue;

			sRet += Tools::Sprintf( " / %%%%%s%s%%%%", sGlyphName.c_str(), sDevice.c_str() );
		}

		return sRet;
	}

	CustomBitmapGlyph* InputManager::GetBitmapGlyph( const std::string& _sActionOrKey, bool _bKeyboard, int _iIndex /*= 0*/ ) const
	{
		const std::string sGlyphName = _GetActionKeyString( _sActionOrKey, _bKeyboard, _iIndex );

		if( sGlyphName.empty() == false )
			return g_pFZN_DataMgr->GetBitmapGlyph( sGlyphName, _GetDeviceTag( _bKeyboard ) );

		return nullptr;
	}

	std::string InputManager::GetActionKeyString( const std::string& _sActionOrKey, bool _bKeyboard, int _iIndex /*= 0 */, bool _bAddBrackets /*= false*/ ) const
	{
		const std::string sGlyphName = _GetActionKeyString( _sActionOrKey, _bKeyboard, _iIndex );

		if( sGlyphName.empty() == false )
		{
			if( _bAddBrackets )
				return "[" + sGlyphName + "]";
			else
				return sGlyphName;
		}

		return "";
	}

	const ActionKeys& InputManager::GetActionKeys() const
	{
		return m_oCustomActionKeys;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on an existing actionKey by its name
	//Parameter : Name
	//Return value : Corresponding actionKey
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	const ActionKey* InputManager::GetActionKey( const std::string& _sActionKey ) const
	{
		auto& actionKey = std::ranges::find( m_oCustomActionKeys, _sActionKey, &ActionKey::m_sName );

		if( actionKey == m_oCustomActionKeys.end() )
			return nullptr;

		return &(*actionKey);
	}

	ActionKey* InputManager::GetActionKey( const std::string& _sActionKey )
	{
		auto& actionKey = std::ranges::find( m_oCustomActionKeys, _sActionKey, &ActionKey::m_sName );

		if( actionKey == m_oCustomActionKeys.end() )
			return nullptr;

		return &( *actionKey );
	}

	bool InputManager::replace_action_key_bind( const std::string& _action_key, BindTypeMask _mask, size_t _index, bool _same_category_only /*= true*/ )
	{
		ActionKey* action_key_ptr{ GetActionKey( _action_key ) };

		if( action_key_ptr == nullptr || _mask == 0 || _mask > BindTypeFlag_All )
			return false;

		m_oActionKeyBindInfo.reset();

		m_oActionKeyBindInfo.m_action_key = action_key_ptr;
		m_oActionKeyBindInfo.m_mask = _mask;
		m_oActionKeyBindInfo.m_bind_index = _index;
		m_oActionKeyBindInfo.m_same_category_only = _same_category_only;

		return true;
	}

	bool InputManager::RemoveActionKeyBind( const std::string& _sActionKey, BindType _eBind, uint32_t _uIndex /*= 0*/ )
	{
		if( _sActionKey.empty() || _eBind < 0 || _eBind >= BindType::eNbTypes )
			return false;

		auto action_key_it = std::ranges::find( m_oCustomActionKeys, _sActionKey, &fzn::ActionKey::m_sName );
		
		if( action_key_it == m_oCustomActionKeys.end() )
			return false;

		

		switch( _eBind )
		{
			case fzn::InputManager::eKey:
			case fzn::InputManager::eMouseButton:
			{
				if( _uIndex >= action_key_it->m_oKeyboardBinds.size() )
					return false;

				action_key_it->m_oKeyboardBinds.erase( action_key_it->m_oKeyboardBinds.begin() + _uIndex );

				break;
			}
			case fzn::InputManager::eJoystickButton:
			case fzn::InputManager::eJoystickAxis:
			{
				if( _uIndex >= action_key_it->m_oControllerBinds.size() )
					return false;

				action_key_it->m_oControllerBinds.erase( action_key_it->m_oControllerBinds.begin() + _uIndex );
				break;
			}
			default:
				return false;
		}

		return true;
	}

	bool InputManager::IsWaitingActionKeyBind() const
	{
		return m_oActionKeyBindInfo.m_action_key != nullptr && m_oActionKeyBindInfo.m_bind_index >= 0 && m_oActionKeyBindInfo.m_mask != 0;
	}

	void InputManager::cancel_action_key_rebind()
	{
		m_oActionKeyBindInfo.reset();
	}

	bool InputManager::IsWaitingInputForType( const BindType& _eBind ) const
	{
		return ( m_oActionKeyBindInfo.m_mask & 1 << _eBind ) != 0;
	}

	void InputManager::BackupActionKeys()
	{
		m_oBackupActionKeys = m_oCustomActionKeys;
	}

	void InputManager::RestoreBackupActionKeys()
	{
		m_oCustomActionKeys = m_oBackupActionKeys;
	}

	void InputManager::restore_default_action_keys()
	{
		m_oCustomActionKeys = m_default_action_keys;
	}

	void InputManager::SaveCustomActionKeysToFile()
	{
		const std::string file_path = g_pFZN_Core->GetSaveFolderPath() + "/actionKeys.xml";

		tinyxml2::XMLDocument dest_file;

		tinyxml2::XMLNode* actions = dest_file.NewElement( "Actions" );
		dest_file.InsertEndChild( actions );

		for( const fzn::ActionKey& action_key : m_oCustomActionKeys )
		{
			tinyxml2::XMLElement* action{ dest_file.NewElement( "Action" ) };

			action->SetAttribute( "Name", action_key.m_sName.c_str() );
			action->SetAttribute( "Category", action_key.m_iCategory );

			if( action_key.m_bFullAxis == false )
				action->SetAttribute( "FullAxis", action_key.m_bFullAxis );

			for( const ActionKey::BindInput& bind_input : action_key.m_oKeyboardBinds )
			{
				tinyxml2::XMLElement* input = dest_file.NewElement( "Input" );

				bind_input.visit<void>( Overloaded
					{
						[&]( const ActionKey::KeyBind& _key_bind )	{ input->SetAttribute( "Type", "Keyboard" );		input->SetAttribute( "Map", key_bind_to_string( _key_bind ).c_str() ); },
						[&]( sf::Mouse::Button _button )			{ input->SetAttribute( "Type", "Mouse" );			input->SetAttribute( "Map", MouseButtonToString( _button ).c_str() ); },
						[&]( uint32_t _button )						{ input->SetAttribute( "Type", "JoystickButton" );	input->SetAttribute( "Map", JoystickButtonToString( _button ).c_str() ); },
						[&]( ActionKey::AxisInput _axis )			{ input->SetAttribute( "Type", "JoystickAxis" );	input->SetAttribute( "Map", axis_input_to_string( _axis ).c_str() ); }
					} );

				action->InsertEndChild( input );
			}

			for( const ActionKey::BindInput& bind_input : action_key.m_oControllerBinds )
			{
				tinyxml2::XMLElement* input = dest_file.NewElement( "Input" );

				bind_input.visit<void>( Overloaded
					{
						[&]( const ActionKey::KeyBind& _key_bind )	{},
						[&]( sf::Mouse::Button _button )			{},
						[&]( uint32_t _button )						{ input->SetAttribute( "Type", "JoystickButton" );	input->SetAttribute( "Map", JoystickButtonToString( _button ).c_str() ); },
						[&]( ActionKey::AxisInput _axis )			{ input->SetAttribute( "Type", "JoystickAxis" );	input->SetAttribute( "Map", axis_input_to_string( _axis ).c_str() ); }
					} );

				action->InsertEndChild( input );
			}

			actions->InsertEndChild( action );
		}

		tinyxml2::XMLError eError = dest_file.SaveFile( file_path.c_str() );

		if( eError != tinyxml2::XML_SUCCESS )
			FZN_COLOR_LOG( "Custom action keys file save failed. XML Error %d", eError );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on the number of buttons of the controller
	//Parameter : Joystick ID
	//Return value : Button count (-1 if invalid ID or joystick not connected)
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	INT8 InputManager::GetJoystickButtonCount( INT8 _id )
	{
		if( _id < 0 || _id > sf::Joystick::Count || !m_joysticks[ _id ].isConnected )
			return -1;

		return m_joysticks[ _id ].buttonCount;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on the number of axes on the controller
	//Parameter : Joystick ID
	//Return value : Axes count (-1 if invalid ID or joystick not connected)
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	INT8 InputManager::GetJoystickAxisCount( INT8 _id )
	{
		if( _id < 0 || _id > sf::Joystick::Count || !m_joysticks[ _id ].isConnected )
			return -1;

		return m_joysticks[ _id ].axisCount;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on the joystick axis dead zone
	//Parameter : Joystick ID
	//Return value : Axis dead zone (-1.f if invalid ID or joystick not connected)
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	float InputManager::GetJoystickAxisDeadZone( INT8 _id )
	{
		if( _id < 0 || _id > sf::Joystick::Count || !m_joysticks[ _id ].isConnected )
			return -1.f;

		return m_joysticks[ _id ].axesDeadZone;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Mutator on the joystick axis dead zone
	//Parameter 1 : Joystick ID
	//Parameter 2 : Axis dead zone
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void InputManager::SetJoystickAxisDeadZone( INT8 _id, float _deadzone )
	{
		if( _id < 0 || _id > sf::Joystick::Count || !m_joysticks[ _id ].isConnected )
			return;

		if( _deadzone < 0.f )
			return;

		m_joysticks[ _id ].axesDeadZone = _deadzone;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on the default value of an axis (when no action is done on the axis)
	//Parameter 1 : Joystick ID
	//Parameter 2 : Joystick axis
	//Return value : Axis default value (-1.f if invalid ID, joystick not connected or invalid axis)
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	float InputManager::GetJoystickAxisDefaultValue( INT8 _id, sf::Joystick::Axis _axis )
	{
		if( _id < 0 || _id > sf::Joystick::Count || !m_joysticks[ _id ].isConnected )
			return -1.f;

		if( _axis < 0 || _axis > m_joysticks[ _id ].axisCount )
			return -1.f;

		return m_joysticks[ _id ].defaultAxisValues[ _axis ];
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on the joystick identification (name, vendorId,productId)
	//Parameter : Joystick ID
	//Return value : Joystick identification (nullptr if invalid ID or joystick not connected)
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	const sf::Joystick::Identification* InputManager::GetJoystickIdentification( INT8 _id )
	{
		if( _id < 0 || _id > sf::Joystick::Count || !m_joysticks[ _id ].isConnected )
			return nullptr;

		return m_joysticks[ _id ].informations;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on the joystick name
	//Parameter : Joystick ID
	//Return value : Joystick name (empty string if invalid ID or joystick not connected)
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	const std::string InputManager::GetJoystickName( INT8 _id )
	{
		if( _id < 0 || _id > sf::Joystick::Count || !m_joysticks[ _id ].isConnected )
			return "";

		return m_joysticks[ _id ].informations->name.toAnsiString().c_str();
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Indicates if any button on the joystick has been pressed
	//Return value : Buttons have been hit (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	INT8 InputManager::IsJoystickHit()
	{
		return m_bJoystickHit;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Indicates if any axis on the joystick has moved
	//Return value : An axis has moved (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	INT8 InputManager::IsJoystickConnected()
	{
		return m_bJoystickMoved;
	}

	InputManager::InputSystems InputManager::GetInputSystem() const
	{
		return m_eInputSystem;
	}

	void InputManager::SetInputSystem( InputSystems eSystem )
	{
		if( eSystem >= 0 && eSystem < InputSystems::nbInputSystems )
			m_eInputSystem = eSystem;

		if( m_eInputSystem == InputSystems::ScanSystem )
			_add_action_keys_to_scan_list();
	}

	void InputManager::AddInputToScan( sf::Keyboard::Key _eInput )
	{
		if( _eInput < 0 && _eInput >= sf::Keyboard::KeyCount )
			return;

		if( std::ranges::find( m_lScannedInputs[ 0 ], _eInput ) != m_lScannedInputs[ 0 ].end() )
			return;

		m_lScannedInputs[ 0 ].push_back( (int)_eInput );
	}

	void InputManager::AddInputToScan( sf::Mouse::Button _eInput )
	{
		if( _eInput < 0 && _eInput >= sf::Mouse::ButtonCount )
			return;

		if( std::ranges::find( m_lScannedInputs[ 1 ], _eInput ) != m_lScannedInputs[ 1 ].end() )
			return;

		m_lScannedInputs[ 1 ].push_back( (int)_eInput );
	}
	
	void InputManager::add_input_to_scan( const ActionKey::KeyBind& _key_bind )
	{
		for( sf::Keyboard::Key key : _key_bind.m_modifiers )
		{
			AddInputToScan( key );
		}

		AddInputToScan( _key_bind.m_key );
	}

	bool InputManager::IsUsingKeyboard() const
	{
		return m_bUsingKeyboard;
	}

	//=========================================================
	//==========================PRIVATE=========================
	//=========================================================


	/////////////////ARRAY INITIALISATIONS/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Keyboard initialisation (default states values)
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void InputManager::KeyboardInit()
	{
		for( int i = 0; i < sf::Keyboard::KeyCount; ++i )
		{
			m_keyStates[ i ] = Status::Up;
			m_keyBasicStates[ i ] = FALSE;
		}

		m_bKeyboardHit = FALSE;

		m_pGetKeyChar[ 0 ]		= &InputManager::GetENKeyChar;
		m_pGetKeyChar[ 1 ]		= &InputManager::GetFRKeyChar;
		m_pGetUpperKeyChar[ 0 ] = &InputManager::GetENKeyUpperChar;
		m_pGetUpperKeyChar[ 1 ] = &InputManager::GetFRKeyUpperChar;
		m_pStringToKey[ 0 ]		= &InputManager::StringToENKey;
		m_pStringToKey[ 1 ]		= &InputManager::StringToFRKey;

		m_keyboardLayout = GetKeyboardLayout( 0 );
#pragma warning(disable : 4302)
		m_bIsKeyboardFrench = ( PRIMARYLANGID( GetKeyboardLayout( 0 ) ) == LANG_FRENCH );
#pragma warning(default : 4302)
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Mouse initialisation (default states values)
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void InputManager::MouseInit()
	{
		for( int i = 0; i < sf::Mouse::ButtonCount; ++i )
		{
			m_buttonStates[ i ] = Status::Up;
			m_buttonBasicStates[ i ] = FALSE;
		}

		m_bMouseHit = FALSE;
		m_bMouseMoved = FALSE;
		m_oldPos = sf::Mouse::getPosition();
		m_bMouseWheelMoved = FALSE;
		m_mouseWheelDelta = 0;
		m_mouseWheelEventDelta = 0;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Joystick initialisation (default states values)
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void InputManager::JoystickInit()
	{
		m_defaultJoystick = -1;

		for( INT8 joyID = 0; joyID < sf::Joystick::Count; ++joyID )
		{
			m_joysticks[ joyID ].buttonCount = sf::Joystick::getButtonCount( joyID );
			for( INT8 button = 0; button < sf::Joystick::ButtonCount; ++button )
			{
				m_joysticks[ joyID ].states[ button ] = Status::Up;
				m_joysticks[ joyID ].basicStates[ button ] = FALSE;
			}

			m_joysticks[ joyID ].axisCount = 0;

			for( INT8 axis = 0; axis < sf::Joystick::AxisCount; ++axis )
			{
				m_joysticks[ joyID ].axes[ axis ] = 0.f;
				m_joysticks[ joyID ].basicAxes[ axis ] = 0.f;

				if( sf::Joystick::hasAxis( joyID, ( sf::Joystick::Axis )axis ) )
					m_joysticks[ joyID ].axisCount++;
			}

			m_joysticks[ joyID ].ID = joyID;

			if( sf::Joystick::isConnected( joyID ) )
			{
				m_joysticks[ joyID ].isConnected = TRUE;
				m_joysticks[ joyID ].informations = new sf::Joystick::Identification( sf::Joystick::getIdentification( joyID ) );
				FZN_LOG( "Joystick detected : %s (ID : %d)", m_joysticks[ joyID ].informations->name.toAnsiString().c_str(), joyID );

				if( m_defaultJoystick == -1 )
				{
					m_defaultJoystick = joyID;
					FZN_LOG( "This is the default joystick" );
				}

				m_joysticks[ joyID ].m_bNeedCalibration = true;
			}
		}

		m_bJoystickHit = FALSE;
		m_bJoystickMoved = FALSE;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Single Joystick initialisation (default states values)
	//Parameter : Joystick id
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void InputManager::JoystickInit( INT8 _id )
	{
		if( _id < 0 || _id >= sf::Joystick::Count )
			return;

		m_joysticks[ _id ].buttonCount = sf::Joystick::getButtonCount( _id );

		for( INT8 button = 0; button < m_joysticks[ _id ].buttonCount; ++button )
		{
			m_joysticks[ _id ].states[ button ] = Status::Up;
			m_joysticks[ _id ].basicStates[ button ] = FALSE;
		}

		m_joysticks[ _id ].axisCount = 0;

		for( INT8 axis = 0; axis < sf::Joystick::AxisCount; ++axis )
		{
			m_joysticks[ _id ].axes[ axis ] = 0.f;
			m_joysticks[ _id ].basicAxes[ axis ] = 0.f;
			m_joysticks[ _id ].axisStates[ axis ][ JoystickAxisDirection_Full ]		= Status::Up;
			m_joysticks[ _id ].axisStates[ axis ][ JoystickAxisDirection_Positive ]	= Status::Up;
			m_joysticks[ _id ].axisStates[ axis ][ JoystickAxisDirection_Negative ]	= Status::Up;

			if( sf::Joystick::hasAxis( _id, (sf::Joystick::Axis)axis ) )
				m_joysticks[ _id ].axisCount++;
		}

		m_joysticks[ _id ].ID = _id;
		m_joysticks[ _id ].isConnected = TRUE;

		if( m_joysticks[ _id ].informations != nullptr )
		{
			sf::Joystick::Identification tmpID = sf::Joystick::getIdentification( _id );
			if( !( m_joysticks[ _id ].informations->productId == tmpID.productId && m_joysticks[ _id ].informations->vendorId == tmpID.vendorId ) )
			{
				delete m_joysticks[ _id ].informations;
			}
		}

		m_joysticks[ _id ].informations = new sf::Joystick::Identification( sf::Joystick::getIdentification( _id ) );

		m_joysticks[ _id ].m_bNeedCalibration = true;
	}


	void InputManager::ReinitAllDevicesStates()
	{
		for( int i = 0; i < sf::Keyboard::KeyCount; ++i )
		{
			m_keyStates[ i ] = Status::Up;
			m_keyBasicStates[ i ] = FALSE;
		}

		for( int i = 0; i < sf::Mouse::ButtonCount; ++i )
		{
			m_buttonStates[ i ] = Status::Up;
			m_buttonBasicStates[ i ] = FALSE;
		}

		for( INT8 joyID = 0; joyID < sf::Joystick::Count; ++joyID )
		{
			m_joysticks[ joyID ].buttonCount = sf::Joystick::getButtonCount( joyID );
			for( INT8 button = 0; button < sf::Joystick::ButtonCount; ++button )
			{
				m_joysticks[ joyID ].states[ button ] = Status::Up;
				m_joysticks[ joyID ].basicStates[ button ] = FALSE;
			}

			m_joysticks[ joyID ].axisCount = 0;

			for( INT8 axis = 0; axis < sf::Joystick::AxisCount; ++axis )
			{
				m_joysticks[ joyID ].axes[ axis ] = 0.f;
				m_joysticks[ joyID ].basicAxes[ axis ] = 0.f;
				m_joysticks[ joyID ].axisStates[ axis ][ JoystickAxisDirection_Full ]		= Status::Up;
				m_joysticks[ joyID ].axisStates[ axis ][ JoystickAxisDirection_Positive ] = Status::Up;
				m_joysticks[ joyID ].axisStates[ axis ][ JoystickAxisDirection_Negative ] = Status::Up;

				if( sf::Joystick::hasAxis( joyID, (sf::Joystick::Axis)axis ) )
					m_joysticks[ joyID ].axisCount++;
			}
		}
	}

	/////////////////OTHER FUNCTIONS/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Sets the default values of the joystick axes (which can be different from a product to another)
	//Called when the joystick has not been calibrated and one of its buttons is pushed
	//Parameter : Joystick id
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void InputManager::CalibrateJoystick( INT8 _id )
	{
		if( _id < 0 || _id >= sf::Joystick::Count )
			return;

		INT8 defaultValue = FALSE;

		FZN_LOG( "%s - %s (ID : %d)", __FUNCTION__, m_joysticks[ _id ].informations->name.toAnsiString().c_str(), _id );
		for( INT8 axis = 0; axis < sf::Joystick::AxisCount; ++axis )
		{
			if( sf::Joystick::hasAxis( _id, ( sf::Joystick::Axis )axis ) )
			{
				m_joysticks[ _id ].basicAxes[ axis ] = sf::Joystick::getAxisPosition( _id, ( sf::Joystick::Axis )axis );

				if( fabs( m_joysticks[ _id ].basicAxes[ axis ] ) > m_joysticks[ _id ].axesDeadZone )
				{
					m_joysticks[ _id ].axes[ axis ] = m_joysticks[ _id ].basicAxes[ axis ];
					m_joysticks[ _id ].defaultAxisValues[ axis ] = m_joysticks[ _id ].basicAxes[ axis ];
					
					m_joysticks[ _id ].axisStates[ axis ][ JoystickAxisDirection_Full ] = Status::Down;

					if( m_joysticks[ _id ].basicAxes[ axis ] > 0 )
					{
						m_joysticks[ _id ].axisStates[ axis ][ JoystickAxisDirection_Positive ] = Status::Down;
						m_joysticks[ _id ].axisStates[ axis ][ JoystickAxisDirection_Negative ] = Status::Up;
					}
					else
					{
						m_joysticks[ _id ].axisStates[ axis ][ JoystickAxisDirection_Positive ] = Status::Up;
						m_joysticks[ _id ].axisStates[ axis ][ JoystickAxisDirection_Negative ] = Status::Down;
					}
				}
				else 
					defaultValue = TRUE;
			}
			else 
				defaultValue = TRUE;

			if( defaultValue )
			{
				m_joysticks[ _id ].axes[ axis ] = 0.f;
				m_joysticks[ _id ].defaultAxisValues[ axis ] = 0.f;
				m_joysticks[ _id ].axisStates[ axis ][ JoystickAxisDirection_Full ] = Status::Up;
				m_joysticks[ _id ].axisStates[ axis ][ JoystickAxisDirection_Positive ] = Status::Up;
				m_joysticks[ _id ].axisStates[ axis ][ JoystickAxisDirection_Negative ] = Status::Up;
				defaultValue = FALSE;
			}
			FZN_LOG( " > Axis %d default value : %f", axis, m_joysticks[ _id ].defaultAxisValues[ axis ] );
		}

		m_joysticks[ _id ].m_bNeedCalibration = false;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Keyboard key states and events update
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void InputManager::UpdateKeyboard()
	{
		m_bKeyboardHit = FALSE;
		m_cLastChar = '\0';

		for( int i = 0; i < sf::Keyboard::KeyCount; ++i )
		{
			if( m_keyStates[ i ] >= 0 && m_keyStates[ i ] < Status::nbStates )
			{
				if( m_keyBasicStates[ i ] )
				{
					m_bKeyboardHit = TRUE;
					if( m_keyStates[ i ] == Status::Pressed || m_keyStates[ i ] == Status::Down )
						m_keyStates[ i ] = Status::Down;
					else
						SetKeyPressed( ( sf::Keyboard::Key )i );
				}
				else
				{
					if( m_keyStates[ i ] == Status::Pressed || m_keyStates[ i ] == Status::Down )
						SetKeyReleased( (sf::Keyboard::Key)i );
					else
						m_keyStates[ i ] = Status::Up;
				}
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Mouse button states and events update
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void InputManager::UpdateMouse()
	{
		m_bMouseHit = FALSE;
		m_bMouseMoved = FALSE;
		m_bMouseWheelMoved = FALSE;
		m_mouseWheelDelta = 0;

		if( m_oldPos != sf::Mouse::getPosition() )
		{
			m_oldPos = sf::Mouse::getPosition();
			m_bMouseMoved = TRUE;
		}

		for( int i = 0; i < sf::Mouse::ButtonCount; ++i )
		{
			if( m_buttonStates[ i ] >= 0 && m_buttonStates[ i ] <= 3 )
			{
				if( m_buttonBasicStates[ i ] )
				{
					m_bMouseHit = TRUE;
					if( m_buttonStates[ i ] == Status::Pressed || m_buttonStates[ i ] == Status::Down )
						m_buttonStates[ i ] = Status::Down;
					else
					{
						m_buttonStates[ i ] = Status::Pressed;
						_SendActionsEvent( _GetActionKey( (sf::Mouse::Button)i ), Status::Pressed );
					}
				}
				else
				{
					if( m_buttonStates[ i ] == Status::Pressed || m_buttonStates[ i ] == Status::Down )
					{
						m_buttonStates[ i ] =  Status::Released;
						_SendActionsEvent( _GetActionKey( (sf::Mouse::Button)i ), Status::Released );
					}
					else
						m_buttonStates[ i ] = Status::Up;
				}
			}
		}

		if( m_mouseWheelEventDelta != 0 )
		{
			m_bMouseWheelMoved = TRUE;
			m_mouseWheelDelta = m_mouseWheelEventDelta;
			m_mouseWheelEventDelta = 0;
		}
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Joystick states and events update
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void InputManager::UpdateJoystick()
	{
		m_bJoystickHit = FALSE;
		m_bJoystickMoved = FALSE;
		for( INT8 joyID = 0; joyID < sf::Joystick::Count; ++joyID )
		{
			if( sf::Joystick::isConnected( joyID ) )
			{
				for( INT8 button = 0; button < sf::Joystick::ButtonCount; ++button )
				{
					if( m_joysticks[ joyID ].states[ button ] >= 0 && m_joysticks[ joyID ].states[ button ] <= 3 )
					{
						if( m_joysticks[ joyID ].basicStates[ button ] )
						{
							m_joysticks[ joyID ].states[ button ] = ( m_joysticks[ joyID ].states[ button ] == Status::Pressed || m_joysticks[ joyID ].states[ button ] == Status::Down ) ? Status::Down : Status::Pressed;
							m_bJoystickHit = TRUE;

							/*if( m_defaultJoystick == joyID && m_joysticks[ joyID ].states[ button ] == Status::Pressed )
								FZN_LOG( "Button %d", button );*/

							if( m_joysticks[ joyID ].m_bNeedCalibration )
								CalibrateJoystick( joyID );
						}
						else
							m_joysticks[ joyID ].states[ button ] = ( m_joysticks[ joyID ].states[ button ] == Status::Pressed || m_joysticks[ joyID ].states[ button ] == Status::Down ) ? Status::Released : Status::Up;
					}
				}

				for( INT8 axis = 0; axis < sf::Joystick::AxisCount; ++axis )
				{
					if( m_joysticks[ joyID ].defaultAxisValues != nullptr && m_joysticks[ joyID ].axes[ axis ] != m_joysticks[ joyID ].defaultAxisValues[ axis ] && fabs( m_joysticks[ joyID ].axes[ axis ] ) > m_joysticks[ joyID ].axesDeadZone )
					{
						m_bJoystickMoved = TRUE;

						const float fAxisPos = m_joysticks[ m_defaultJoystick ].axes[ axis ];

						Status eState = m_joysticks[ joyID ].axisStates[ axis ][ JoystickAxisDirection_Full ];

						if( fabs( fAxisPos ) >= JoystickAxisPressedThreshold )
							m_joysticks[ joyID ].axisStates[ axis ][ JoystickAxisDirection_Full ] = ( eState == Status::Pressed || eState == Status::Down ) ? Status::Down : Status::Pressed;
						else
							m_joysticks[ joyID ].axisStates[ axis ][ JoystickAxisDirection_Full ] = ( eState == Status::Pressed || eState == Status::Down ) ? Status::Released : Status::Up;


						eState = m_joysticks[ joyID ].axisStates[ axis ][ JoystickAxisDirection_Positive ];

						if( fAxisPos >= JoystickAxisPressedThreshold )
							m_joysticks[ joyID ].axisStates[ axis ][ JoystickAxisDirection_Positive ] = ( eState == Status::Pressed || eState == Status::Down ) ? Status::Down : Status::Pressed;
						else
							m_joysticks[ joyID ].axisStates[ axis ][ JoystickAxisDirection_Positive ] = ( eState == Status::Pressed || eState == Status::Down ) ? Status::Released : Status::Up;


						eState = m_joysticks[ joyID ].axisStates[ axis ][ JoystickAxisDirection_Negative ];

						if( fAxisPos <= -JoystickAxisPressedThreshold )
							m_joysticks[ joyID ].axisStates[ axis ][ JoystickAxisDirection_Negative ] = ( eState == Status::Pressed || eState == Status::Down ) ? Status::Down : Status::Pressed;
						else
							m_joysticks[ joyID ].axisStates[ axis ][ JoystickAxisDirection_Negative ] = ( eState == Status::Pressed || eState == Status::Down ) ? Status::Released : Status::Up;
					}
					else
					{
						Status eState = m_joysticks[ joyID ].axisStates[ axis ][ JoystickAxisDirection_Full ];
						m_joysticks[ joyID ].axisStates[ axis ][ JoystickAxisDirection_Full ] = ( eState == Status::Pressed || eState == Status::Down ) ? Status::Released : Status::Up;


						eState = m_joysticks[ joyID ].axisStates[ axis ][ JoystickAxisDirection_Positive ];
						m_joysticks[ joyID ].axisStates[ axis ][ JoystickAxisDirection_Positive ] = ( eState == Status::Pressed || eState == Status::Down ) ? Status::Released : Status::Up;


						eState = m_joysticks[ joyID ].axisStates[ axis ][ JoystickAxisDirection_Negative ];
						m_joysticks[ joyID ].axisStates[ axis ][ JoystickAxisDirection_Negative ] = ( eState == Status::Pressed || eState == Status::Down ) ? Status::Released : Status::Up;
					}

					m_joysticks[ joyID ].basicAxes[ axis ] = m_joysticks[ joyID ].axes[ axis ];
				}
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Scanned keys states update
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void InputManager::UpdateScannedInputs()
	{
		if( m_eInputSystem == InputSystems::EventSystem )
			return;

		int iScannedInputSize = 0;
		INT8* pInputList[ 2 ] = { m_keyBasicStates,m_buttonBasicStates };
		std::vector< int >* pScannedInputs = nullptr;
		int iCurrentKey = sf::Keyboard::KeyCount;

		for( int iInputType = 0; iInputType < 2; ++iInputType )
		{
			iScannedInputSize = m_lScannedInputs[ iInputType ].size();
			pScannedInputs = &m_lScannedInputs[ iInputType ];

			for( int iInput = 0; iInput < iScannedInputSize; ++iInput )
			{
				iCurrentKey = ( *pScannedInputs )[ iInput ];

				if( iInputType == 0 )
				{
					const auto current_key{ static_cast< sf::Keyboard::Key >( iCurrentKey ) };

					if( sf::Keyboard::isKeyPressed( current_key ) )
					{
						if( pInputList[ iInputType ][ iCurrentKey ] == false && is_modifier_key( current_key ) )
							_add_currently_pressed_modifier( current_key );

						pInputList[ iInputType ][ iCurrentKey ] = TRUE;
					}
					else
					{
						if( pInputList[ iInputType ][ iCurrentKey ] && is_modifier_key( current_key ) )
							_remove_currently_pressed_modifier( current_key );

						pInputList[ iInputType ][ iCurrentKey ] = FALSE;
					}
				}
				else
					pInputList[ iInputType ][ iCurrentKey ] = sf::Mouse::isButtonPressed( ( sf::Mouse::Button )iCurrentKey ) == true;
			}
		}
	}

	void InputManager::UpdateDeviceInUse()
	{
		const bool bUsingKeyboard = m_bUsingKeyboard;

		if( m_bKeyboardHit || m_bMouseHit || m_bMouseWheelMoved )
		{
			m_bUsingKeyboard = true;
		}

		if( m_bJoystickHit || m_bJoystickMoved )
		{
			m_bUsingKeyboard = false;
		}

		if( bUsingKeyboard != m_bUsingKeyboard )
		{
			Event oEvent( Event::eChangedDevice );
			oEvent.m_oChangedDevice.m_bUsingKeyboard = m_bUsingKeyboard;

			g_pFZN_Core->PushEvent( oEvent );
		}
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Set a key to the "Pressed" state, fill the last character and pass the Hit boolean to true
	//Paramater : Concerned key
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void InputManager::SetKeyPressed( sf::Keyboard::Key _key )
	{
		m_keyStates[ _key ] = Status::Pressed;
		m_cLastChar = ( sf::Keyboard::isKeyPressed( sf::Keyboard::LShift ) || sf::Keyboard::isKeyPressed( sf::Keyboard::RShift ) ) ? ( this->*m_pGetUpperKeyChar[ m_bIsKeyboardFrench ] )( _key ) : ( this->*m_pGetKeyChar[ m_bIsKeyboardFrench ] )( _key );

		Event oEvent( Event::eKeyPressed );
		oEvent.m_oKeyPressed.m_eKey = _key;
		oEvent.m_oKeyPressed.m_cKeyChar = m_cLastChar;

		g_pFZN_Core->PushEvent( oEvent );

		_SendActionsEvent( _GetActionKey( _key ), Status::Pressed );
	}

	void InputManager::SetKeyReleased( sf::Keyboard::Key _key )
	{
		m_keyStates[ _key ] = Status::Released;
		_SendActionsEvent( _GetActionKey( _key ), Status::Released );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Reads the XML file containing corresponding keys to each game action
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void InputManager::LoadActionKeysFromXML( ActionKeys& _oActionKeysArray, const std::string& _sXMLPath, bool _bCrypted )
	{
		tinyxml2::XMLDocument File;

		if( g_pFZN_DataMgr->LoadXMLFile( File, _sXMLPath, _bCrypted ) )
		{
			FZN_COLOR_LOG( DBG_MSG_COL_RED, "Failure : %s (%s)", File.ErrorName(), File.ErrorStr() );
			return;
		}

		_oActionKeysArray.clear();
		tinyxml2::XMLElement* Action = File.FirstChildElement( "Actions" );

		Action = Action->FirstChildElement( "Action" );
		tinyxml2::XMLElement* ActionInput;

		std::string mapedKey;

		while( Action )
		{
			ActionKey oNewActionKey;
			oNewActionKey.m_sName = Tools::XMLStringAttribute( Action, "Name" );
			oNewActionKey.m_iCategory = Action->IntAttribute( "Category" );
			oNewActionKey.m_bFullAxis = Action->BoolAttribute( "FullAxis", true );

			ActionInput = Action->FirstChildElement( "Input" );

			while( ActionInput )
			{
				mapedKey = Tools::XMLStringAttribute( ActionInput, "Map" );

				if( mapedKey.empty() == false )
				{
					if( Tools::XMLStringAttribute( ActionInput, "Type" ) == "Keyboard" )
						_add_key_bind_to_action_key( oNewActionKey, mapedKey );
					else if( Tools::XMLStringAttribute( ActionInput, "Type" ) == "Mouse" )
						_AddMouseButtonToActionKey( oNewActionKey, StringToMouseButton( mapedKey ) );
					else if( Tools::XMLStringAttribute( ActionInput, "Type" ) == "JoystickButton" )
						_AddJoystickButtonToActionKey( oNewActionKey, StringToJoystickButton( mapedKey ) );
					else if( Tools::XMLStringAttribute( ActionInput, "Type" ) == "JoystickAxis" )
					{
						const ActionKey::AxisInput axis_input = string_to_axis_input( mapedKey );

						_AddJoystickAxisToActionKey( oNewActionKey, axis_input );
					}
				}

				ActionInput = ActionInput->NextSiblingElement();
			}

			_oActionKeysArray.push_back( oNewActionKey );
			Action = Action->NextSiblingElement();
		}
	}

	/**
	* @brief Check if there are differences between default and custom action keys and merge them.
	* This can happen if the custom file is created but new keys were added to the default one in the game/application data folder.
	**/
	void InputManager::_merge_action_keys()
	{
		std::erase_if( m_oCustomActionKeys, [&]( const ActionKey& _action_key )
			{
				return std::ranges::find( m_default_action_keys, _action_key.m_sName, &ActionKey::m_sName ) == m_default_action_keys.end();
			} );

		// We compare the two vectors.
		for( uint32_t action_key_id{ 0 }; action_key_id < m_default_action_keys.size(); ++action_key_id )
		{
			const ActionKey& action_key{ m_default_action_keys[ action_key_id ] };
			auto it_action_key = std::ranges::find( m_oCustomActionKeys, action_key.m_sName, &ActionKey::m_sName );

			// If the action is in the custom vector, we do nothing. Even if the bindings are empty, we can't tell if the backup action is new, or if the user simply removed the bind from the action.
			if( it_action_key != m_oCustomActionKeys.end() )
				continue;

			// If the action is not in the custom vector, we add it.
			m_oCustomActionKeys.emplace( m_oCustomActionKeys.begin() + action_key_id, action_key );
		}
		// @todo Remove unused bindings.
	}

	void InputManager::_AddKeyToActionKey( ActionKey& _action_key, sf::Keyboard::Key _key )
	{
		if( _key >= sf::Keyboard::Key::KeyCount )
			return;

		ActionKey::KeyBind new_combo{ .m_key = _key };

		if( std::ranges::find( _action_key.m_oKeyboardBinds, ActionKey::BindInput{ new_combo } ) != _action_key.m_oKeyboardBinds.end() )
			return;

		_action_key.m_oKeyboardBinds.push_back( new_combo );
	}

	void InputManager::_add_key_bind_to_action_key( ActionKey& _action_key, const std::string& _key_bind )
	{
		std::regex reg( "\\w+" );
		std::smatch matches;
		std::regex_match( _key_bind, matches, reg );

		ActionKey::KeyBind new_combo;
		std::string::const_iterator text_iter = _key_bind.cbegin();

		while( std::regex_search( text_iter, _key_bind.end(), matches, reg ) )
		{
			sf::Keyboard::Key key = ( this->*m_pStringToKey[ m_bIsKeyboardFrench ] )( std::string( matches[ 0 ].first, matches[ 0 ].second ) );

			if( key == sf::Keyboard::Unknown )
			{
				FZN_COLOR_LOG( DBG_MSG_COL_RED, "Invalid key bind : %s", _key_bind.c_str() );
				return;
			}

			if( is_modifier_key( key ) )
				new_combo.m_modifiers.push_back( key );
			else
				new_combo.m_key = key;

			text_iter = matches[ 0 ].second;
		}

		_sort_key_bind( new_combo.m_modifiers );

		if( std::ranges::find( _action_key.m_oKeyboardBinds, ActionKey::BindInput{ new_combo } ) != _action_key.m_oKeyboardBinds.end() )
			return;

		_action_key.m_oKeyboardBinds.push_back( new_combo );
	}

	void InputManager::_AddMouseButtonToActionKey( ActionKey& _action_key, sf::Mouse::Button _mouse_button )
	{
		if( _mouse_button >= sf::Mouse::Button::ButtonCount )
			return;

		if( std::ranges::find( _action_key.m_oKeyboardBinds, ActionKey::BindInput{ _mouse_button } ) != _action_key.m_oKeyboardBinds.end() )
			return;

		_action_key.m_oKeyboardBinds.push_back( _mouse_button );
	}

	void InputManager::_AddJoystickButtonToActionKey( ActionKey& _action_key, uint32_t _joystick_button )
	{
		if( _joystick_button >= sf::Joystick::ButtonCount )
			return;

		if( std::ranges::find( _action_key.m_oControllerBinds, ActionKey::BindInput{ _joystick_button } ) != _action_key.m_oControllerBinds.end() )
			return;

		_action_key.m_oControllerBinds.push_back( _joystick_button );
	}

	void InputManager::_AddJoystickAxisToActionKey( ActionKey& _action_key, const ActionKey::AxisInput& _axis_input )
	{
		if( _axis_input.m_axis >= sf::Joystick::AxisCount )
			return;

		if( std::ranges::find( _action_key.m_oControllerBinds, ActionKey::BindInput{ _axis_input } ) != _action_key.m_oControllerBinds.end() )
			return;

		_action_key.m_oControllerBinds.push_back( _axis_input );
	}

	void InputManager::_add_action_keys_to_scan_list()
	{
		for( const ActionKey& action_key : m_oCustomActionKeys )
		{
			for( const ActionKey::BindInput& bind_input : action_key.m_oKeyboardBinds )
			{
				bind_input.visit<void>( Overloaded
					{
						[&]( const ActionKey::KeyBind& _key_bind ) { add_input_to_scan( _key_bind ); },
						[&]( sf::Mouse::Button _button ) { AddInputToScan( _button ); },
						[&]( uint32_t _button ) {},
						[&]( ActionKey::AxisInput _axis ) {}
					} );
			}
		}
	}

	void InputManager::_add_currently_pressed_modifier( sf::Keyboard::Key _key )
	{
		if( is_modifier_key( _key ) == false || std::ranges::find( m_currently_pressed_modifiers, _key ) != m_currently_pressed_modifiers.end() )
			return;

		m_currently_pressed_modifiers.push_back( _key );
		_sort_key_bind( m_currently_pressed_modifiers );
	}

	void InputManager::_remove_currently_pressed_modifier( sf::Keyboard::Key _released_key )
	{
		if( is_modifier_key( _released_key ) == false )
			return;

		std::erase_if( m_currently_pressed_modifiers, [_released_key]( const sf::Keyboard::Key& _key )
		{
			return _key == _released_key;
		} );
	}

	void InputManager::_sort_key_bind( Keys& _key_bind )
	{
		std::ranges::sort( _key_bind, [&]( sf::Keyboard::Key _a, sf::Keyboard::Key _b )
		{
			const bool key_a_sorted = modifiers_order.count( _a ) > 0;
			const bool key_b_sorted = modifiers_order.count( _b ) > 0;

			if( key_a_sorted == false && key_b_sorted == false )
				return _a < _b;

			if( key_a_sorted == false )
				return false;

			if( key_b_sorted == false )
				return true;

			return modifiers_order.at( _a ) < modifiers_order.at( _b );
		});
	}

	const ActionKey* InputManager::_GetActionKey( const ActionKey::KeyBind& _key_bind ) const
	{
		for( const ActionKey& oActionKey : m_oCustomActionKeys )
		{
			for( const ActionKey::BindInput& rBind : oActionKey.m_oKeyboardBinds )
			{
				if( rBind == _key_bind )
					return &oActionKey;
			}
		}

		return nullptr;
	}

	const ActionKey* InputManager::_GetActionKey( sf::Mouse::Button _mouse_button ) const
	{
		for( const ActionKey& oActionKey : m_oCustomActionKeys )
		{
			for( const ActionKey::BindInput& rBind : oActionKey.m_oKeyboardBinds )
			{
				if( rBind == _mouse_button )
					return &oActionKey;
			}
		}

		return nullptr;
	}

	const ActionKey* InputManager::_GetActionKey( uint32_t _joystick_button ) const
	{
		for( const ActionKey& oActionKey : m_oCustomActionKeys )
		{
			for( const ActionKey::BindInput& rBind : oActionKey.m_oControllerBinds )
			{
				if( rBind == _joystick_button )
				{
					return &oActionKey;
				}
			}
		}

		return nullptr;
	}

	const ActionKey* InputManager::_GetActionKey( const ActionKey::AxisInput& _axis_input ) const
	{
		for( const ActionKey& oActionKey : m_oCustomActionKeys )
		{
			for( const ActionKey::BindInput& rBind : oActionKey.m_oControllerBinds )
			{
				if( rBind == _axis_input )
					return &oActionKey;
			}
		}

		return nullptr;
	}

	void InputManager::_SendActionsEvent( const ActionKey* _pActionKey, Status _eStatus )
	{
		if( _pActionKey == nullptr )
			return;

		Event oEvent( Event::eActionEvent );
		oEvent.m_oActionEvent.m_eActionStatus = _eStatus;
		oEvent.m_oActionEvent.m_pAction = _pActionKey;

		g_pFZN_Core->PushEvent( oEvent );
	}

	std::string InputManager::_GetActionKeyString( const std::string& _sActionOrKey, bool _bKeyboard, int _iIndex /*= 0 */ ) const
	{
		const ActionKey* pActionKey = GetActionKey( _sActionOrKey );
		std::string sGlyphName;

		static const Overloaded input_string_overloads
		{
			[&]( const ActionKey::KeyBind& _key_bind ) -> std::string			{ return key_bind_to_string( _key_bind ); },
			[&]( sf::Mouse::Button _button ) -> std::string						{ return MouseButtonToString( _button ); },
			[&]( uint32_t _joystick_button ) -> std::string						{ return JoystickButtonToString( _joystick_button ); },
			[&]( const ActionKey::AxisInput& _joystick_axis ) -> std::string	{ return axis_input_to_string( _joystick_axis ); }
		};

		if( pActionKey != nullptr )
		{
			if( _bKeyboard && _iIndex < (int)pActionKey->m_oKeyboardBinds.size() )
			{
				sGlyphName = pActionKey->m_oKeyboardBinds[ _iIndex ].visit< std::string >( input_string_overloads );

				if( sGlyphName.empty() )
					sGlyphName = _sActionOrKey;
			}

			if( _bKeyboard == false && _iIndex < (int)pActionKey->m_oControllerBinds.size() )
			{
				sGlyphName = pActionKey->m_oControllerBinds[ _iIndex ].visit< std::string >( input_string_overloads );

				if( sGlyphName.empty() )
					sGlyphName = _sActionOrKey;
			}
		}
		else
			sGlyphName = _sActionOrKey;

		return sGlyphName;
	}

	std::string InputManager::_GetDeviceTag( bool _bKeyboard ) const
	{
		if( _bKeyboard )
			return "Keyboard";

		return "XInput";
	}

	bool InputManager::_check_keyboard_key_state( sf::Keyboard::Key _key, Status _status_to_check ) const
	{
		if( _key >= sf::Keyboard::KeyCount )
			return false;

		return m_keyStates[ _key ] == _status_to_check;
	}

	bool InputManager::_check_keyboard_key_state_flag( sf::Keyboard::Key _key, StatusMask _state_mask ) const
	{
		if( _key >= sf::Keyboard::KeyCount )
			return false;

		return Tools::mask_has_flag_raised( _state_mask, 1 << m_keyStates[ _key ] );
	}

	bool InputManager::_check_mouse_button_state( sf::Mouse::Button _button, Status _status_to_check ) const
	{
		if( _button >= sf::Mouse::ButtonCount )
			return false;

		return m_buttonStates[ _button ] == _status_to_check;
	}

	bool InputManager::_check_joystick_button_state( int8_t _id, uint32_t _button, Status _status_to_check ) const
	{
		if( _id < 0 || _id > sf::Joystick::Count || !m_joysticks[ _id ].isConnected )
			return false;

		if( _button < 0 || _button >= m_joysticks[ _id ].buttonCount )
			return false;

		return m_joysticks[ _id ].states[ _button ] == _status_to_check;
	}

	bool InputManager::_check_joystick_axis_state( int8_t _id, const ActionKey::AxisInput& _axis_input, Status _status_to_check ) const
	{
		if( _id < 0 || _id > sf::Joystick::Count || m_joysticks[ _id ].isConnected || _axis_input.m_axis >= JoystickAxisCount || _axis_input.m_axis_direction >= JoystickAxisDirection_Count )
			return false;

		return m_joysticks[ _id ].axisStates[ _axis_input.m_axis ][ _axis_input.m_axis_direction ] == _status_to_check;
	}

	/////////////////CHARS MANAGEMENT/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Returns the corresponding character to a key (FR Keyboard)
	//Parameter : Concerned key
	//Return value : Corresponding character
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	char InputManager::GetFRKeyChar( const sf::Keyboard::Key& _key ) const
	{
		if( _key > sf::Keyboard::KeyCount )
			return '\0';

		switch( _key )
		{
		case sf::Keyboard::Unknown:		return '\0';
		case sf::Keyboard::A:			return 'a';
		case sf::Keyboard::B:			return 'b';
		case sf::Keyboard::C:			return 'c';
		case sf::Keyboard::D:			return 'd';
		case sf::Keyboard::E:			return 'e';
		case sf::Keyboard::F:			return 'f';
		case sf::Keyboard::G:			return 'g';
		case sf::Keyboard::H:			return 'h';
		case sf::Keyboard::I:			return 'i';
		case sf::Keyboard::J:			return 'j';
		case sf::Keyboard::K:			return 'k';
		case sf::Keyboard::L:			return 'l';
		case sf::Keyboard::M:			return 'm';
		case sf::Keyboard::N:			return 'n';
		case sf::Keyboard::O:			return 'o';
		case sf::Keyboard::P:			return 'p';
		case sf::Keyboard::Q:			return 'q';
		case sf::Keyboard::R:			return 'r';
		case sf::Keyboard::S:			return 's';
		case sf::Keyboard::T:			return 't';
		case sf::Keyboard::U:			return 'u';
		case sf::Keyboard::V:			return 'v';
		case sf::Keyboard::W:			return 'w';
		case sf::Keyboard::X:			return 'x';
		case sf::Keyboard::Y:			return 'y';
		case sf::Keyboard::Z:			return 'z';
		case sf::Keyboard::Num0:		return 'à';
		case sf::Keyboard::Num1:		return '&';
		case sf::Keyboard::Num2:		return 'é';
		case sf::Keyboard::Num3:		return '\"';
		case sf::Keyboard::Num4:		return '\'';
		case sf::Keyboard::Num5:		return '(';
		case sf::Keyboard::Num6:		return '-';
		case sf::Keyboard::Num7:		return 'è';
		case sf::Keyboard::Num8:		return '_';
		case sf::Keyboard::Num9:		return 'ç';
		case sf::Keyboard::LBracket:	return ')';
		case sf::Keyboard::RBracket:	return '^';
		case sf::Keyboard::SemiColon:	return '$';
		case sf::Keyboard::Comma:		return ',';
		case sf::Keyboard::Period:		return ';';
		case sf::Keyboard::Quote:		return '\'';
		case sf::Keyboard::Slash:		return ':';
		case sf::Keyboard::BackSlash:	return '*';
		case sf::Keyboard::Tilde:		return 'ù';
		case sf::Keyboard::Equal:		return '=';
		case sf::Keyboard::Dash:		return '-';
		case sf::Keyboard::Space:		return ' ';
		case sf::Keyboard::Return:		return '\n';
		case sf::Keyboard::Tab:			return '\t';
		case sf::Keyboard::Add:			return '+';
		case sf::Keyboard::Subtract:	return '-';
		case sf::Keyboard::Multiply:	return '*';
		case sf::Keyboard::Divide:		return '/';
		case sf::Keyboard::Numpad0:		return '0';
		case sf::Keyboard::Numpad1:		return '1';
		case sf::Keyboard::Numpad2:		return '2';
		case sf::Keyboard::Numpad3:		return '3';
		case sf::Keyboard::Numpad4:		return '4';
		case sf::Keyboard::Numpad5:		return '5';
		case sf::Keyboard::Numpad6:		return '6';
		case sf::Keyboard::Numpad7:		return '7';
		case sf::Keyboard::Numpad8:		return '8';
		case sf::Keyboard::Numpad9:		return '9';
		default:						return '\0';
		};
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Returns the corresponding upper character to a key (FR Keyboard)
	//Parameter : Concerned key
	//Return value : Corresponding character
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	char InputManager::GetFRKeyUpperChar( const sf::Keyboard::Key& _key ) const
	{
		if( _key > sf::Keyboard::KeyCount )
			return '\0';

		switch( _key )
		{
		case sf::Keyboard::Unknown:		return '\0';
		case sf::Keyboard::A:			return 'A';
		case sf::Keyboard::B:			return 'B';
		case sf::Keyboard::C:			return 'C';
		case sf::Keyboard::D:			return 'D';
		case sf::Keyboard::E:			return 'E';
		case sf::Keyboard::F:			return 'F';
		case sf::Keyboard::G:			return 'G';
		case sf::Keyboard::H:			return 'H';
		case sf::Keyboard::I:			return 'I';
		case sf::Keyboard::J:			return 'J';
		case sf::Keyboard::K:			return 'K';
		case sf::Keyboard::L:			return 'L';
		case sf::Keyboard::M:			return 'M';
		case sf::Keyboard::N:			return 'N';
		case sf::Keyboard::O:			return 'O';
		case sf::Keyboard::P:			return 'P';
		case sf::Keyboard::Q:			return 'Q';
		case sf::Keyboard::R:			return 'R';
		case sf::Keyboard::S:			return 'S';
		case sf::Keyboard::T:			return 'T';
		case sf::Keyboard::U:			return 'U';
		case sf::Keyboard::V:			return 'V';
		case sf::Keyboard::W:			return 'W';
		case sf::Keyboard::X:			return 'X';
		case sf::Keyboard::Y:			return 'Y';
		case sf::Keyboard::Z:			return 'Z';
		case sf::Keyboard::Num0:		return '0';
		case sf::Keyboard::Num1:		return '1';
		case sf::Keyboard::Num2:		return '2';
		case sf::Keyboard::Num3:		return '3';
		case sf::Keyboard::Num4:		return '4';
		case sf::Keyboard::Num5:		return '5';
		case sf::Keyboard::Num6:		return '6';
		case sf::Keyboard::Num7:		return '7';
		case sf::Keyboard::Num8:		return '8';
		case sf::Keyboard::Num9:		return '9';
		case sf::Keyboard::LBracket:	return '°';
		case sf::Keyboard::RBracket:	return '¨';
		case sf::Keyboard::SemiColon:	return '£';
		case sf::Keyboard::Comma:		return '?';
		case sf::Keyboard::Period:		return '.';
		case sf::Keyboard::Slash:		return '/';
		case sf::Keyboard::BackSlash:	return 'µ';
		case sf::Keyboard::Tilde:		return '%';
		case sf::Keyboard::Equal:		return '+';
		default:						return '\0';
		};
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Returns the corresponding character to a key (EN Keyboard)
	//Parameter : Concerned key
	//Return value : Corresponding character
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	char InputManager::GetENKeyChar( const sf::Keyboard::Key& _key ) const
	{
		if( _key > sf::Keyboard::KeyCount )
			return '\0';

		switch( _key )
		{
		case sf::Keyboard::Unknown:		return '\0';
		case sf::Keyboard::A:			return 'a';
		case sf::Keyboard::B:			return 'b';
		case sf::Keyboard::C:			return 'c';
		case sf::Keyboard::D:			return 'd';
		case sf::Keyboard::E:			return 'e';
		case sf::Keyboard::F:			return 'f';
		case sf::Keyboard::G:			return 'g';
		case sf::Keyboard::H:			return 'h';
		case sf::Keyboard::I:			return 'i';
		case sf::Keyboard::J:			return 'j';
		case sf::Keyboard::K:			return 'k';
		case sf::Keyboard::L:			return 'l';
		case sf::Keyboard::M:			return 'm';
		case sf::Keyboard::N:			return 'n';
		case sf::Keyboard::O:			return 'o';
		case sf::Keyboard::P:			return 'p';
		case sf::Keyboard::Q:			return 'q';
		case sf::Keyboard::R:			return 'r';
		case sf::Keyboard::S:			return 's';
		case sf::Keyboard::T:			return 't';
		case sf::Keyboard::U:			return 'u';
		case sf::Keyboard::V:			return 'v';
		case sf::Keyboard::W:			return 'w';
		case sf::Keyboard::X:			return 'x';
		case sf::Keyboard::Y:			return 'y';
		case sf::Keyboard::Z:			return 'z';
		case sf::Keyboard::Num0:		return '0';
		case sf::Keyboard::Num1:		return '1';
		case sf::Keyboard::Num2:		return '2';
		case sf::Keyboard::Num3:		return '3';
		case sf::Keyboard::Num4:		return '4';
		case sf::Keyboard::Num5:		return '5';
		case sf::Keyboard::Num6:		return '6';
		case sf::Keyboard::Num7:		return '7';
		case sf::Keyboard::Num8:		return '8';
		case sf::Keyboard::Num9:		return '9';
		case sf::Keyboard::LBracket:	return '[';
		case sf::Keyboard::RBracket:	return ']';
		case sf::Keyboard::SemiColon:	return ';';
		case sf::Keyboard::Comma:		return ',';
		case sf::Keyboard::Period:		return '.';
		case sf::Keyboard::Quote:		return '\'';
		case sf::Keyboard::Slash:		return '/';
		case sf::Keyboard::BackSlash:	return '\\';
		case sf::Keyboard::Tilde:		return '~';
		case sf::Keyboard::Equal:		return '=';
		case sf::Keyboard::Dash:		return '-';
		case sf::Keyboard::Space:		return ' ';
		case sf::Keyboard::Return:		return '\n';
		case sf::Keyboard::Tab:			return '\t';
		case sf::Keyboard::Add:			return '+';
		case sf::Keyboard::Subtract:	return '-';
		case sf::Keyboard::Multiply:	return '*';
		case sf::Keyboard::Divide:		return '/';
		case sf::Keyboard::Numpad0:		return '0';
		case sf::Keyboard::Numpad1:		return '1';
		case sf::Keyboard::Numpad2:		return '2';
		case sf::Keyboard::Numpad3:		return '3';
		case sf::Keyboard::Numpad4:		return '4';
		case sf::Keyboard::Numpad5:		return '5';
		case sf::Keyboard::Numpad6:		return '6';
		case sf::Keyboard::Numpad7:		return '7';
		case sf::Keyboard::Numpad8:		return '8';
		case sf::Keyboard::Numpad9:		return '9';
		default:						return '\0';
		};
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Returns the corresponding upper character to a key (EN Keyboard)
	//Parameter : Concerned key
	//Return value : Corresponding character
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	char InputManager::GetENKeyUpperChar( const sf::Keyboard::Key& _key ) const
	{
		if( _key > sf::Keyboard::KeyCount )
			return '\0';

		switch( _key )
		{
		case sf::Keyboard::Unknown:		return '\0';
		case sf::Keyboard::A:			return 'A';
		case sf::Keyboard::B:			return 'B';
		case sf::Keyboard::C:			return 'C';
		case sf::Keyboard::D:			return 'D';
		case sf::Keyboard::E:			return 'E';
		case sf::Keyboard::F:			return 'F';
		case sf::Keyboard::G:			return 'G';
		case sf::Keyboard::H:			return 'H';
		case sf::Keyboard::I:			return 'I';
		case sf::Keyboard::J:			return 'J';
		case sf::Keyboard::K:			return 'K';
		case sf::Keyboard::L:			return 'L';
		case sf::Keyboard::M:			return 'M';
		case sf::Keyboard::N:			return 'N';
		case sf::Keyboard::O:			return 'O';
		case sf::Keyboard::P:			return 'P';
		case sf::Keyboard::Q:			return 'Q';
		case sf::Keyboard::R:			return 'R';
		case sf::Keyboard::S:			return 'S';
		case sf::Keyboard::T:			return 'T';
		case sf::Keyboard::U:			return 'U';
		case sf::Keyboard::V:			return 'V';
		case sf::Keyboard::W:			return 'W';
		case sf::Keyboard::X:			return 'X';
		case sf::Keyboard::Y:			return 'Y';
		case sf::Keyboard::Z:			return 'Z';
		default:						return '\0';
		};
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Returns the corresponding key to a string (FR Keyboard)
	//Parameter : Concerned string
	//Return value : Corresponding key
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	sf::Keyboard::Key InputManager::StringToFRKey( const std::string& _key ) const
	{
		if( _key == "\0" )				return sf::Keyboard::Unknown;
		else if( _key == "A" )			return sf::Keyboard::A;
		else if( _key == "B" )			return sf::Keyboard::B;
		else if( _key == "C" )			return sf::Keyboard::C;
		else if( _key == "D" )			return sf::Keyboard::D;
		else if( _key == "E" )			return sf::Keyboard::E;
		else if( _key == "F" )			return sf::Keyboard::F;
		else if( _key == "G" )			return sf::Keyboard::G;
		else if( _key == "H" )			return sf::Keyboard::H;
		else if( _key == "I" )			return sf::Keyboard::I;
		else if( _key == "J" )			return sf::Keyboard::J;
		else if( _key == "K" )			return sf::Keyboard::K;
		else if( _key == "L" )			return sf::Keyboard::L;
		else if( _key == "M" )			return sf::Keyboard::M;
		else if( _key == "N" )			return sf::Keyboard::N;
		else if( _key == "O" )			return sf::Keyboard::O;
		else if( _key == "P" )			return sf::Keyboard::P;
		else if( _key == "Q" )			return sf::Keyboard::Q;
		else if( _key == "R" )			return sf::Keyboard::R;
		else if( _key == "S" )			return sf::Keyboard::S;
		else if( _key == "T" )			return sf::Keyboard::T;
		else if( _key == "U" )			return sf::Keyboard::U;
		else if( _key == "V" )			return sf::Keyboard::V;
		else if( _key == "W" )			return sf::Keyboard::W;
		else if( _key == "X" )			return sf::Keyboard::X;
		else if( _key == "Y" )			return sf::Keyboard::Y;
		else if( _key == "Z" )			return sf::Keyboard::Z;
		else if( _key == "0" )			return sf::Keyboard::Num0;
		else if( _key == "1" )			return sf::Keyboard::Num1;
		else if( _key == "2" )			return sf::Keyboard::Num2;
		else if( _key == "3" )			return sf::Keyboard::Num3;
		else if( _key == "4" )			return sf::Keyboard::Num4;
		else if( _key == "5" )			return sf::Keyboard::Num5;
		else if( _key == "6" )			return sf::Keyboard::Num6;
		else if( _key == "7" )			return sf::Keyboard::Num7;
		else if( _key == "8" )			return sf::Keyboard::Num8;
		else if( _key == "9" )			return sf::Keyboard::Num9;
		else if( _key == "Escape" )		return sf::Keyboard::Escape;
		else if( _key == "LCtrl" )		return sf::Keyboard::LControl;
		else if( _key == "LShift" )		return sf::Keyboard::LShift;
		else if( _key == "LAlt" )		return sf::Keyboard::LAlt;
		else if( _key == "LSystem" )	return sf::Keyboard::LSystem;
		else if( _key == "RCtrl" )		return sf::Keyboard::RControl;
		else if( _key == "RShift" )		return sf::Keyboard::RShift;
		else if( _key == "RAlt" )		return sf::Keyboard::RAlt;
		else if( _key == "RSystem" )	return sf::Keyboard::RSystem;
		else if( _key == "Menu" )		return sf::Keyboard::Menu;
		else if( _key == "°" )			return sf::Keyboard::LBracket;
		else if( _key == "¨" )			return sf::Keyboard::RBracket;
		else if( _key == "+" )			return sf::Keyboard::Equal;
		else if( _key == "£" )			return sf::Keyboard::SemiColon;
		else if( _key == "?" )			return sf::Keyboard::Comma;
		else if( _key == "." )			return sf::Keyboard::Period;
		else if( _key == "/" )			return sf::Keyboard::Slash;
		else if( _key == "µ" )			return sf::Keyboard::BackSlash;
		else if( _key == "%" )			return sf::Keyboard::Tilde;
		else if( _key == "+" )			return sf::Keyboard::Equal;
		else if( _key == "Space" )		return sf::Keyboard::Space;
		else if( _key == "Return" )		return sf::Keyboard::Return;
		else if( _key == "BackSpace" )	return sf::Keyboard::BackSpace;
		else if( _key == "Tab" )		return sf::Keyboard::Tab;
		else if( _key == "PageUp" )		return sf::Keyboard::PageUp;
		else if( _key == "PageDown" )	return sf::Keyboard::PageDown;
		else if( _key == "End" )		return sf::Keyboard::End;
		else if( _key == "Home" )		return sf::Keyboard::Home;
		else if( _key == "Inser" )		return sf::Keyboard::Insert;
		else if( _key == "Delete" )		return sf::Keyboard::Delete;
		else if( _key == "Add" )		return sf::Keyboard::Add;
		else if( _key == "Substract" )	return sf::Keyboard::Subtract;
		else if( _key == "Multiply" )	return sf::Keyboard::Multiply;
		else if( _key == "Divide" )		return sf::Keyboard::Divide;
		else if( _key == "Left" )		return sf::Keyboard::Left;
		else if( _key == "Right" )		return sf::Keyboard::Right;
		else if( _key == "Up" )			return sf::Keyboard::Up;
		else if( _key == "Down" )		return sf::Keyboard::Down;
		else if( _key == "NP0" )		return sf::Keyboard::Numpad0;
		else if( _key == "NP1" )		return sf::Keyboard::Numpad1;
		else if( _key == "NP2" )		return sf::Keyboard::Numpad2;
		else if( _key == "NP3" )		return sf::Keyboard::Numpad3;
		else if( _key == "NP4" )		return sf::Keyboard::Numpad4;
		else if( _key == "NP5" )		return sf::Keyboard::Numpad5;
		else if( _key == "NP6" )		return sf::Keyboard::Numpad6;
		else if( _key == "NP7" )		return sf::Keyboard::Numpad7;
		else if( _key == "NP8" )		return sf::Keyboard::Numpad8;
		else if( _key == "NP9" )		return sf::Keyboard::Numpad9;
		else if( _key == "F1" )			return sf::Keyboard::F1;
		else if( _key == "F2" )			return sf::Keyboard::F2;
		else if( _key == "F3" )			return sf::Keyboard::F3;
		else if( _key == "F4" )			return sf::Keyboard::F4;
		else if( _key == "F5" )			return sf::Keyboard::F5;
		else if( _key == "F6" )			return sf::Keyboard::F6;
		else if( _key == "F7" )			return sf::Keyboard::F7;
		else if( _key == "F8" )			return sf::Keyboard::F8;
		else if( _key == "F9" )			return sf::Keyboard::F9;
		else if( _key == "F10" )		return sf::Keyboard::F10;
		else if( _key == "F11" )		return sf::Keyboard::F11;
		else if( _key == "F12" )		return sf::Keyboard::F12;
		else if( _key == "F13" )		return sf::Keyboard::F13;
		else if( _key == "F14" )		return sf::Keyboard::F14;
		else if( _key == "F15" )		return sf::Keyboard::F15;
		else							return sf::Keyboard::Unknown;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Returns the corresponding key to a string (EN Keyboard)
	//Parameter : Concerned string
	//Return value : Corresponding key
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	sf::Keyboard::Key InputManager::StringToENKey( const std::string& _key ) const
	{
		if( _key == "\0" )				return sf::Keyboard::Unknown;
		else if( _key == "A" )			return sf::Keyboard::A;
		else if( _key == "B" )			return sf::Keyboard::B;
		else if( _key == "C" )			return sf::Keyboard::C;
		else if( _key == "D" )			return sf::Keyboard::D;
		else if( _key == "E" )			return sf::Keyboard::E;
		else if( _key == "F" )			return sf::Keyboard::F;
		else if( _key == "G" )			return sf::Keyboard::G;
		else if( _key == "H" )			return sf::Keyboard::H;
		else if( _key == "I" )			return sf::Keyboard::I;
		else if( _key == "J" )			return sf::Keyboard::J;
		else if( _key == "K" )			return sf::Keyboard::K;
		else if( _key == "L" )			return sf::Keyboard::L;
		else if( _key == "M" )			return sf::Keyboard::M;
		else if( _key == "N" )			return sf::Keyboard::N;
		else if( _key == "O" )			return sf::Keyboard::O;
		else if( _key == "P" )			return sf::Keyboard::P;
		else if( _key == "Q" )			return sf::Keyboard::Q;
		else if( _key == "R" )			return sf::Keyboard::R;
		else if( _key == "S" )			return sf::Keyboard::S;
		else if( _key == "T" )			return sf::Keyboard::T;
		else if( _key == "U" )			return sf::Keyboard::U;
		else if( _key == "V" )			return sf::Keyboard::V;
		else if( _key == "W" )			return sf::Keyboard::W;
		else if( _key == "X" )			return sf::Keyboard::X;
		else if( _key == "Y" )			return sf::Keyboard::Y;
		else if( _key == "Z" )			return sf::Keyboard::Z;
		else if( _key == "0" )			return sf::Keyboard::Num0;
		else if( _key == "1" )			return sf::Keyboard::Num1;
		else if( _key == "2" )			return sf::Keyboard::Num2;
		else if( _key == "3" )			return sf::Keyboard::Num3;
		else if( _key == "4" )			return sf::Keyboard::Num4;
		else if( _key == "5" )			return sf::Keyboard::Num5;
		else if( _key == "6" )			return sf::Keyboard::Num6;
		else if( _key == "7" )			return sf::Keyboard::Num7;
		else if( _key == "8" )			return sf::Keyboard::Num8;
		else if( _key == "9" )			return sf::Keyboard::Num9;
		else if( _key == "Escape" )		return sf::Keyboard::Escape;
		else if( _key == "LCtrl" )		return sf::Keyboard::LControl;
		else if( _key == "LShift" )		return sf::Keyboard::LShift;
		else if( _key == "LAlt" )		return sf::Keyboard::LAlt;
		else if( _key == "LSystem" )	return sf::Keyboard::LSystem;
		else if( _key == "RCtrl" )		return sf::Keyboard::RControl;
		else if( _key == "RShift" )		return sf::Keyboard::RShift;
		else if( _key == "RAlt" )		return sf::Keyboard::RAlt;
		else if( _key == "RSystem" )	return sf::Keyboard::RSystem;
		else if( _key == "Menu" )		return sf::Keyboard::Menu;
		else if( _key == "[" )			return sf::Keyboard::LBracket;
		else if( _key == "]" )			return sf::Keyboard::RBracket;
		else if( _key == "+" )			return sf::Keyboard::Equal;
		else if( _key == ";" )			return sf::Keyboard::SemiColon;
		else if( _key == "," )			return sf::Keyboard::Comma;
		else if( _key == "." )			return sf::Keyboard::Period;
		else if( _key == "'" )			return sf::Keyboard::Quote;
		else if( _key == "/" )			return sf::Keyboard::Slash;
		else if( _key == "\\" )			return sf::Keyboard::BackSlash;
		else if( _key == "~" )			return sf::Keyboard::Tilde;
		else if( _key == "=" )			return sf::Keyboard::Equal;
		else if( _key == "-" )			return sf::Keyboard::Dash;
		else if( _key == "Space" )		return sf::Keyboard::Space;
		else if( _key == "Return" )		return sf::Keyboard::Return;
		else if( _key == "BackSpace" )	return sf::Keyboard::BackSpace;
		else if( _key == "Tab" )		return sf::Keyboard::Tab;
		else if( _key == "PageUp" )		return sf::Keyboard::PageUp;
		else if( _key == "PageDown" )	return sf::Keyboard::PageDown;
		else if( _key == "End" )		return sf::Keyboard::End;
		else if( _key == "Home" )		return sf::Keyboard::Home;
		else if( _key == "Inser" )		return sf::Keyboard::Insert;
		else if( _key == "Delete" )		return sf::Keyboard::Delete;
		else if( _key == "Add" )		return sf::Keyboard::Add;
		else if( _key == "Substract" )	return sf::Keyboard::Subtract;
		else if( _key == "Multiply" )	return sf::Keyboard::Multiply;
		else if( _key == "Divide" )		return sf::Keyboard::Divide;
		else if( _key == "Left" )		return sf::Keyboard::Left;
		else if( _key == "Right" )		return sf::Keyboard::Right;
		else if( _key == "Up" )			return sf::Keyboard::Up;
		else if( _key == "Down" )		return sf::Keyboard::Down;
		else if( _key == "NP0" )		return sf::Keyboard::Numpad0;
		else if( _key == "NP1" )		return sf::Keyboard::Numpad1;
		else if( _key == "NP2" )		return sf::Keyboard::Numpad2;
		else if( _key == "NP3" )		return sf::Keyboard::Numpad3;
		else if( _key == "NP4" )		return sf::Keyboard::Numpad4;
		else if( _key == "NP5" )		return sf::Keyboard::Numpad5;
		else if( _key == "NP6" )		return sf::Keyboard::Numpad6;
		else if( _key == "NP7" )		return sf::Keyboard::Numpad7;
		else if( _key == "NP8" )		return sf::Keyboard::Numpad8;
		else if( _key == "NP9" )		return sf::Keyboard::Numpad9;
		else if( _key == "F1" )			return sf::Keyboard::F1;
		else if( _key == "F2" )			return sf::Keyboard::F2;
		else if( _key == "F3" )			return sf::Keyboard::F3;
		else if( _key == "F4" )			return sf::Keyboard::F4;
		else if( _key == "F5" )			return sf::Keyboard::F5;
		else if( _key == "F6" )			return sf::Keyboard::F6;
		else if( _key == "F7" )			return sf::Keyboard::F7;
		else if( _key == "F8" )			return sf::Keyboard::F8;
		else if( _key == "F9" )			return sf::Keyboard::F9;
		else if( _key == "F10" )		return sf::Keyboard::F10;
		else if( _key == "F11" )		return sf::Keyboard::F11;
		else if( _key == "F12" )		return sf::Keyboard::F12;
		else if( _key == "F13" )		return sf::Keyboard::F13;
		else if( _key == "F14" )		return sf::Keyboard::F14;
		else if( _key == "F15" )		return sf::Keyboard::F15;
		else							return sf::Keyboard::Unknown;
	}

	std::string InputManager::KeyToString( const sf::Keyboard::Key& _key ) const
	{
		if( _key > sf::Keyboard::KeyCount )
			return "";

		switch( _key )
		{
		case sf::Keyboard::Unknown:		return "";
		case sf::Keyboard::A:			return "A";
		case sf::Keyboard::B:			return "B";
		case sf::Keyboard::C:			return "C";
		case sf::Keyboard::D:			return "D";
		case sf::Keyboard::E:			return "E";
		case sf::Keyboard::F:			return "F";
		case sf::Keyboard::G:			return "G";
		case sf::Keyboard::H:			return "H";
		case sf::Keyboard::I:			return "I";
		case sf::Keyboard::J:			return "J";
		case sf::Keyboard::K:			return "K";
		case sf::Keyboard::L:			return "L";
		case sf::Keyboard::M:			return "M";
		case sf::Keyboard::N:			return "N";
		case sf::Keyboard::O:			return "O";
		case sf::Keyboard::P:			return "P";
		case sf::Keyboard::Q:			return "Q";
		case sf::Keyboard::R:			return "R";
		case sf::Keyboard::S:			return "S";
		case sf::Keyboard::T:			return "T";
		case sf::Keyboard::U:			return "U";
		case sf::Keyboard::V:			return "V";
		case sf::Keyboard::W:			return "W";
		case sf::Keyboard::X:			return "X";
		case sf::Keyboard::Y:			return "Y";
		case sf::Keyboard::Z:			return "Z";
		case sf::Keyboard::Num0:		return "0";
		case sf::Keyboard::Num1:		return "1";
		case sf::Keyboard::Num2:		return "2";
		case sf::Keyboard::Num3:		return "3";
		case sf::Keyboard::Num4:		return "4";
		case sf::Keyboard::Num5:		return "5";
		case sf::Keyboard::Num6:		return "6";
		case sf::Keyboard::Num7:		return "7";
		case sf::Keyboard::Num8:		return "8";
		case sf::Keyboard::Num9:		return "9";
		case sf::Keyboard::LControl:	return "LCtrl";
		case sf::Keyboard::LShift:		return "LShift";
		case sf::Keyboard::LAlt:		return "LAlt";
		case sf::Keyboard::LSystem:		return "LSystem";
		case sf::Keyboard::RControl:	return "RCtrl";
		case sf::Keyboard::RShift:		return "RShift";
		case sf::Keyboard::RAlt:		return "RAlt";
		case sf::Keyboard::RSystem:		return "RSystem";
		case sf::Keyboard::LBracket:	return "LBracket";
		case sf::Keyboard::RBracket:	return "RBracket";
		case sf::Keyboard::SemiColon:	return "SemiColon";
		case sf::Keyboard::Comma:		return "Comma";
		case sf::Keyboard::Period:		return "Period";
		case sf::Keyboard::Quote:		return "Quote";
		case sf::Keyboard::Slash:		return "Slash";
		case sf::Keyboard::BackSlash:	return "BackSlash";
		case sf::Keyboard::Tilde:		return "Tilde";
		case sf::Keyboard::Equal:		return "Equal";
		case sf::Keyboard::Dash:		return "Dash";
		case sf::Keyboard::Space:		return "Space";
		case sf::Keyboard::Return:		return "Return";
		case sf::Keyboard::BackSpace:	return "BackSpace";
		case sf::Keyboard::Tab:			return "Tab";
		case sf::Keyboard::PageUp:		return "PageUp";
		case sf::Keyboard::PageDown:	return "PageDown";
		case sf::Keyboard::End:			return "End";
		case sf::Keyboard::Home:		return "Home";
		case sf::Keyboard::Insert:		return "Inser";
		case sf::Keyboard::Delete:		return "Delete";
		case sf::Keyboard::Add:			return "Add";
		case sf::Keyboard::Subtract:	return "Substract";
		case sf::Keyboard::Multiply:	return "Multiply";
		case sf::Keyboard::Divide:		return "Divide";
		case sf::Keyboard::Numpad0:		return "Numpad0";
		case sf::Keyboard::Numpad1:		return "Numpad1";
		case sf::Keyboard::Numpad2:		return "Numpad2";
		case sf::Keyboard::Numpad3:		return "Numpad3";
		case sf::Keyboard::Numpad4:		return "Numpad4";
		case sf::Keyboard::Numpad5:		return "Numpad5";
		case sf::Keyboard::Numpad6:		return "Numpad6";
		case sf::Keyboard::Numpad7:		return "Numpad7";
		case sf::Keyboard::Numpad8:		return "Numpad8";
		case sf::Keyboard::Numpad9:		return "Numpad9";
		case sf::Keyboard::F1:			return "F1";
		case sf::Keyboard::F2:			return "F2";
		case sf::Keyboard::F3:			return "F3";
		case sf::Keyboard::F4:			return "F4";
		case sf::Keyboard::F5:			return "F5";
		case sf::Keyboard::F6:			return "F6";
		case sf::Keyboard::F7:			return "F7";
		case sf::Keyboard::F8:			return "F8";
		case sf::Keyboard::F9:			return "F9";
		case sf::Keyboard::F10:			return "F10";
		case sf::Keyboard::F11:			return "F11";
		case sf::Keyboard::F12:			return "F12";
		case sf::Keyboard::F13:			return "F13";
		case sf::Keyboard::F14:			return "F14";
		case sf::Keyboard::F15:			return "F15";
		case sf::Keyboard::Up:			return "Up";
		case sf::Keyboard::Down:		return "Down";
		case sf::Keyboard::Left:		return "Left";
		case sf::Keyboard::Right:		return "Right";
		case sf::Keyboard::Escape:		return "Escape";
		default:						return "";
		};
	}

	std::string InputManager::key_bind_to_string( const ActionKey::KeyBind& _key_bind ) const
	{
		std::string result{};
		std::string key_string{};

		auto key_to_string = [&result,&key_string,this]( sf::Keyboard::Key _key )
		{
			key_string = KeyToString( _key );

			if( key_string.empty() == false )
				result += ( result.empty() ? "" : "+" ) + key_string;
		};

		for( sf::Keyboard::Key key : _key_bind.m_modifiers )
		{
			key_to_string( key );
		}

		key_to_string( _key_bind.m_key );

		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Returns the corresponding mouse button to a string
	//Parameter : Concerned string
	//Return value : Corresponding mouse button
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	sf::Mouse::Button InputManager::StringToMouseButton( const std::string& _button ) const
	{
		if( _button == "Left" )					return sf::Mouse::Left;
		else if( _button == "Right" )			return sf::Mouse::Right;
		else if( _button == "Middle" )			return sf::Mouse::Middle;
		else if( _button == "ExtraButton1" )	return sf::Mouse::XButton1;
		else if( _button == "ExtraButton2" )	return sf::Mouse::XButton2;
		else									return sf::Mouse::ButtonCount;
	}

	std::string InputManager::MouseButtonToString( const sf::Mouse::Button& _eButton ) const
	{
		switch( _eButton )
		{
		case sf::Mouse::Left:			return "Left";
		case sf::Mouse::Right:			return "Right";
		case sf::Mouse::Middle:			return "Middle";
		case sf::Mouse::XButton1:		return "ExtraButton1";
		case sf::Mouse::XButton2:		return "ExtraButton2";
		default:						return "";
		};
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Returns the corresponding joystick button to a string
	//Parameter : Concerned string
	//Return value : Corresponding joystick button
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	INT8 InputManager::StringToJoystickButton( const std::string& _button ) const
	{
		if( _button == "B0" )			return 0;
		else if( _button == "B1" )		return 1;
		else if( _button == "B2" )		return 2;
		else if( _button == "B3" )		return 3;
		else if( _button == "B4" )		return 4;
		else if( _button == "B5" )		return 5;
		else if( _button == "B6" )		return 6;
		else if( _button == "B7" )		return 7;
		else if( _button == "B8" )		return 8;
		else if( _button == "B9" )		return 9;
		else if( _button == "B10" )		return 10;
		else if( _button == "B11" )		return 11;
		else if( _button == "B12" )		return 12;
		else if( _button == "B13" )		return 13;
		else if( _button == "B14" )		return 14;
		else if( _button == "B15" )		return 15;
		else if( _button == "B16" )		return 16;
		else if( _button == "B17" )		return 17;
		else if( _button == "B18" )		return 18;
		else if( _button == "B19" )		return 19;
		else if( _button == "B20" )		return 20;
		else if( _button == "B21" )		return 21;
		else if( _button == "B22" )		return 22;
		else if( _button == "B23" )		return 23;
		else if( _button == "B24" )		return 24;
		else if( _button == "B25" )		return 25;
		else if( _button == "B26" )		return 26;
		else if( _button == "B27" )		return 27;
		else if( _button == "B28" )		return 28;
		else if( _button == "B29" )		return 29;
		else if( _button == "B30" )		return 30;
		else if( _button == "B31" )		return 31;
		else							return -1;
	}

	std::string InputManager::JoystickButtonToString( const INT8& _iButton ) const
	{
		switch( _iButton )
		{
		case 0:		return "B0";
		case 1:		return "B1";
		case 2:		return "B2";
		case 3:		return "B3";
		case 4:		return "B4";
		case 5:		return "B5";
		case 6:		return "B6";
		case 7:		return "B7";
		case 8:		return "B8";
		case 9:		return "B9";
		case 10:	return "B10";
		case 11:	return "B11";
		case 12:	return "B12";
		case 13:	return "B13";
		case 14:	return "B14";
		case 15:	return "B15";
		case 16:	return "B16";
		case 17:	return "B17";
		case 18:	return "B18";
		case 19:	return "B19";
		case 20:	return "B20";
		case 21:	return "B21";
		case 22:	return "B22";
		case 23:	return "B23";
		case 24:	return "B24";
		case 25:	return "B25";
		case 26:	return "B26";
		case 27:	return "B27";
		case 28:	return "B28";
		case 29:	return "B29";
		case 30:	return "B30";
		case 31:	return "B31";
		default:	return "";
		};
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Returns the corresponding joystick axis to a string
	//Parameter : Concerned string
	//Return value : Corresponding joystick axis
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	ActionKey::AxisInput InputManager::string_to_axis_input( const std::string& _button ) const
	{
		int iAxisDirection = _button.find( "Pos" );

		if( iAxisDirection == std::string::npos )
			iAxisDirection = _button.find( "Neg" );

		std::string sAxis = "";
		JoystickAxisDirection axis_direction{ JoystickAxisDirection_Full };

		if( iAxisDirection == std::string::npos )
		{
			sAxis = _button;
		}
		else
		{
			sAxis = _button.substr( 0, iAxisDirection );
			std::string sDirection = _button.substr( iAxisDirection );

			if( sDirection == "Pos" )
				axis_direction = JoystickAxisDirection_Positive;
			else
				axis_direction = JoystickAxisDirection_Negative;
		}

		if( sAxis == "X" )				return { sf::Joystick::Axis::X,		axis_direction };
		else if( sAxis == "Y" )			return { sf::Joystick::Axis::Y,		axis_direction };
		else if( sAxis == "Z" )			return { sf::Joystick::Axis::Z,		axis_direction };
		else if( sAxis == "R" )			return { sf::Joystick::Axis::R,		axis_direction };
		else if( sAxis == "U" )			return { sf::Joystick::Axis::U,		axis_direction };
		else if( sAxis == "V" )			return { sf::Joystick::Axis::V,		axis_direction };
		else if( sAxis == "PovX" )		return { sf::Joystick::Axis::PovX,	axis_direction };
		else if( sAxis == "PovY" )		return { sf::Joystick::Axis::PovY,	axis_direction };
		else							return { sf::Joystick::Axis::X,		axis_direction };
	}
	std::string InputManager::axis_input_to_string( const ActionKey::AxisInput& _axis_input ) const
	{
		std::string sAxis = "";
		switch( _axis_input.m_axis )
		{
		case sf::Joystick::Axis::X:
			sAxis = "X";
			break;
		case sf::Joystick::Axis::Y:
			sAxis = "Y";
			break;
		case sf::Joystick::Axis::Z:
			sAxis = "Z";
			break;
		case sf::Joystick::Axis::R:
			sAxis = "R";
			break;
		case sf::Joystick::Axis::U:
			sAxis = "U";
			break;
		case sf::Joystick::Axis::V:
			sAxis = "V";
			break;
		case sf::Joystick::Axis::PovX:
			sAxis = "PovX";
			break;
		case sf::Joystick::Axis::PovY:
			sAxis = "PovY";
			break;
		default:
			return "";
		};

		if( _axis_input.m_axis_direction == JoystickAxisDirection_Full )
			return sAxis;

		return sAxis + ( _axis_input.m_axis_direction == JoystickAxisDirection_Positive ? "Pos" : "Neg" );
	}
} //namespace fzn
