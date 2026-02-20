#pragma once

#include <functional>

#include <SFML/System/Vector2.hpp>

#include <Externals/ImGui/imgui.h>
#include <Externals/json/json.h>

#include <FZN/Managers/InputManager.h>
#include <FZN/Tools/Event.h>


namespace fzn
{
	class FZN_EXPORT AppOptions
	{
	public:
		AppOptions();

		/**
		* @brief Prepare the option window to be displayed. Setup its first state.
		**/
		virtual void open_options();
		/**
		* @brief Main display fonction.
		**/
		void display();
		/**
		* @brief Event management function.
		**/
		void on_event();

	protected:
		/**
		* @brief Main display function for custom options data. Called by display().
		**/
		virtual void _display_custom_options() = 0;
		/**
		* @brief Custom event management function. Called by on_event().
		* @param _sf_event Event coming from SFML.
		* @param _fzn_event Event coming from FaZoN (including custom user events).
		**/
		virtual void _on_custom_event( sf::Event& _sf_event, fzn::Event& _fzn_event ) {}
		/**
		* @brief Binding display function.
		**/
		void _display_bindings();

		/**
		* @brief Validate edited options and close the window.
		**/
		virtual void _confirm_options();
		/**
		* @brief Cancel edited options and restore old ones, then close window.
		**/
		virtual void _cancel_options();

		/**
		* @brief Load options data from default options.json in the save folder path.
		**/
		void _load_options();
		/**
		* @brief Custom load function called by the default one to let user read the given json root.
		* @param _root The json root to read the custom data it contains.
		**/
		virtual void _load_options_from_json( Json::Value& _root ) = 0;
		/**
		* @brief Save options data to default options.json in the save folder path.
		**/
		void _save_options();
		/**
		* @brief Custom save function called by the default one to let user fill the given json root.
		* @param _root The json root to fill with custom data.
		**/
		virtual void _save_options_to_json( Json::Value& _root ) = 0;
		

		/**
		* @brief Creation of the table containing the options fields.
		* The size of the first column is a ratio of the option window size, both setable via member variables.
		* @return True if the table is valid.
		**/
		bool _begin_option_table() const;
		/**
		* @brief Display a label in the first column, with or without a helper tooltip.
		* @param _label The label to display.
		* @param _tooltip The tooltip to display when hovering the helper. If no tooltip is given, the helper won't be displayed.
		**/
		void _first_column_label( std::string_view _label, std::string_view _tooltip = {} );
		/**
		* @brief Display a colored label in the first column, with or without a helper tooltip.
		* @param _color The color for the label.
		* @param _label The label to display.
		* @param _tooltip The tooltip to display when hovering the helper. If no tooltip is given, the helper won't be displayed.
		**/
		void _first_column_colored_label( const ImVec4& _color, std::string_view _label, std::string_view _tooltip = {} );
		/**
		* @brief Display a widget in the first column, with or without a helper tooltip.
		* @param _widget_fct The widget to display.
		* @param _tooltip The tooltip to display when hovering the helper. If no tooltip is given, the helper won't be displayed.
		**/
		void _first_column_widget( std::function<bool( void )> _widget_fct, std::string_view _tooltip = {} );
		/**
		* @brief Display a widget in the second column, with or without a helper tooltip.
		* @param _widget_fct The widget to display.
		* @param _width The custom width of the widget. 0 will use the default size set by ImGui.
		* @param _tooltip The tooltip to display when hovering the helper. If no tooltip is given, the helper won't be displayed.
		**/
		void _second_column_widget( std::function<bool( void )> _widget_fct, float _width = 0.f, std::string_view _tooltip = {} );

	protected:
		bool	m_show_window{ false };					// Bool indicating if the option window must be shown or not. Set true by open_options() and false by display() when the user closes the window.
		bool	m_edited{ false };						// Indicate if a change has been made in the options or not.
		ImVec2	m_app_window_size{};					// The size of the application window.
		ImVec2	m_option_window_size{};					// The size of the option window.
		float	m_table_first_column_ratio{ 0.45f };	// The first column of the option table will be the size of the window multiplied by this ratio.
		float	m_first_column_width{ 0.f };			// The width of the first column, calculated each frame from the available space in the window and the ratio above.
	};
} // fzn
