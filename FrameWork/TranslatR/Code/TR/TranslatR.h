#pragma once

#include <Externals/json/json.h>

#include <FZN/Defines.h>
#include <FZN/Managers/LocalisationManager.h>

#include "FileManager.h"


namespace TR
{
	/************************************************************************
	* @brief ImGui interface displaying all the words and their translations in all the available languages.
	************************************************************************/
	class TranslatR
	{
	public:
		TranslatR();
		~TranslatR();

		/**
		* @brief Main ImGui display.
		**/
		void display();

	private:
		/**
		* @brief Display the window menu bar.
		**/
		void _display_menu_bar();
		/**
		* @brief Display and manage available languages.
		**/
		void _display_languages();
		/**
		* @brief Display and manage localisation entries.
		**/
		void _display_entries();
		/**
		* @brief Display the last table line that is used to add an entry to the localisation.
		* @param _row_id The last table row number
		**/
		void _display_new_entry( uint32_t _row_id );

		void _show_new_language_popup();
		void _add_language_popup();
		void _add_new_languages();
		void _remove_language( uint32_t _language_id );
		
		void _create_json();

	private:
		fzn::Localisation::LocalisationData	m_loc_data;

		FileManager		m_file_manager;

		std::string		m_new_entry{};
		StringVector	m_new_languages{};
		std::string		m_new_language{};
		bool			m_show_new_language_popup{ false };
	};
}
