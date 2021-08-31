//------------------------------------------------------------------------
//Author : Philippe OFFERMANN
//Date : 25.03.14
//Description : Keyboard INPUTSSs manager
//------------------------------------------------------------------------

#include <fstream>

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
	/////////////////CONSTRUCTOR / DESTRUCTOR/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Default constructor
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	InputManager::InputManager()
	{
		g_pFZN_Core->AddCallBack( this, FctInputMgrEvent, fzn::FazonCore::CB_Event );

		KeyboardInit();
		MouseInit();

		sf::Joystick::update();
		JoystickInit();
		LoadActionKeysFromXML( m_oDefaultActionKeys, DATAPATH( "XMLFiles/actionKeys.xml" ) );

		if( g_pFZN_Core->FileExists( g_pFZN_Core->GetSaveFolderPath() + "/actionKeys.xml" ) )
		{
			LoadActionKeysFromXML( m_oCustomActionKeys, g_pFZN_Core->GetSaveFolderPath() + "/actionKeys.xml" );

			if( m_oCustomActionKeys.empty() )
				m_oCustomActionKeys = m_oDefaultActionKeys;
		}
		else
		{
			m_oCustomActionKeys = m_oDefaultActionKeys;
			SaveCustomActionKeysToFile();
		}

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

		switch( _event.type )
		{
		case sf::Event::KeyReleased:
			m_keyBasicStates[ _event.key.code ] = FALSE;
			break;
		case sf::Event::MouseButtonReleased:
			m_buttonBasicStates[ _event.mouseButton.button ] = FALSE;
			break;
		case sf::Event::JoystickButtonReleased:
			m_joysticks[ _event.joystickButton.joystickId ].basicStates[ _event.joystickButton.button ] = FALSE;
			break;
		};

		// Handling pressed event for eventual key binds before updating basic states so the event used for the bind doesn't trigger anything else.
		if( IsWaitingActionKeyBind() )
		{
			bool bResetEvent = false;
			bool bBindReplaced = false;
			if( IsWaitingInputForType( BindType::eKey ) && _event.type == sf::Event::KeyPressed )
			{
				bBindReplaced = _SetActionKeyBind_Key( _event.key.code );
				bResetEvent = true;
			}
			else if( IsWaitingInputForType( BindType::eMouseButton ) && _event.type == sf::Event::MouseButtonPressed )
			{
				bBindReplaced = _SetActionKeyBind_MouseButton( _event.mouseButton.button );
				bResetEvent = true;
			}
			else if( IsWaitingInputForType( BindType::eJoystickButton ) && _event.type == sf::Event::JoystickButtonPressed )
			{
				bBindReplaced = _SetActionKeyBind_JoystickButton( _event.joystickButton.button );
				bResetEvent = true;
			}
			else if( IsWaitingInputForType( BindType::eJoystickAxis ) && _event.type == sf::Event::JoystickMoved )
			{
				bBindReplaced = _SetActionKeyBind_JoystickAxis( _event.joystickMove.axis );
				bResetEvent = true;
			}

			// Resetting so event the code using events won't get this one.
			if( bResetEvent )
				_event = sf::Event();

			if( bBindReplaced )
			{
				g_pFZN_Core->PushEvent( Event::Type::eActionKeyBindDone );
			}

			return;
		}

		switch( _event.type )
		{
		case sf::Event::KeyPressed:
			m_keyBasicStates[ _event.key.code ] = TRUE;
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
			m_joysticks[ _event.joystickButton.joystickId ].basicAxes[ _event.joystickMove.axis ] = _event.joystickMove.position;
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
	bool InputManager::IsKeyPressed( sf::Keyboard::Key _key )
	{
		return ( m_keyStates[ _key ] == Status::Pressed );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Key press test (maintained donw)
	//Parameter : Concerned key
	//Return value : The key is down (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	bool InputManager::IsKeyDown( sf::Keyboard::Key _key )
	{
		return ( m_keyStates[ _key ] == Status::Down );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Release test (transition from Down to Up)
	//Parameter : Concerned key
	//Return value : The key is released (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	bool InputManager::IsKeyReleased( sf::Keyboard::Key _key )
	{
		return ( m_keyStates[ _key ] == Status::Released );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Key not pressed test (staying Up)
	//Parameter : Concerned key
	//Return value : The key is up (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	bool InputManager::IsKeyUp( sf::Keyboard::Key _key )
	{
		return ( m_keyStates[ _key ] == Status::Up );
	}

	fzn::InputManager::Status InputManager::GetKeyState( sf::Keyboard::Key _oKey )
	{
		if( _oKey < sf::Keyboard::KeyCount )
			return m_keyStates[ _oKey ];

		return Status::Up;
	}


	/////////////////ACTIONKEYS TESTS/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Single press test (transition from Up to Down)
	//Parameter : Concerned actionKey
	//Return value : The actionKey is pressed (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	bool InputManager::IsKeyPressed( const char* _key )
	{
		const ActionKey* pActionKey = GetActionKey( _key );

		for( unsigned int i = 0; i < pActionKey->m_oKeys.size(); ++i )
		{
			if( m_keyStates[ pActionKey->m_oKeys[ i ] ] == Status::Pressed )
				return true;
		}

		for( unsigned int i = 0; i < pActionKey->m_oMouseButtons.size(); ++i )
		{
			if( m_buttonStates[ pActionKey->m_oMouseButtons[ i ] ] == Status::Pressed )
				return true;
		}

		INT8 iIndex = 0;
		for( unsigned int i = 0; i < pActionKey->m_oJoystickButtons.size(); ++i )
		{
			iIndex = pActionKey->m_oJoystickButtons[ i ];

			if( iIndex < sf::Joystick::ButtonCount && m_joysticks[ m_defaultJoystick ].states[ iIndex ] == Status::Pressed )
				return true;
		}

		for( unsigned int i = 0; i < pActionKey->m_oJoystickAxes.size(); ++i )
		{
			iIndex = pActionKey->m_oJoystickAxes[ i ];

			if( iIndex >= sf::Joystick::AxisCount )
				continue;

			if( fabs( m_joysticks[ m_defaultJoystick ].axes[ iIndex ] - m_joysticks[ m_defaultJoystick ].defaultAxisValues[ iIndex ] ) > m_joysticks[ m_defaultJoystick ].axesDeadZone )
				return true;
		}

		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Key press test (maintained down)
	//Parameter : Concerned actionKey
	//Return value : The actionKey is down (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	bool InputManager::IsKeyDown( const char* _key )
	{
		const ActionKey* pActionKey = GetActionKey( _key );

		for( unsigned int i = 0; i < pActionKey->m_oKeys.size(); ++i )
		{
			if( m_keyStates[ pActionKey->m_oKeys[ i ] ] == Status::Down )
				return true;
		}

		for( unsigned int i = 0; i < pActionKey->m_oMouseButtons.size(); ++i )
		{
			if( m_buttonStates[ pActionKey->m_oMouseButtons[ i ] ] == Status::Down )
				return true;
		}

		INT8 iIndex = 0;
		for( unsigned int i = 0; i < pActionKey->m_oJoystickButtons.size(); ++i )
		{
			iIndex = pActionKey->m_oJoystickButtons[ i ];
			if( iIndex < sf::Joystick::ButtonCount && m_joysticks[ m_defaultJoystick ].states[ iIndex ] == Status::Down )
				return true;
		}

		for( unsigned int i = 0; i < pActionKey->m_oJoystickAxes.size(); ++i )
		{
			iIndex = pActionKey->m_oJoystickAxes[ i ];

			if( iIndex >= sf::Joystick::AxisCount )
				continue;

			if( fabs( m_joysticks[ m_defaultJoystick ].axes[ iIndex ] - m_joysticks[ m_defaultJoystick ].defaultAxisValues[ iIndex ] ) > m_joysticks[ m_defaultJoystick ].axesDeadZone )
				return true;
		}

		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Release test (transition from Down to Up)
	//Parameter : Concerned actionKey
	//Return value : The actionKey is released (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	bool InputManager::IsKeyReleased( const char* _key )
	{
		const ActionKey* pActionKey = GetActionKey( _key );

		for( unsigned int i = 0; i < pActionKey->m_oKeys.size(); ++i )
		{
			if( m_keyStates[ pActionKey->m_oKeys[ i ] ] == Status::Released )
				return true;
		}

		for( unsigned int i = 0; i < pActionKey->m_oMouseButtons.size(); ++i )
		{
			if( m_buttonStates[ pActionKey->m_oMouseButtons[ i ] ] == Status::Released )
				return true;
		}

		INT8 iIndex = 0;
		for( unsigned int i = 0; i < pActionKey->m_oJoystickButtons.size(); ++i )
		{
			iIndex = pActionKey->m_oJoystickButtons[ i ];
			if( iIndex < sf::Joystick::ButtonCount && m_joysticks[ m_defaultJoystick ].states[ iIndex ] == Status::Released )
				return true;
		}

		for( unsigned int i = 0; i < pActionKey->m_oJoystickAxes.size(); ++i )
		{
			iIndex = pActionKey->m_oJoystickAxes[ i ];

			if( iIndex >= sf::Joystick::AxisCount )
				continue;

			if( fabs( m_joysticks[ m_defaultJoystick ].axes[ iIndex ] - m_joysticks[ m_defaultJoystick ].defaultAxisValues[ iIndex ] ) <= m_joysticks[ m_defaultJoystick ].axesDeadZone )
				return true;
		}

		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Key not pressed test (staying Up)
	//Parameter : Concerned actionKey
	//Return value : The actionKey is up (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	bool InputManager::IsKeyUp( const char* _key )
	{
		const ActionKey* pActionKey = GetActionKey( _key );

		for( unsigned int i = 0; i < pActionKey->m_oKeys.size(); ++i )
		{
			if( m_keyStates[ pActionKey->m_oKeys[ i ] ] != Status::Up )
				return false;
		}

		for( unsigned int i = 0; i < pActionKey->m_oMouseButtons.size(); ++i )
		{
			if( m_buttonStates[ pActionKey->m_oMouseButtons[ i ] ] != Status::Up )
				return false;
		}

		INT8 iIndex = 0;
		for( unsigned int i = 0; i < pActionKey->m_oJoystickButtons.size(); ++i )
		{
			iIndex = pActionKey->m_oJoystickButtons[ i ];
			if( iIndex < sf::Joystick::ButtonCount && m_joysticks[ m_defaultJoystick ].states[ iIndex ] != Status::Up )
				return false;
		}

		for( unsigned int i = 0; i < pActionKey->m_oJoystickAxes.size(); ++i )
		{
			iIndex = pActionKey->m_oJoystickAxes[ i ];

			if( iIndex >= sf::Joystick::AxisCount )
				continue;

			if( fabs( m_joysticks[ m_defaultJoystick ].axes[ iIndex ] - m_joysticks[ m_defaultJoystick ].defaultAxisValues[ iIndex ] ) > m_joysticks[ m_defaultJoystick ].axesDeadZone )
				return false;
		}

		return true;
	}

	fzn::InputManager::Status InputManager::GetKeyState( const char* _actionKey )
	{
		const ActionKey* pActionKey = GetActionKey( _actionKey );

		if( pActionKey == nullptr )
			return Status::Up;

		for( unsigned int i = 0; i < pActionKey->m_oKeys.size(); ++i )
		{
			if( m_keyStates[ pActionKey->m_oKeys[ i ] ] != Status::Up )
				return m_keyStates[ pActionKey->m_oKeys[ i ] ];
		}

		for( unsigned int i = 0; i < pActionKey->m_oMouseButtons.size(); ++i )
		{
			if( m_buttonStates[ pActionKey->m_oMouseButtons[ i ] ] != Status::Up )
				return m_buttonStates[ pActionKey->m_oMouseButtons[ i ] ];
		}

		INT8 iIndex = 0;
		for( unsigned int i = 0; i < pActionKey->m_oJoystickButtons.size(); ++i )
		{
			iIndex = pActionKey->m_oJoystickButtons[ i ];

			if( iIndex < sf::Joystick::ButtonCount && m_joysticks[ m_defaultJoystick ].states[ iIndex ] != Status::Up )
				return m_joysticks[ m_defaultJoystick ].states[ iIndex ];
		}

		return Status::Up;
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
	INT8 InputManager::IsMousePressed( sf::Mouse::Button _button )
	{
		return ( m_buttonStates[ _button ] == Status::Pressed );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Button press test (maintained donw)
	//Parameter : Concerned button
	//Return value : The button is down (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	INT8 InputManager::IsMouseDown( sf::Mouse::Button _button )
	{
		return ( m_buttonStates[ _button ] == Status::Down );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Release test (transition from Down to Up)
	//Parameter : Concerned button
	//Return value : The button is released (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	INT8 InputManager::IsMouseReleased( sf::Mouse::Button _button )
	{
		return ( m_buttonStates[ _button ] == Status::Released );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Button not pressed test (staying Up)
	//Parameter : Concerned button
	//Return value : The button is up (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	INT8 InputManager::IsMouseUp( sf::Mouse::Button _button )
	{
		return ( m_buttonStates[ _button ] == Status::Up );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Checks if the mouse wheel is scrolling up
	//Return value : The mouse wheel goes up (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	INT8 InputManager::IsMouseWheelGoingUp()
	{
		if( m_mouseWheelDelta > 0 )
			return TRUE;
		else return FALSE;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Checks if the mouse wheel is scrolling down
	//Return value : The mouse wheel goes down (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	INT8 InputManager::IsMouseWheelGoingDown()
	{
		if( m_mouseWheelDelta < 0 )
			return TRUE;
		else return FALSE;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on the mouse wheel delta
	//Return value : return status Down (delta < 0) or Up (delta > 0), returns "nbStates" if delta = 0
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	InputManager::Status InputManager::GetMouseWheelDirection()
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
	INT8 InputManager::IsJoystickButtonPressed( INT8 _id, INT8 _button )
	{
		if( _id < 0 || _id > sf::Joystick::Count || !m_joysticks[ _id ].isConnected )
			return FALSE;

		if( _button < 0 || _button > m_joysticks[ _id ].buttonCount )
			return FALSE;

		return m_joysticks[ _id ].states[ _button ] == Status::Pressed;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Button pressed test (maintened down)
	//Parameter 1 : Joystick ID
	//Parameter 2 : Concerned joystick button
	//Return value : The button is pressed (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	INT8 InputManager::IsJoystickButtonDown( INT8 _id, INT8 _button )
	{
		if( _id < 0 || _id > sf::Joystick::Count || !m_joysticks[ _id ].isConnected )
			return FALSE;

		if( _button < 0 || _button > m_joysticks[ _id ].buttonCount )
			return FALSE;

		return m_joysticks[ _id ].states[ _button ] == Status::Down;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Release test (transition from Down to Up)
	//Parameter 1 : Joystick ID
	//Parameter 2 : Concerned joystick button
	//Return value : The button is pressed (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	INT8 InputManager::IsJoystickButtonReleased( INT8 _id, INT8 _button )
	{
		if( _id < 0 || _id > sf::Joystick::Count || !m_joysticks[ _id ].isConnected )
			return FALSE;

		if( _button < 0 || _button > m_joysticks[ _id ].buttonCount )
			return FALSE;

		return m_joysticks[ _id ].states[ _button ] == Status::Released;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Button not pressed test (staying up)
	//Parameter 1 : Joystick ID
	//Parameter 2 : Concerned joystick button
	//Return value : The button is pressed (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	INT8 InputManager::IsJoystickButtonUp( INT8 _id, INT8 _button )
	{
		if( _id < 0 || _id > sf::Joystick::Count || !m_joysticks[ _id ].isConnected )
			return FALSE;

		if( _button < 0 || _button > m_joysticks[ _id ].buttonCount )
			return FALSE;

		return m_joysticks[ _id ].states[ _button ] == Status::Up;
	}


	/////////////////STICKS FUNCTIONS/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Indicates if the given axis is stopped (on its default value)
	//Parameter 1 : Joystick ID
	//Parameter 2 : Concerned joystick axis
	//Return value : The axis is stopped (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	INT8 InputManager::IsJoystickAxisStopped( INT8 _id, sf::Joystick::Axis _axis )
	{
		if( _id < 0 || _id > sf::Joystick::Count || !m_joysticks[ _id ].isConnected )
			return FALSE;

		if( _axis < 0 || _axis > m_joysticks[ _id ].axisCount || m_joysticks[ _id ].defaultAxisValues == nullptr )
			return FALSE;

		return fabs( m_joysticks[ _id ].axes[ _axis ] - m_joysticks[ _id ].defaultAxisValues[ _axis ] ) < m_joysticks[ _id ].axesDeadZone;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Indicates if the axis corresponding to the given action is stopped (on its default value)
	//Parameter 1 : Joystick ID
	//Parameter 2 : Action
	//Return value : The axis is stopped (true) or not
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	INT8 InputManager::IsJoystickAxisStopped( INT8 _id, const char* _action )
	{
		if( _id < 0 || _id > sf::Joystick::Count || !m_joysticks[ _id ].isConnected )
			return FALSE;

		const ActionKey* pActionKey = GetActionKey( _action );

		INT8 iIndex = 0;
		for( unsigned int i = 0; i < pActionKey->m_oJoystickAxes.size(); ++i )
		{
			iIndex = pActionKey->m_oJoystickAxes[ i ];

			if( iIndex >= sf::Joystick::AxisCount )
				continue;

			if( fabs( m_joysticks[ _id ].axes[ iIndex ] - m_joysticks[ _id ].defaultAxisValues[ iIndex ] ) > m_joysticks[ _id ].axesDeadZone )
				return FALSE;
		}

		return TRUE;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on the position of a given axis
	//Parameter 1 : Joystick ID
	//Parameter 2 : Concerned joystick axis
	//Return value : Axis position
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	float InputManager::GetJoystickAxisPosition( INT8 _id, sf::Joystick::Axis _axis )
	{
		if( _id < 0 || _id > sf::Joystick::Count || !m_joysticks[ _id ].isConnected )
			return 0.f;

		if( _axis < 0 || _axis > m_joysticks[ _id ].axisCount )
			return 0.f;

		return m_joysticks[ _id ].axes[ _axis ];
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on the position of the axis corresponding to an action
	//Parameter 1 : Joystick ID
	//Parameter 2 : Action
	//Return value : Axis position
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	float InputManager::GetJoystickAxisPosition( INT8 _id, const char* _action )
	{
		if( _id < 0 || _id > sf::Joystick::Count || !m_joysticks[ _id ].isConnected )
			return 0.f;

		const ActionKey* pActionKey = GetActionKey( _action );

		INT8 iIndex = 0;
		for( unsigned int i = 0; i < pActionKey->m_oJoystickAxes.size(); ++i )
		{
			iIndex = pActionKey->m_oJoystickAxes[ i ];

			if( iIndex >= sf::Joystick::AxisCount )
				continue;

			if( fabs( m_joysticks[ _id ].axes[ iIndex ] - m_joysticks[ _id ].defaultAxisValues[ iIndex ] ) > m_joysticks[ _id ].axesDeadZone )
				return m_joysticks[ _id ].axes[ iIndex ];
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

	CustomBitmapGlyph* InputManager::GetBitmapGlyph( const std::string& _sActionOrKey, int _iIndex /*= 0*/ )
	{
		const ActionKey* pActionKey = GetActionKey( _sActionOrKey );
		std::string sGlyphName = "";

		if( pActionKey != nullptr )
		{
			if( _iIndex < (int)pActionKey->m_oKeys.size() )
			{
				sGlyphName = KeyToString( pActionKey->m_oKeys[ _iIndex ] );

				if( sGlyphName.empty() )
					sGlyphName = _sActionOrKey;
			}
		}
		else
			sGlyphName = _sActionOrKey;

		if( sGlyphName.empty() == false )
			return g_pFZN_DataMgr->GetBitmapGlyph( sGlyphName );

		return nullptr;
	}

	std::string InputManager::GetActionGlyphString( const std::string& _sAction, bool _bAllKeys )
	{
		const ActionKey* pActionKey = GetActionKey( _sAction );

		if( pActionKey == nullptr )
			return "";

		if( pActionKey->m_oKeys.size() == 1 || _bAllKeys == false )
			return Tools::Sprintf( "%%%%%s%%%%", _sAction.c_str() );

		std::string sRet = Tools::Sprintf( "%%%%%s,0%%%%", _sAction.c_str() );
		for( int iBind = 1; iBind < pActionKey->m_oKeys.size(); ++iBind )
		{
			sRet += Tools::Sprintf( " / %%%%%s,%d%%%%", _sAction.c_str(), iBind );
		}

		return sRet;
	}

	std::string InputManager::GetActionKeyString( const std::string& _sActionOrKey, int _iIndex /*= 0 */ )
	{
		const ActionKey* pActionKey = GetActionKey( _sActionOrKey );
		std::string sGlyphName = "";

		if( pActionKey != nullptr )
		{
			if( _iIndex < (int)pActionKey->m_oKeys.size() )
			{
				sGlyphName = KeyToString( pActionKey->m_oKeys[ _iIndex ] );

				if( sGlyphName.empty() )
					sGlyphName = _sActionOrKey;
			}
		}
		else
			sGlyphName = _sActionOrKey;

		if( sGlyphName.empty() == false )
			return "[" + sGlyphName + "]";

		return "";
	}

	const std::vector< fzn::ActionKey >& InputManager::GetActionKeys() const
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
		for( const fzn::ActionKey& oActionKey : m_oCustomActionKeys )
		{
			if( _sActionKey == oActionKey.m_sName )
				return &oActionKey;
		}

		return nullptr;
	}

	ActionKey* InputManager::GetActionKey( const std::string& _sActionKey )
	{
		for( fzn::ActionKey& oActionKey : m_oCustomActionKeys )
		{
			if( _sActionKey == oActionKey.m_sName )
				return &oActionKey;
		}

		return nullptr;
	}

	bool InputManager::ReplaceActionKeyBind( const std::string& _sActionKey, const BindType& _eBind, int _iIndex, bool _bSameCategoryOnly /*= true*/ )
	{
		if( _sActionKey.empty() || _eBind >= BindType::eNbTypes || _iIndex < 0 )
			return false;

		m_oActionKeyBindInfo.m_sActionKey = _sActionKey;
		m_oActionKeyBindInfo.m_iBindIndex = _iIndex;
		m_oActionKeyBindInfo.m_uBindTypeMask = ( 1 << _eBind );
		m_oActionKeyBindInfo.m_bSameCategoryOnly = _bSameCategoryOnly;

		return true;
	}

	bool InputManager::IsWaitingActionKeyBind() const
	{
		return m_oActionKeyBindInfo.m_sActionKey.empty() == false && m_oActionKeyBindInfo.m_iBindIndex >= 0 && m_oActionKeyBindInfo.m_uBindTypeMask != 0;
	}

	bool InputManager::IsWaitingInputForType( const BindType& _eBind ) const
	{
		return ( m_oActionKeyBindInfo.m_uBindTypeMask & 1 << _eBind ) != 0;
	}

	void InputManager::ResetActionKeys()
	{
		m_oCustomActionKeys = m_oDefaultActionKeys;
	}

	void InputManager::SaveCustomActionKeysToFile()
	{
		const std::string sFilePath = g_pFZN_Core->GetSaveFolderPath() + "/actionKeys.xml";

		tinyxml2::XMLDocument oDestFile;

		tinyxml2::XMLNode* pActions = oDestFile.NewElement( "Actions" );
		oDestFile.InsertEndChild( pActions );

		for( const fzn::ActionKey& oActionKey : m_oCustomActionKeys )
		{
			tinyxml2::XMLElement* pAction = oDestFile.NewElement( "Action" );

			pAction->SetAttribute( "Name", oActionKey.m_sName.c_str() );
			pAction->SetAttribute( "Category", oActionKey.m_iCategory );

			for( const sf::Keyboard::Key& eKey : oActionKey.m_oKeys )
			{
				tinyxml2::XMLElement* pInput = oDestFile.NewElement( "Input" );

				pInput->SetAttribute( "Type", "Keyboard" );
				pInput->SetAttribute( "Map", KeyToString( eKey ).c_str() );

				pAction->InsertEndChild( pInput );
			}

			for( const sf::Mouse::Button& eButton : oActionKey.m_oMouseButtons )
			{
				tinyxml2::XMLElement* pInput = oDestFile.NewElement( "Input" );

				pInput->SetAttribute( "Type", "Mouse" );
				pInput->SetAttribute( "Map", MouseButtonToString( eButton ).c_str() );

				pAction->InsertEndChild( pInput );
			}

			for( INT8 iButton : oActionKey.m_oJoystickButtons )
			{
				tinyxml2::XMLElement* pInput = oDestFile.NewElement( "Input" );

				pInput->SetAttribute( "Type", "JoystickButton" );
				pInput->SetAttribute( "Map", JoystickButtonToString( iButton ).c_str() );

				pAction->InsertEndChild( pInput );
			}

			for( const sf::Joystick::Axis& eAxis : oActionKey.m_oJoystickAxes )
			{
				tinyxml2::XMLElement* pInput = oDestFile.NewElement( "Input" );

				pInput->SetAttribute( "Type", "JoystickAxis" );
				pInput->SetAttribute( "Map", JoystickAxisToString( eAxis ).c_str() );

				pAction->InsertEndChild( pInput );
			}

			pActions->InsertEndChild( pAction );
		}

		tinyxml2::XMLError eError = oDestFile.SaveFile( sFilePath.c_str() );

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
	}

	void InputManager::AddInputToScan( sf::Keyboard::Key _eInput )
	{
		if( _eInput < 0 && _eInput >= sf::Keyboard::KeyCount )
			return;

		m_lScannedInputs[ 0 ].push_back( (int)_eInput );
	}

	void InputManager::AddInputToScan( sf::Mouse::Button _eInput )
	{
		if( _eInput < 0 && _eInput >= sf::Mouse::ButtonCount )
			return;

		m_lScannedInputs[ 1 ].push_back( (int)_eInput );
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

		m_pGetKeyChar[ 0 ] = &InputManager::GetENKeyChar;
		m_pGetKeyChar[ 1 ] = &InputManager::GetFRKeyChar;
		m_pGetUpperKeyChar[ 0 ] = &InputManager::GetENKeyUpperChar;
		m_pGetUpperKeyChar[ 1 ] = &InputManager::GetFRKeyUpperChar;
		m_pStringToKey[ 0 ] = &InputManager::StringToENKey;
		m_pStringToKey[ 1 ] = &InputManager::StringToFRKey;

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

			if( sf::Joystick::hasAxis( _id, ( sf::Joystick::Axis )axis ) )
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

				if( sf::Joystick::hasAxis( joyID, ( sf::Joystick::Axis )axis ) )
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
				}
				else defaultValue = TRUE;
			}
			else defaultValue = TRUE;

			if( defaultValue )
			{
				m_joysticks[ _id ].axes[ axis ] = 0.f;
				m_joysticks[ _id ].defaultAxisValues[ axis ] = 0.f;
				defaultValue = FALSE;
			}
			FZN_LOG( " > Axis %d default value : %f", axis, m_joysticks[ _id ].defaultAxisValues[ axis ] );
		}
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
			if( m_keyStates[ i ] >= 0 && m_keyStates[ i ] <= 3 )
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
					m_keyStates[ i ] = ( m_keyStates[ i ] == Status::Pressed || m_keyStates[ i ] == Status::Down ) ? Status::Released : Status::Up;
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
					( m_buttonStates[ i ] == Status::Pressed || m_buttonStates[ i ] == Status::Down ) ? m_buttonStates[ i ] = Status::Down : ( m_buttonStates[ i ] = Status::Pressed, m_bMouseHit = TRUE );
				else
					m_buttonStates[ i ] = ( m_buttonStates[ i ] == Status::Pressed || m_buttonStates[ i ] == Status::Down ) ? Status::Released : Status::Up;
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

							if( m_joysticks[ joyID ].defaultAxisValues == nullptr )
								CalibrateJoystick( joyID );
						}
						else
							m_joysticks[ joyID ].states[ button ] = ( m_joysticks[ joyID ].states[ button ] == Status::Pressed || m_joysticks[ joyID ].states[ button ] == Status::Down ) ? Status::Released : Status::Up;
					}
				}

				for( INT8 axis = 0; axis < sf::Joystick::AxisCount; ++axis )
				{
					m_joysticks[ joyID ].axes[ axis ] = m_joysticks[ joyID ].basicAxes[ axis ];

					if( m_joysticks[ joyID ].defaultAxisValues != nullptr && m_joysticks[ joyID ].axes[ axis ] != m_joysticks[ joyID ].defaultAxisValues[ axis ] && fabs( m_joysticks[ joyID ].axes[ axis ] ) > m_joysticks[ joyID ].axesDeadZone )
						m_bJoystickMoved = TRUE;
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
				pInputList[ iInputType ][ iCurrentKey ] = ( iInputType == 0 ) ? sf::Keyboard::isKeyPressed( ( sf::Keyboard::Key )iCurrentKey ) == true : sf::Mouse::isButtonPressed( ( sf::Mouse::Button )iCurrentKey ) == true;
			}
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
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Reads the XML file containing corresponding keys to each game action
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void InputManager::LoadActionKeysFromXML( std::vector< ActionKey >& _oActionKeysArray, const std::string& _sXMLPath )
	{
		tinyxml2::XMLDocument File;

		if( File.LoadFile( _sXMLPath.c_str() ) )
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

			ActionInput = Action->FirstChildElement( "Input" );

			while( ActionInput )
			{
				mapedKey = Tools::XMLStringAttribute( ActionInput, "Map" );

				if( Tools::XMLStringAttribute( ActionInput, "Type" ) ==  "Keyboard" )
					_AddKeyToActionKey( oNewActionKey, ( this->*m_pStringToKey[ m_bIsKeyboardFrench ] )( mapedKey ) );
				else if( Tools::XMLStringAttribute( ActionInput, "Type" ) ==  "Mouse" )
					_AddMouseButtonToActionKey( oNewActionKey, StringToMouseButton( mapedKey ) );
				else if( Tools::XMLStringAttribute( ActionInput, "Type" ) ==  "JoystickButton" )
					_AddJoystickButtonToActionKey( oNewActionKey, StringToJoystickButton( mapedKey ) );
				else if( Tools::XMLStringAttribute( ActionInput, "Type" ) ==  "JoystickAxis" )
					_AddJoystickAxisToActionKey( oNewActionKey, StringToJoystickAxis( mapedKey ) );

				ActionInput = ActionInput->NextSiblingElement();
			}

			_oActionKeysArray.push_back( oNewActionKey );
			Action = Action->NextSiblingElement();
		}
	}

	void InputManager::_AddKeyToActionKey( ActionKey& _oActionKey, const sf::Keyboard::Key& _eKey )
	{
		if( _eKey >= sf::Keyboard::Key::KeyCount )
			return;

		for( const sf::Keyboard::Key& eKey : _oActionKey.m_oKeys )
		{
			if( eKey == _eKey )
				return;
		}

		_oActionKey.m_oKeys.push_back( _eKey );
	}

	void InputManager::_AddMouseButtonToActionKey( ActionKey& _oActionKey, const sf::Mouse::Button& _eMouseButton )
	{
		if( _eMouseButton >= sf::Mouse::Button::ButtonCount )
			return;

		for( const sf::Mouse::Button& eMouseButton : _oActionKey.m_oMouseButtons )
		{
			if( eMouseButton == _eMouseButton )
				return;
		}

		_oActionKey.m_oMouseButtons.push_back( _eMouseButton );
	}

	void InputManager::_AddJoystickButtonToActionKey( ActionKey& _oActionKey, INT8 _iJoystickButton )
	{
		if( _iJoystickButton >= sf::Joystick::ButtonCount )
			return;

		for( INT8 iJoystickButton : _oActionKey.m_oJoystickButtons )
		{
			if( iJoystickButton == _iJoystickButton )
				return;
		}

		_oActionKey.m_oJoystickButtons.push_back( _iJoystickButton );
	}

	void InputManager::_AddJoystickAxisToActionKey( ActionKey& _oActionKey, const sf::Joystick::Axis& _eJoystickAxis )
	{
		if( _eJoystickAxis >= sf::Joystick::AxisCount )
			return;

		for( const sf::Joystick::Axis& eAxis : _oActionKey.m_oJoystickAxes )
		{
			if( eAxis == _eJoystickAxis )
				return;
		}

		_oActionKey.m_oJoystickAxes.push_back( _eJoystickAxis );
	}

	bool InputManager::_SetActionKeyBind_Key( const sf::Keyboard::Key& _eKey )
	{
		if( m_oActionKeyBindInfo.m_sActionKey.empty() || _eKey >= sf::Keyboard::Key::KeyCount )
			return false;

		bool bActionKeyFound = false;
		int iCategory = -1;
		for( fzn::ActionKey& oActionKey : m_oCustomActionKeys )
		{
			if( m_oActionKeyBindInfo.m_sActionKey == oActionKey.m_sName )
			{
				// If the key is already set, we don't need to change anything.
				for( const sf::Keyboard::Key& eKey : oActionKey.m_oKeys )
				{
					if( eKey == _eKey )
						return false;
				}

				if( m_oActionKeyBindInfo.m_bSameCategoryOnly )
					iCategory = oActionKey.m_iCategory;

				if( m_oActionKeyBindInfo.m_iBindIndex >= (int)oActionKey.m_oKeys.size() )
				{
					oActionKey.m_oKeys.push_back( _eKey );
					break;
				}

				for( int iKey = 0; iKey < (int)oActionKey.m_oKeys.size(); ++iKey )
				{
					if( iKey == m_oActionKeyBindInfo.m_iBindIndex )
					{
						oActionKey.m_oKeys[ iKey ] = _eKey;
						bActionKeyFound = true;
						break;
					}
				}
			}

			if( bActionKeyFound )
				break;
		}

		for( fzn::ActionKey& oActionKey : m_oCustomActionKeys )
		{
			if( oActionKey.m_sName == m_oActionKeyBindInfo.m_sActionKey )
				continue;

			if( iCategory >= 0 && oActionKey.m_iCategory != iCategory )
				continue;

			for( std::vector< sf::Keyboard::Key >::iterator itKey = oActionKey.m_oKeys.begin(); itKey != oActionKey.m_oKeys.end(); ++itKey )
			{
				if( (*itKey) == _eKey )
				{
					oActionKey.m_oKeys.erase( itKey );
					break;
				}
			}
		}

		m_oActionKeyBindInfo.Reset();
		return true;
	}

	bool InputManager::_SetActionKeyBind_MouseButton( const sf::Mouse::Button& _eMouseButton )
	{
		if( m_oActionKeyBindInfo.m_sActionKey.empty() || _eMouseButton >= sf::Mouse::Button::ButtonCount )
			return false;

		bool bActionKeyFound = false;
		int iCategory = -1;
		for( fzn::ActionKey& oActionKey : m_oCustomActionKeys )
		{
			if( m_oActionKeyBindInfo.m_sActionKey == oActionKey.m_sName )
			{
				for( const sf::Mouse::Button& eMouseButton : oActionKey.m_oMouseButtons )
				{
					if( eMouseButton == _eMouseButton )
						return false;
				}

				if( m_oActionKeyBindInfo.m_bSameCategoryOnly )
					iCategory = oActionKey.m_iCategory;

				if( m_oActionKeyBindInfo.m_iBindIndex >= (int)oActionKey.m_oMouseButtons.size() )
				{
					oActionKey.m_oMouseButtons.push_back( _eMouseButton );
					break;
				}

				for( int iButton = 0; iButton < (int)oActionKey.m_oMouseButtons.size(); ++iButton )
				{
					if( iButton == m_oActionKeyBindInfo.m_iBindIndex )
					{
						oActionKey.m_oMouseButtons[ iButton ] = _eMouseButton;
						bActionKeyFound = true;
						break;
					}
				}
			}

			if( bActionKeyFound )
				break;
		}

		for( fzn::ActionKey& oActionKey : m_oCustomActionKeys )
		{
			if( oActionKey.m_sName == m_oActionKeyBindInfo.m_sActionKey )
				continue;

			if( iCategory >= 0 && oActionKey.m_iCategory != iCategory )
				continue;

			for( std::vector< sf::Mouse::Button >::iterator itMouseButton = oActionKey.m_oMouseButtons.begin(); itMouseButton != oActionKey.m_oMouseButtons.end(); ++itMouseButton )
			{
				if( (*itMouseButton) == _eMouseButton )
				{
					oActionKey.m_oMouseButtons.erase( itMouseButton );
					break;
				}
			}
		}
		
		m_oActionKeyBindInfo.Reset();
		return true;
	}

	bool InputManager::_SetActionKeyBind_JoystickButton( INT8 _iJoystickButton )
	{
		if( m_oActionKeyBindInfo.m_sActionKey.empty() || _iJoystickButton >= sf::Joystick::ButtonCount )
			return false;

		bool bActionKeyFound = false;
		int iCategory = -1;
		for( fzn::ActionKey& oActionKey : m_oCustomActionKeys )
		{
			if( m_oActionKeyBindInfo.m_sActionKey == oActionKey.m_sName )
			{
				for( INT8 iButton : oActionKey.m_oJoystickButtons )
				{
					if( iButton == _iJoystickButton )
						return false;
				}

				if( m_oActionKeyBindInfo.m_bSameCategoryOnly )
					iCategory = oActionKey.m_iCategory;

				if( m_oActionKeyBindInfo.m_iBindIndex >= (int)oActionKey.m_oJoystickButtons.size() )
				{
					oActionKey.m_oJoystickButtons.push_back( _iJoystickButton );
					break;
				}

				for( int iButton = 0; iButton < (int)oActionKey.m_oJoystickButtons.size(); ++iButton )
				{
					if( iButton == m_oActionKeyBindInfo.m_iBindIndex )
					{
						oActionKey.m_oJoystickButtons[ iButton ] = _iJoystickButton;
						bActionKeyFound = true;
						break;
					}
				}
			}

			if( bActionKeyFound )
				break;
		}

		for( fzn::ActionKey& oActionKey : m_oCustomActionKeys )
		{
			if( oActionKey.m_sName == m_oActionKeyBindInfo.m_sActionKey )
				continue;

			if( iCategory >= 0 && oActionKey.m_iCategory != iCategory )
				continue;

			for( std::vector< INT8 >::iterator itJoystickButton = oActionKey.m_oJoystickButtons.begin(); itJoystickButton != oActionKey.m_oJoystickButtons.end(); ++itJoystickButton )
			{
				if( (*itJoystickButton) == _iJoystickButton )
				{
					oActionKey.m_oJoystickButtons.erase( itJoystickButton );
					break;
				}
			}
		}
		
		m_oActionKeyBindInfo.Reset();
		return true;
	}

	bool InputManager::_SetActionKeyBind_JoystickAxis( const sf::Joystick::Axis& _eJoystickAxis )
	{
		if( m_oActionKeyBindInfo.m_sActionKey.empty() || _eJoystickAxis >= sf::Joystick::AxisCount )
			return false;

		bool bActionKeyFound = false;
		int iCategory = -1;
		for( fzn::ActionKey& oActionKey : m_oCustomActionKeys )
		{
			if( m_oActionKeyBindInfo.m_sActionKey == oActionKey.m_sName )
			{
				for( const sf::Joystick::Axis& eAxis : oActionKey.m_oJoystickAxes )
				{
					if( eAxis == _eJoystickAxis )
						return false;
				}

				if( m_oActionKeyBindInfo.m_bSameCategoryOnly )
					iCategory = oActionKey.m_iCategory;

				if( m_oActionKeyBindInfo.m_iBindIndex >= (int)oActionKey.m_oJoystickAxes.size() )
				{
					oActionKey.m_oJoystickAxes.push_back( _eJoystickAxis );
					break;
				}

				for( int iAxis = 0; iAxis < (int)oActionKey.m_oJoystickAxes.size(); ++iAxis )
				{
					if( iAxis == m_oActionKeyBindInfo.m_iBindIndex )
					{
						oActionKey.m_oJoystickAxes[ iAxis ] = _eJoystickAxis;
						bActionKeyFound = true;
						break;
					}
				}
			}

			if( bActionKeyFound )
				break;
		}

		for( fzn::ActionKey& oActionKey : m_oCustomActionKeys )
		{
			if( oActionKey.m_sName == m_oActionKeyBindInfo.m_sActionKey )
				continue;

			if( iCategory >= 0 && oActionKey.m_iCategory != iCategory )
				continue;

			for( std::vector< sf::Joystick::Axis >::iterator itJoystickAxis = oActionKey.m_oJoystickAxes.begin(); itJoystickAxis != oActionKey.m_oJoystickAxes.end(); ++itJoystickAxis )
			{
				if( (*itJoystickAxis) == _eJoystickAxis )
				{
					oActionKey.m_oJoystickAxes.erase( itJoystickAxis );
					break;
				}
			}
		}
		
		m_oActionKeyBindInfo.Reset();
		return true;
	}

	/////////////////CHARS MANAGEMENT/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Returns the corresponding character to a key (FR Keyboard)
	//Parameter : Concerned key
	//Return value : Corresponding character
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	char InputManager::GetFRKeyChar( sf::Keyboard::Key _key )
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
	char InputManager::GetFRKeyUpperChar( sf::Keyboard::Key _key )
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
	char InputManager::GetENKeyChar( sf::Keyboard::Key _key )
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
	char InputManager::GetENKeyUpperChar( sf::Keyboard::Key _key )
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
	sf::Keyboard::Key InputManager::StringToFRKey( std::string& _key )
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
		else if( _key == "LControl" )	return sf::Keyboard::LControl;
		else if( _key == "LShift" )		return sf::Keyboard::LShift;
		else if( _key == "LAlt" )		return sf::Keyboard::LAlt;
		else if( _key == "LSystem" )	return sf::Keyboard::LSystem;
		else if( _key == "RControl" )	return sf::Keyboard::RControl;
		else if( _key == "RShift" )		return sf::Keyboard::RShift;
		else if( _key == "RAlt" )		return sf::Keyboard::RAlt;
		else if( _key == "RSystem" )	return sf::Keyboard::RSystem;
		else if( _key == "Menu" )		return sf::Keyboard::Menu;
		else if( _key == "Â°" )			return sf::Keyboard::LBracket;
		else if( _key == "Â¨" )			return sf::Keyboard::RBracket;
		else if( _key == "+" )			return sf::Keyboard::Equal;
		else if( _key == "Â£" )			return sf::Keyboard::SemiColon;
		else if( _key == "?" )			return sf::Keyboard::Comma;
		else if( _key == "." )			return sf::Keyboard::Period;
		else if( _key == "/" )			return sf::Keyboard::Slash;
		else if( _key == "Âµ" )			return sf::Keyboard::BackSlash;
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
	sf::Keyboard::Key InputManager::StringToENKey( std::string& _key )
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
		else if( _key == "LControl" )	return sf::Keyboard::LControl;
		else if( _key == "LShift" )		return sf::Keyboard::LShift;
		else if( _key == "LAlt" )		return sf::Keyboard::LAlt;
		else if( _key == "LSystem" )	return sf::Keyboard::LSystem;
		else if( _key == "RControl" )	return sf::Keyboard::RControl;
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

	std::string InputManager::KeyToString( const sf::Keyboard::Key& _key )
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
		case sf::Keyboard::LShift:		return "LShift";
		case sf::Keyboard::RShift:		return "RShift";
		default:						return "";
		};
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Returns the corresponding mouse button to a string
	//Parameter : Concerned string
	//Return value : Corresponding mouse button
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	sf::Mouse::Button InputManager::StringToMouseButton( std::string& _button )
	{
		if( _button == "Left" )					return sf::Mouse::Left;
		else if( _button == "Right" )			return sf::Mouse::Right;
		else if( _button == "Middle" )			return sf::Mouse::Middle;
		else if( _button == "ExtraButton1" )	return sf::Mouse::XButton1;
		else if( _button == "ExtraButton2" )	return sf::Mouse::XButton2;
		else									return sf::Mouse::ButtonCount;
	}

	std::string InputManager::MouseButtonToString( const sf::Mouse::Button& _eButton )
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
	INT8 InputManager::StringToJoystickButton( std::string& _button )
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
		else if( _button == "B32" )		return 32;
		else							return -1;
	}

	std::string InputManager::JoystickButtonToString( INT8 _iButton )
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
		case 32:	return "B32";
		default:	return "";
		};
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Returns the corresponding joystick axis to a string
	//Parameter : Concerned string
	//Return value : Corresponding joystick axis
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	sf::Joystick::Axis InputManager::StringToJoystickAxis( std::string& _button )
	{
		if( _button == "X" )				return sf::Joystick::Axis::X;
		else if( _button == "Y" )			return sf::Joystick::Axis::Y;
		else if( _button == "Z" )			return sf::Joystick::Axis::Z;
		else if( _button == "R" )			return sf::Joystick::Axis::R;
		else if( _button == "U" )			return sf::Joystick::Axis::U;
		else if( _button == "V" )			return sf::Joystick::Axis::V;
		else if( _button == "PovX" )		return sf::Joystick::Axis::PovX;
		else if( _button == "PovY" )		return sf::Joystick::Axis::PovY;
		else								return sf::Joystick::Axis::X;
	}
	std::string InputManager::JoystickAxisToString( const sf::Joystick::Axis& _eAxis )
	{
		switch( _eAxis )
		{
		case sf::Joystick::Axis::X:		return "X";
		case sf::Joystick::Axis::Y:		return "Y";
		case sf::Joystick::Axis::Z:		return "Z";
		case sf::Joystick::Axis::R:		return "R";
		case sf::Joystick::Axis::U:		return "U";
		case sf::Joystick::Axis::V:		return "V";
		case sf::Joystick::Axis::PovX:	return "PovX";
		case sf::Joystick::Axis::PovY:	return "PovY";
		default:						return "";
		};
	}
} //namespace fzn

void FctInputMgrEvent( void* _pData )
{
	( (fzn::InputManager*)_pData)->OnEvent();
}
