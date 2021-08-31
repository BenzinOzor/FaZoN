#pragma once

#include <string>

#include <SFML/Window/Keyboard.hpp>


namespace fzn
{
	class Event
	{
	public:
		enum Type
		{
			eActionKeyBindDone,		// Input manager just finished binding a button on an action key.	(no data)
			eToggleFullScreen,		// Full screen mode has been switched on or off.					(m_oFillScreen)
			eKeyPressed,			// Keyboard key has just been pressed.								(m_oKeyPressed)
			eUserEvent,				// Custom event of the user.										(m_pUserData)
			eNbTypes,
		};

		struct FullScreenEvent
		{
			int		m_iWindowIndex = -1;
			bool	m_bIsFullScreen = false;
		};

		struct KeyPressedEvent
		{
			sf::Keyboard::Key	m_eKey		= sf::Keyboard::KeyCount;
			char				m_cKeyChar	= '\0';
		};

		Event()
		: m_eType( Type::eNbTypes )
		{}

		Event( const Type& _eType )
		: m_eType( _eType )
		{}

		Type m_eType = Type::eNbTypes;

		union
		{
			FullScreenEvent m_oFullScreen;			// Full screen event parameters.	(Event::eToggleFullScreen)
			KeyPressedEvent m_oKeyPressed;			// Key pressed event parameters.	(Event::eKeyPressed)
			void*			m_pUserData = nullptr;	// Custom user event.				(Event::eUserEvent)
		};
	};
}
