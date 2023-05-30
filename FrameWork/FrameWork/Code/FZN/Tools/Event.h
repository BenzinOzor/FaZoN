#pragma once

#include <string>

#include <SFML/Window/Keyboard.hpp>

#include "FZN/Managers/InputManager.h"


namespace fzn
{
	class Event
	{
	public:
		enum Type
		{
			eActionKeyBindDone,		// Input manager just finished binding a button on an action key.		(no data)
			eToggleFullScreen,		// Full screen mode has been switched on or off.						(m_oFillScreen)
			eKeyPressed,			// Keyboard key has just been pressed.									(m_oKeyPressed)
			eChangedDevice,			// User changed from keyboard to controller or the other way around.	(m_oChangedDevice)
			eFileLoaded,			// A file has just been loaded by the data manager.						(m_oFileLoading)
			eBeginFileLoading,		// Beginning of a file loading task.									(m_oFileLoading)
			eActionEvent,			// An action has been pressed or released.								(m_oActionEvent)

			eUserEvent,				// Custom event of the user.											(m_pUserData)
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

		struct ChangedDevice
		{
			bool m_bUsingKeyboard = true;
		};

		struct FileLoading
		{
			/*std::string m_sFileName			= "";
			std::string m_sFilePath			= "";*/
			int			m_iNbFilesToLoad	= 0;
		};

		struct ActionEvent
		{
			const ActionKey*		m_pAction;
			char*					m_sAction;
			InputManager::Status	m_eActionStatus;
		};

		Event( Type _eType = Type::eNbTypes ):
			m_eType( _eType )
		{}

		Event( void* _pUserData ):
			m_eType( Type::eUserEvent ),
			m_pUserData( _pUserData )
		{}

		Type m_eType = Type::eNbTypes;

		union
		{
			FullScreenEvent m_oFullScreen;			// Full screen event parameters.	(Event::eToggleFullScreen)
			KeyPressedEvent m_oKeyPressed;			// Key pressed event parameters.	(Event::eKeyPressed)
			ChangedDevice	m_oChangedDevice;		// Changed device event parameters.	(Event::eChangedDevice)
			FileLoading		m_oFileLoading;			// File loaded event parameters.	(Event::eFileLoaded, Event::eBeginFileLoading)
			ActionEvent		m_oActionEvent;			// Action event parameters			(Event::eActionEvent)

			void*			m_pUserData = nullptr;	// Custom user event.				(Event::eUserEvent)
		};
	};
}
