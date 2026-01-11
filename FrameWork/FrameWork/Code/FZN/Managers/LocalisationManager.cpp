#include "LocalisationManager.h"


FZN_EXPORT fzn::Localisation::Manager* g_pFZN_LocMgr = nullptr;

namespace fzn
{
	namespace Localisation
	{

		Manager::Manager()
		{
			g_pFZN_LocMgr = this;
		}

		Manager::~Manager()
		{
			m_entries.clear();
		}

		/**
		* @brief Load translation entries from localisation json.
		* @param _path The path to the entries definition file.
		**/
		void Manager::load_entries( std::string_view _path )
		{
			auto file = std::ifstream{ _path.data() };

			if( file.is_open() == false )
			{
				FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "Couldn't open file: %s", _path.data() );
				return;
			}
			else
				FZN_LOG( "Loading entries at %s...", _path.data() );

			auto root = Json::Value{};
			file >> root;

			Json::Value available_languages = root[ "available_languages" ];
			Json::Value::iterator it_language = available_languages.begin();

			for( Json::Value::iterator it_language = available_languages.begin(); it_language != available_languages.end(); ++it_language )
			{
				if( get_language_id( it_language->asString() ) < m_languages.size() )
				{
					FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "Language '%s' already added, possible duplicate.", it_language->asCString() );
					continue;
				}

				m_languages.push_back( it_language->asString() );
			}

			Json::Value entries = root[ "entries" ];
			Json::Value::iterator it_entry = entries.begin();

			m_entries.reserve( entries.size() );

			for( Json::Value::iterator it_entry = entries.begin(); it_entry != entries.end(); ++it_entry )
				_load_entry( *it_entry );
		}

		uint32_t Manager::get_language_id( std::string_view _language ) const
		{
			if( auto it_language = std::ranges::find( m_languages, _language ); it_language != m_languages.end() )
			{
				return std::distance( m_languages.begin(), it_language );
			}

			return Uint32_Max;
		}

		std::string_view Manager::get_language_name( uint32_t _language_id ) const
		{
			if( _language_id >= m_languages.size() )
				return {};

			return m_languages[ _language_id ];
		}

		void Manager::_load_entry( const Json::Value& _entry )
		{
			auto entry = Entry{};

			entry.m_name = _entry[ "name" ].asString();
			entry.m_translations.resize( m_languages.size(), "" );

			Json::Value translations = _entry[ "translations" ];
			Json::Value::iterator it_translation = translations.begin();

			for( Json::Value::iterator it_translation = translations.begin(); it_translation != translations.end(); ++it_translation )
			{
				if( it_translation->size() != 2 )
				{
					FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "Invalid number of values in translation %d of entry '%s'.", std::distance( translations.begin(), it_translation ), entry.m_name.c_str() );
					continue;
				}

				const Json::Value& translation{ *it_translation };
				const uint32_t language_id{ get_language_id( translation[ 0 ].asString() ) };

				if( language_id >= entry.m_translations.size() )
				{
					FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "Invalid language for entry '%s' (%s).", entry.m_name.c_str(), translation[ 0 ].asCString() );
					continue;
				}

				entry.m_translations[ language_id ] = translation[ 1 ].asString();
			}

			m_entries.push_back( std::move( entry ) );
		}

	} // namespace Localisation
} // namespace fzn