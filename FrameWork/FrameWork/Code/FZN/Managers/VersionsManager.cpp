#include "FZN/Managers/VersionsManager.h"
#include "FZN/Managers/LocalisationManager.h"
#include "FZN/Tools/Tools.h"


FZN_EXPORT fzn::VersionsManager* g_pFZN_VersionsMgr = nullptr;

namespace fzn
{

	std::string_view Version::get_localised_item_name() const
	{
		return g_pFZN_LocMgr->get_string( m_loc_ID );
	}

	std::string Version::get_version() const
	{
		if( m_string_version.empty() == false )
			return m_string_version;

		return fzn::Tools::Sprintf( "%d.%d.%d", m_major, m_minor, m_patch );
	}

	VersionsManager::VersionsManager()
	{
		g_pFZN_VersionsMgr = this;
	}

	VersionsManager::~VersionsManager()
	{
		m_versions.clear();
	}

	/**
	* @brief Read elements versions from the versions file. Invalid versions (no name or no valid version number/string) will be ignored.
	* @param _path The path to the versions definition file.
	**/
	void VersionsManager::open_versions_file( std::string_view _path )
	{
		auto file = std::ifstream{ _path.data() };

		if( file.is_open() == false )
		{
			FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "Couldn't open file: %s", _path.data() );
			return;
		}
		else
			FZN_LOG( "Loading version at %s...", _path.data() );

		auto root = Json::Value{};
		file >> root;

		if( root[ "versions" ].isNull() )
		{
			FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "Invalid versions file: %s", _path.data() );
			return;
		}

		m_versions.clear();

		Json::Value versions = root[ "versions" ];

		m_versions.reserve( versions.size() );
		uint32_t item_id{ 0 };

		for( Json::Value::iterator it_version = versions.begin(); it_version != versions.end(); ++it_version )
		{
			const Json::Value& json_item{ *it_version };

			if( json_item[ "name" ].asString().empty() )
			{
				FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "Item #%d has no name, it will be ignored.", item_id );
				continue;
			}

			auto version = Version{};

			version.m_item_name = json_item[ "name" ].asString();

			if( json_item[ "loc_id" ].isNull() == false )
				version.m_loc_ID = json_item[ "loc_id" ].asUInt();

			if( json_item[ "version" ].isNull() == false )
			{
				version.m_string_version = json_item[ "version" ].asString();
			}
			else
			{
				if( json_item[ "major" ].isNull() )
				{
					FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "Item '%s' (#%d) has no valid version, it will be ignored.", version.m_string_version.c_str(), item_id );
					continue;
				}

				version.m_major = json_item[ "major" ].asUInt();

				if( json_item[ "minor" ].isNull() == false )
					version.m_minor = json_item[ "minor" ].asUInt();
				else
					version.m_minor = 0;

				if( json_item[ "patch" ].isNull() == false )
					version.m_patch = json_item[ "patch" ].asUInt();
				else
					version.m_patch = 0;
			}

			FZN_LOG( "Adding '%s' - %s ...", version.m_item_name.c_str(), version.get_version().c_str() );
			m_versions.push_back( std::move( version ) );
		}
	}

	/**
	* @brief Retrieve the version informations of a given item.
	* @param _item The name of the item.
	* @return A pointer to the struct containing the informations about the item. nullptr if nothing was found.
	**/
	const Version* VersionsManager::get_version( std::string_view _item ) const
	{
		const auto it_version = std::ranges::find( m_versions, _item.data(), &Version::m_item_name );

		if( it_version == m_versions.end() )
			return nullptr;

		return &(*it_version);
	}
} // fzn
