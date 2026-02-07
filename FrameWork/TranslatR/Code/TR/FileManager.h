#pragma once

#include <FZN/Managers/LocalisationManager.h>


namespace TR
{
	/************************************************************************
	* @brief All the informations about the current localisation project.
	************************************************************************/
	struct Project
	{
		/**
		* @brief Clear all paths.
		**/
		void clear() { m_project_path.clear(); m_entries_path.clear(); m_enum_file_path.clear(); }

		std::string m_project_path{};		// The path to the project.
		std::string m_entries_path{};		// The path to the entries json file.
		std::string m_enum_file_path{};		// The path that will be used to generate the enum file.
	};

	/************************************************************************
	* @brief File opening and saving management.
	************************************************************************/
	class FileManager
	{
	public:
		/************************************************************************
		* INTERFACE
		************************************************************************/

		/**
		* @brief Manage the File Manager part of the menu bar in the interface.
		* @param [in,out] _loc_data The entries and languages of the current project.
		**/
		void display_menu_bar_items( fzn::Localisation::LocalisationData& _loc_data );


	private:
		/************************************************************************
		* ENTRIES
		************************************************************************/

		/**
		* @brief Show an open file dialog to select the entries file and fill the given localisation data with it.
		* @param [out] _loc_data The localisation data to be filled.
		**/
		void _open_entries_file( fzn::Localisation::LocalisationData& _loc_data );
		/**
		* @brief Save the given localisation data to the previously selected path.
		* @param [out] _loc_data The localisation data to be saved.
		**/
		void _save_entries( fzn::Localisation::LocalisationData& _loc_data ) const;
		/**
		* @brief Show a save file dialog to select where to save the given localisation data.
		* @param [out] _loc_data The localisation data to be saved.
		**/
		void _save_entries_as( fzn::Localisation::LocalisationData& _loc_data );
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
		/**
		* @brief Close the current entries file, clearing all entries and the path to their file.
		* @param [out] _loc_data The localisation data to be cleared.
		**/
		void _close_entries( fzn::Localisation::LocalisationData& _loc_data );


		/************************************************************************
		* ENUM FILE
		************************************************************************/

		/**
		* @brief Generate the enum file using the given localisation data to the previously selected path.
		* @param [out] _loc_data The localisation data to be used.
		**/
		void _generate_enum_file( fzn::Localisation::LocalisationData& _loc_data ) const;
		/**
		* @brief Show a fave ile dialog to select where to generate the enum file using the given localisation data.
		* @param [out] _loc_data The localisation data to be used.
		**/
		void _generate_enum_file_as( fzn::Localisation::LocalisationData& _loc_data );


		/************************************************************************
		* PROJECT
		************************************************************************/

		/**
		* @brief Show an open file dialog to select the project file and load the corresponding entries file to fill the given localisation data with it.
		* @param [out] _loc_data The localisation data to be filled.
		**/
		void _open_project_file( fzn::Localisation::LocalisationData& _loc_data );
		/**
		* @brief Save the current project to the previously selected path.
		**/
		void _save_project() const;
		/**
		* @brief Show a save file dialog to select where to save the current project.
		**/
		void _save_project_as();
		/**
		* @brief Close the current project, clearing all save paths and loc data.
		* @param [out] _loc_data The localisation data to be cleared.
		**/
		void _close_project( fzn::Localisation::LocalisationData& _loc_data );

	public:
		Project			m_project;					// The currently opened project.

		StringVector	m_recent_entries_paths;		// The last opened entries files path.
		StringVector	m_recent_enum_file_paths;	// The last generated enum files path.
	};
}
