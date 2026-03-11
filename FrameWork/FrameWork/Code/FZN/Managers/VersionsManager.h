#pragma once

#include <string>

#include <SFML/System.hpp>

#include "FZN/Defines.h"


namespace fzn
{
	/************************************************************************
	* @brief A version of something. Containing its name, a loc ID if needed for display purposes.
	* Also contains a version number for a Major.Minor.Patch format or a string to indicate a custom version directly.
	************************************************************************/
	struct FZN_EXPORT Version
	{
		/**
		* @brief Get the item name as entered in the .json file.
		* @return A string view containing the name.
		**/
		std::string_view get_item_name() const { return m_item_name; }
		/**
		* @brief Look for the localised name of the item using the loc ID read in the .json file.
		* @return A string view to the found name. An empty string if nothing was found.
		**/
		std::string_view get_localised_item_name() const;
		/**
		* @brief Create a string containing the version of the item.
		* @return The custom string if one was given, or the formatted major minor and patch numbers.
		**/
		std::string get_version() const;

		std::string		m_item_name{};					// The name of the versionned item.
		uint32_t		m_loc_ID{ Uint32_Max };		// The loc ID we can use to display the name.

		uint32_t		m_major{ Uint32_Max };		// The major number of the version. Increased when making breaking changes.
		uint32_t		m_minor{ Uint32_Max };		// The minor number of the version. Increased when adding a feature to the project.
		uint32_t		m_patch{ Uint32_Max };		// The patch number of the version. Increased when fixing a bug.
		std::string		m_string_version{};			// A custom string version of the versionned item (i.e. 1.12-alpha)
	};
	using Versions = std::vector< Version >;

	/************************************************************************
	* @brief The class containing all the versions of the elements the current project uses. Version informations are read from a .json in the project files.
	************************************************************************/
	class FZN_EXPORT VersionsManager : public sf::NonCopyable
	{
	public:
		VersionsManager();
		~VersionsManager();

		/**
		* @brief Read elements versions from the versions file. Invalid versions (no name or no valid version number/string) will be ignored.
		* @param _path The path to the versions definition file.
		**/
		void open_versions_file( std::string_view _path );

		/**
		* @brief Retrieve the version informations of a given item.
		* @param _item The name of the item.
		* @return A pointer to the struct containing the informations about the item. nullptr if nothing was found.
		**/
		const Version* get_version( std::string_view _item ) const;
		/**
		* @brief Get all the versionned items of the project.
		* @return A reference to the versions vector.
		**/
		const Versions& get_versions() const { return m_versions; }

	private:
		Versions m_versions;
	};
} // fzn

extern FZN_EXPORT fzn::VersionsManager* g_pFZN_VersionsMgr;
