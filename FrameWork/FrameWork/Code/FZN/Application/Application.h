#pragma once

#include <functional>

#include <Externals/ImGui/imgui.h>

#include <SFML/System.hpp>

#include "FZN/Defines.h"


namespace fzn
{
	/************************************************************************
	* @brief Common root of all ImGui Applications. Handles window creation and display.
	************************************************************************/
	class FZN_EXPORT Application : public sf::NonCopyable
	{
	public:
		Application();

		/**
		* @brief Main display function. Create the window and calls overridable display functions.
		**/
		void display();

	protected:
		virtual void _handle_actions() {}
		virtual void _push_style_vars_and_colors();

		void _display_menu_bar();
		virtual void _custom_menu_bar() {}
		void _menu_item( const char* _label, std::function< void( void ) > _fct, bool _enabled = true, const char* _shortcut = nullptr );
		void _display_version();

		virtual void _custom_display() = 0;

	protected:
		ImGuiWindowFlags m_window_flags;

	private:
		bool m_pop_default_style_vars{ false };
	};
} // fzn
