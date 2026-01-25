#pragma once

#include <vector>
#include <string>

#include <SFML/System/NonCopyable.hpp>
#include <Externals/json/json.h>

#include "FZN/Defines.h"
#include "FZN/Tools/Logging.h"


namespace fzn
{
	namespace Localisation
	{
		/************************************************************************
		* @brief All the informations about a word or sentence entered in the localisation file. (ID, name, translations)
		************************************************************************/
		struct Entry
		{
			std::string m_name;				// The enum name that will be used in the project using localisation.
			StringVector m_translations;	// All the corresponding translations for this entry. Will always be a size corresponding to the total number of languages in the project.
		};
		using Entries = std::vector< Entry >;

		/************************************************************************
		* @brief The manager that will handle translation querries in the project. Takes an enum value, and a language ID, returns the translation.
		************************************************************************/
		class FZN_EXPORT Manager : public sf::NonCopyable
		{
		public:
			Manager();
			~Manager();

			/**
			* @brief Load translation entries from localisation json.
			* @param _path The path to the entries definition file.
			**/
			void load_entries( std::string_view _path );
			/**
			* @brief Load translation entries from localisation json.
			* @param _path The path to the entries definition file.
			* @param [out] _entries An entries vector to fill with the entries contained in the given file.
			* @param [out] _languages A vector to fill with all the available languages.
			**/
			static void load_entries( std::string_view _path, Entries& _entries, StringVector& _languages );

			/**
			* @brief Retrieve an entry translation from its ID. The two params will be casted in uint32_t in the function, allowing to use project local types to call the function.
			* @param _entry uint32_t castable entry ID.
			* @param _language uint32_t castable language ID.
			**/
			template< typename EntryType, typename LanguageType >
			std::string_view get_string( EntryType _entry, LanguageType _language )
			{
				auto entry_id = static_cast< uint32_t >( _entry );
				auto language_id = static_cast< uint32_t >( _language );

				if( entry_id >= m_entries.size() )
				{
					FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "Entry #%d not found. (nb entries: %d)", entry_id, m_entries.size() );
					return {};
				}

				const Entry& entry{ m_entries[ entry_id ] };

				if( language_id >= entry.m_translations.size() )
				{
					FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "Language #%d for entry %s (#%d) not found. (max nb languages: %d, nb translations for entry: %d)", language_id, entry.m_name.c_str(), entry_id, m_languages.size(), entry.m_translations.size() );
					return {};
				}

				// If there is no translation in the requested language, return the entry name to make it obvious so it can be fixed.
				if( entry.m_translations[ language_id ].empty() )
				{
					FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "No translation in language #%d found for entry '%s' (#%d)", language_id, entry.m_name.c_str(), entry_id );
					return entry.m_name;
				}

				return entry.m_translations[ language_id ];
			}
			/**
			* @brief Retrieve an entry translation from its ID. The two params will be casted in uint32_t in the function, allowing to use project local types to call the function.
			* @param _entry uint32_t castable entry ID.
			* @param _language uint32_t castable language ID.
			**/
			template< typename EntryType >
			std::string_view get_string( EntryType _entry )
			{
				return get_string( _entry, m_current_language );
			}

			/**
			* @brief Return the currently chosen laguage ID.
			* @return Current language ID without casting it.
			**/
			uint32_t get_current_language_id() const { return m_current_language; }
			/**
			* @brief Return the currently chosen language casted in project local type.
			* @return Casted language ID.
			**/
			template< typename LanguageType >
			LanguageType get_current_language() const { return static_cast<LanguageType>( m_current_language ); }

			uint32_t get_language_id( std::string_view _language ) const;
			static uint32_t get_language_id( std::string_view _language, const StringVector& _languages );
			std::string_view get_language_name( uint32_t _language_id ) const;
			static std::string_view get_language_name( uint32_t _language_id, const StringVector& _languages );

		private:
			static void _load_entry( const Json::Value& _entry, Entries& _entries, const StringVector& _languages );

			Entries			m_entries;					// All the translated entries.
			StringVector	m_languages;				// All the languages available.

			uint32_t	m_current_language{ 0 };	// Currently selected language for the software.
		};
	} // namespace Localisation
} // namespace fzn

extern FZN_EXPORT fzn::Localisation::Manager* g_pFZN_LocMgr;
