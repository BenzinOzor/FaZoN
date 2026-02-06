#pragma once

#include <FZN/Managers/LocalisationManager.h>


namespace TR
{
	/************************************************************************
	* @brief All the informations about the current localisation project.
	************************************************************************/
	struct Project
	{
		std::string m_entries_path{};		// The path to the entries json file.
		std::string m_enum_file_path{};		// The path that will be used to generate the enum file.
	};

	/************************************************************************
	* @brief File opening and saving management.
	************************************************************************/
	class FileManager
	{
	public:
		/**
		* @brief Manage the File Manager part of the menu bar in the interface.
		* @param [in,out] _loc_data The entries and languages of the current project.
		**/
		void display_menu_bar_items( fzn::Localisation::LocalisationData& _loc_data );

		/**
		* @brief Show an open file dialog to select the entries file and fill the given localisation data with it.
		* @param [out] _loc_data The localisation data to be filled.
		**/
		void open_entries_file( fzn::Localisation::LocalisationData& _loc_data );
		/**
		* @brief Save the given localisation data to the previously selected path.
		* @param [out] _loc_data The localisation data to be saved.
		**/
		void save_entries( fzn::Localisation::LocalisationData& _loc_data ) const;
		/**
		* @brief Show a save file dialog to select where to save the given localisation data.
		* @param [out] _loc_data The localisation data to be saved.
		**/
		void save_entries_as( fzn::Localisation::LocalisationData& _loc_data );

		/**
		* @brief Generate the enum file using the given localisation data to the previously selected path.
		* @param [out] _loc_data The localisation data to be used.
		**/
		void generate_enum_file( fzn::Localisation::LocalisationData& _loc_data ) const;
		/**
		* @brief Show a fave ile dialog to select where to generate the enum file using the given localisation data.
		* @param [out] _loc_data The localisation data to be used.
		**/
		void generate_enum_file_as( fzn::Localisation::LocalisationData& _loc_data );

	private:
		/**
		* @brief Write the languages section of the given localisation data in the given json root.
		* @param [in,out] _root The json root to fill with the data.
		* @param _loc_data The localisation data to use.
		**/
		void _write_languages( Json::Value& _root, const fzn::Localisation::LocalisationData& _loc_data ) const;
		/**
		* @brief Write the entries section of the given localisation data in the given json root.
		* @param [in,out] _root The json root to fill with the data.
		* @param _loc_data The localisation data to use.
		**/
		void _write_entries( Json::Value& _root, const fzn::Localisation::LocalisationData& _loc_data ) const;

	public:
		Project			m_project;					// The currently opened project.

		StringVector	m_recent_entries_paths;		// The last opened entries files path.
		StringVector	m_recent_enum_file_paths;	// The last generated enum files path.
	};
}
