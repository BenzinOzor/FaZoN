#pragma once

#include <Externals/json/json.h>

#include <FZN/Defines.h>
#include <FZN/Managers/LocalisationManager.h>


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

		void _create_json();
		void _save_json();
		void _save_json_as();
		void _load_json();

		void _write_languages( Json::Value& _root );
		void _write_entries( Json::Value& _root );

		void _generate_enum_file();
		void _generate_enum_file_as();

	private:
		fzn::Localisation::Entries	m_entries;
		StringVector				m_languages;

		std::string					m_entries_path{};
		std::string					m_enum_file_path{};
	};
}
