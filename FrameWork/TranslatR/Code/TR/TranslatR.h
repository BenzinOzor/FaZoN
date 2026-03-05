#pragma once

#include <Externals/json/json.h>

#include <FZN/Defines.h>
#include <FZN/Managers/LocalisationManager.h>

#include "TR/FileManager.h"
#include "TR/Options.h"


namespace TR
{
	/************************************************************************
	* @brief ImGui interface displaying all the words and their translations in all the available languages.
	************************************************************************/
	class TranslatR
	{
	public:
		struct Cell
		{
			void reset() { m_row = Uint32_Max; m_column = Uint32_Max; }
			bool operator==( const Cell& _other ) const { return m_row == _other.m_row && m_column == _other.m_column; }

			uint32_t m_row{ Uint32_Max };
			uint32_t m_column{ Uint32_Max };
		};

		TranslatR();
		~TranslatR();

		/**
		* @brief Main ImGui display.
		**/
		void display();

	private:
		/**
		* @brief Actions tracking and handling.
		**/
		void _handle_actions();

		/**
		* @brief Display the window menu bar.
		**/
		void _display_menu_bar();
		/**
		* @brief Display and manage localisation entries.
		**/
		void _display_entries();
		/**
		* @brief Display the last table line that is used to add an entry to the localisation.
		* @param _row_id The last table row number
		**/
		void _display_new_entry( uint32_t _row_id );
		void _test_entry( uint32_t _row_id );
		bool table_cell( const Cell& _current_cell, Cell& _selected_cell, std::string& _cell_text, std::string_view _hint, bool _new_entry );

		/**
		* @brief Enable the new language popup and clear new language strings.
		**/
		void _show_new_language_popup();
		/**
		* @brief Display and manage the new language popup.
		**/
		void _add_language_popup();
		/**
		* @brief Validate the new languages entered in the popup and add them to the projects available languages.
		**/
		void _add_new_languages();
		/**
		* @brief Remove the given language from the projects available ones and from all the entries.
		* @param _language_id The ID of the language in the languages vector.
		**/
		void _remove_language( uint32_t _language_id );
		/**
		* @brief Remove all the languages from the project and clear them from all the entries.
		**/
		void _remove_all_languages();

	private:
		fzn::Localisation::LocalisationData	m_loc_data;			// Current project localisation data.

		FileManager		m_file_manager;
		Options			m_options;

		std::string		m_new_entry{};							// The empty entry displayed after the ones in the data. It will be added in the entries list once validated and cleared to allow for the next addition.
		StringVector	m_new_languages{};						// The languages added by the popup.
		std::string		m_new_language{};						// The language to add in the languages list of the popup.
		bool			m_show_new_language_popup{ false }; 	// True if we are currently displaying the language addition popup.

		Cell m_selected_cell;
		ImGuiID m_active_item{ Uint32_Max };
		bool selection_moved{ false };
	};
}
