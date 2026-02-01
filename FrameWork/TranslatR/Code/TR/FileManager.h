#pragma once

#include <FZN/Managers/LocalisationManager.h>


namespace TR
{
	/************************************************************************
	* @brief File opening and saving management.
	************************************************************************/
	class FileManager
	{
	public:
		void open_entries_file( fzn::Localisation::LocalisationData& _loc_data );
		void save_entries( fzn::Localisation::LocalisationData& _loc_data );
		void save_entries_as( fzn::Localisation::LocalisationData& _loc_data );

		void generate_enum_file( fzn::Localisation::LocalisationData& _loc_data );
		void generate_enum_file_as( fzn::Localisation::LocalisationData& _loc_data );

	private:
		void _write_languages( Json::Value& _root, fzn::Localisation::LocalisationData& _loc_data );
		void _write_entries( Json::Value& _root, fzn::Localisation::LocalisationData& _loc_data );

	public:
		std::string					m_entries_path{};
		std::string					m_enum_file_path{};

		StringVector				m_recent_entries_paths;
		StringVector				m_recent_enum_file_paths;
	};
}
