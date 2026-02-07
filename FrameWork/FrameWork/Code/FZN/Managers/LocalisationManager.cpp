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
			m_loc_data.clear();
		}

		/**
		* @brief Load translation entries from localisation json.
		* @param _path The path to the entries definition file.
		**/
		void Manager::load_entries( std::string_view _path )
		{
			load_entries( _path, m_loc_data );
		}

		/**
		* @brief Load translation entries from localisation json.
		* @param _path						The path to the entries definition file.
		* @param [out] _localisation_data	The localisation data to be filled with the entries contained in the given file and all the available languages.
		**/
		void Manager::load_entries( std::string_view _path, LocalisationData& _localisation_data )
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

			if( root[ "entries" ].isNull() )
			{
				FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "Invalid localisation file: %s", _path.data() );
				return;
			}

			_localisation_data.m_entries.clear();
			_localisation_data.m_languages.clear();

			Json::Value available_languages = root[ "available_languages" ];
			Json::Value::iterator it_language = available_languages.begin();

			for( Json::Value::iterator it_language = available_languages.begin(); it_language != available_languages.end(); ++it_language )
			{
				if( get_language_id( it_language->asString(), _localisation_data.m_languages ) < _localisation_data.m_languages.size() )
				{
					FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "Language '%s' already added, possible duplicate.", it_language->asCString() );
					continue;
				}

				_localisation_data.m_languages.push_back( it_language->asString() );
			}

			Json::Value entries = root[ "entries" ];
			Json::Value::iterator it_entry = entries.begin();

			_localisation_data.m_entries.reserve( entries.size() );

			for( Json::Value::iterator it_entry = entries.begin(); it_entry != entries.end(); ++it_entry )
				_load_entry( *it_entry, _localisation_data );
		}

		uint32_t Manager::get_language_id( std::string_view _language ) const
		{
			return get_language_id( _language, m_loc_data.m_languages );
		}

		uint32_t Manager::get_language_id( std::string_view _language, const StringVector& _languages )
		{
			if( auto it_language = std::ranges::find( _languages, _language ); it_language != _languages.end() )
			{
				return std::distance( _languages.begin(), it_language );
			}

			return Uint32_Max;
		}

		std::string_view Manager::get_language_name( uint32_t _language_id ) const
		{
			return get_language_name( _language_id, m_loc_data.m_languages );
		}

		std::string_view Manager::get_language_name( uint32_t _language_id, const StringVector& _languages )
		{
			if( _language_id >= _languages.size() )
				return {};

			return _languages[ _language_id ];
		}

		/**
		* @brief Retrieve the data of a single localisation entry.
		* @param _entry						The json value the data come from.
		* @param [out] _localisation_data	The localisation data to be filled.
		**/
		void Manager::_load_entry( const Json::Value& _entry, LocalisationData& _localisation_data )
		{
			auto entry = Entry{};

			entry.m_name = _entry[ "name" ].asString();
			entry.m_translations.resize( _localisation_data.m_languages.size(), "" );

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
				const uint32_t language_id{ get_language_id( translation[ 0 ].asString(), _localisation_data.m_languages ) };

				if( language_id >= entry.m_translations.size() )
				{
					FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "Invalid language for entry '%s' (%s).", entry.m_name.c_str(), translation[ 0 ].asCString() );
					continue;
				}

				entry.m_translations[ language_id ] = translation[ 1 ].asString();
			}

			_localisation_data.m_entries.push_back( std::move( entry ) );
		}

	} // namespace Localisation
} // namespace fzn