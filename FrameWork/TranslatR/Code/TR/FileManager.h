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
		FileManager();
		~FileManager();

		/**
		* @brief Event handling.
		**/
		void on_event();

		/************************************************************************
		* INTERFACE
		************************************************************************/

		/**
		* @brief Manage the File Manager part of the menu bar in the interface.
		* @param [in,out] _loc_data The entries and languages of the current project.
		**/
		void display_menu_bar_items( fzn::Localisation::LocalisationData& _loc_data );


	private:
		enum FileType
		{
			project,
			entries,
			enum_file,
			COUNT
		};
		using PathDeque = std::deque< std::string >;
		using PathArray = std::array< PathDeque, FileType::COUNT >;


		/************************************************************************
		* ENTRIES
		************************************************************************/

		/**
		* @brief Show an open file dialog to select the entries file and fill the given localisation data with it.
		* @param [out] _loc_data The localisation data to be filled.
		**/
		void _open_entries_file( fzn::Localisation::LocalisationData& _loc_data );
		/**
		* @brief Show an open file dialog to select the entries file and fill the given localisation data with it.
		* @param _path The path to the entries file.
		* @param [out] _loc_data The localisation data to be filled.
		**/
		void _open_entries_file( std::string_view _path, fzn::Localisation::LocalisationData& _loc_data );
		/**
		* @brief Save the given localisation data to the previously selected path.
		* @param [out] _loc_data The localisation data to be saved.
		**/
		void _save_entries( fzn::Localisation::LocalisationData& _loc_data );
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
		void _generate_enum_file( fzn::Localisation::LocalisationData& _loc_data );
		/**
		* @brief Show a fave ile dialog to select where to generate the enum file using the given localisation data.
		* @param [out] _loc_data The localisation data to be used.
		**/
		void _generate_enum_file_as( fzn::Localisation::LocalisationData& _loc_data );
		/**
		* @brief Show a fave ile dialog to select where to generate the enum file using the given localisation data.
		* @param _path The path to the enum file.
		* @param [out] _loc_data The localisation data to be used.
		**/
		void _generate_enum_file_as( std::string_view _path, fzn::Localisation::LocalisationData& _loc_data );


		/************************************************************************
		* PROJECT
		************************************************************************/

		/**
		* @brief Show an open file dialog to select the project file and load the corresponding entries file to fill the given localisation data with it.
		* @param [out] _loc_data The localisation data to be filled.
		**/
		void _open_project_file( fzn::Localisation::LocalisationData& _loc_data );
		/**
		* @brief Show an open file dialog to select the project file and load the corresponding entries file to fill the given localisation data with it.
		* @param _path The path to the project file.
		* @param [out] _loc_data The localisation data to be filled.
		**/
		void _open_project_file( std::string_view _path, fzn::Localisation::LocalisationData& _loc_data );
		/**
		* @brief Save the current project to the previously selected path.
		**/
		void _save_project();
		/**
		* @brief Show a save file dialog to select where to save the current project.
		**/
		void _save_project_as();
		/**
		* @brief Close the current project, clearing all save paths and loc data.
		* @param [out] _loc_data The localisation data to be cleared.
		**/
		void _close_project( fzn::Localisation::LocalisationData& _loc_data );


		/************************************************************************
		* RECENT PATHS
		************************************************************************/

		/**
		* @brief Add a path to the history.
		* @param _file_type The file type of the path.
		* @param _path The path to add.
		**/
		void _add_recent_path( FileType _file_type, std::string_view _path );
		/**
		* @brief Save the recent paths to the file.
		**/
		void _save_recent_paths() const;
		/**
		* @brief Open the recent paths file to retrieve them.
		**/
		void _load_recent_paths();
		/**
		* @brief Remove a path from its corresponding deque.
		* @param _file_type The file type of the path.
		* @param _path_id The id of the path in the deque.
		**/
		void _remove_recent_path( FileType _file_type, uint32_t _path_id );
		/**
		* @brief Remove all paths from history.
		* @param _type_to_clear The type of file that needs clearing. COUNT will clear everything.
		**/
		void _clear_history( FileType _type_to_clear );


		/************************************************************************
		* MISC
		************************************************************************/

		/**
		* @brief Generic function that display recent paths of a given type and manage them.
		* @param _file_type The type of recent paths to display.
		* @param _loc_data The localisation data to manage.
		**/
		void _display_recent_paths( FileType _file_type, fzn::Localisation::LocalisationData& _loc_data );

	public:
		Project		m_project;					// The currently opened project.

		uint32_t	m_max_recent_paths{ 10 };	// The maximum number of recent path to keep in memory for each type of path (project, entries, enum file).
		PathArray	m_recent_paths;				// Array containing the recent paths used for each type of file.
	};
}
