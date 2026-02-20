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
		enum class Case
		{
			all_lower,						// Turn all characters to lower case.
			all_upper,						// Turn all characters to upper case.
			first_letter_upper,				// Turn the first letter of the string to upper case.
			first_letter_upper_all_words,	// Turn the first letter of each word in the string to upper case.
			COUNT
		};

		/************************************************************************
		* @brief All the informations about a word or sentence entered in the localisation file. (ID, name, translations)
		************************************************************************/
		struct Entry
		{
			std::string		m_name;				// The enum name that will be used in the project using localisation.
			StringVector	m_translations;		// All the corresponding translations for this entry. Will always be a size corresponding to the total number of languages in the project.
		};
		using Entries = std::vector< Entry >;

		/************************************************************************
		* @brief A struct keeping together the two elements of a localisation file, the entries and the available languages.
		************************************************************************/
		struct LocalisationData
		{
			/**
			* @brief Clear all data (entries and languages).
			**/
			void clear()				{ m_entries.clear(); m_languages.clear(); }
			/**
			* @brief Check if there are entries in the data.
			* @return True if there are entries, false if empty.
			**/
			bool has_entries() const	{ return m_entries.size() > 0; }

			Entries			m_entries;		// All the translated entries.
			StringVector	m_languages;	// All the languages available.
		};

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
			* @param _path						The path to the entries definition file.
			* @param [out] _localisation_data	The localisation data to be filled with the entries contained in the given file and all the available languages.
			**/
			static void load_entries( std::string_view _path, LocalisationData& _localisation_data );

			/**
			* @brief Retrieve an entry translation from its ID. The two params will be casted in uint32_t in the function, allowing to use project local types to call the function.
			* @param _entry uint32_t castable entry ID.
			* @param _language uint32_t castable language ID.
			* @param _case_transform How to transform the case of the string. COUNT means to transformation is done.
			* @return The string corresponding to the given ID and language. Empty if not found or couldn't be retrieved.
			**/
			template< typename EntryType, typename LanguageType >
			std::string_view get_string( EntryType _entry, LanguageType _language, Case _case_transform = Case::COUNT )
			{
				auto entry_id = static_cast< uint32_t >( _entry );
				auto language_id = static_cast< uint32_t >( _language );

				if( entry_id >= m_loc_data.m_entries.size() )
				{
					FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "Entry #%d not found. (nb entries: %d)", entry_id, m_loc_data.m_entries.size() );
					return {};
				}

				const Entry& entry{ m_loc_data.m_entries[ entry_id ] };

				if( language_id >= entry.m_translations.size() )
				{
					FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "Language #%d for entry %s (#%d) not found. (max nb languages: %d, nb translations for entry: %d)", language_id, entry.m_name.c_str(), entry_id, m_loc_data.m_languages.size(), entry.m_translations.size() );
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
			* @param _case_transform How to transform the case of the string. COUNT means to transformation is done.
			* @return The string corresponding to the given ID and language. Empty if not found or couldn't be retrieved.
			**/
			template< typename EntryType >
			std::string_view get_string( EntryType _entry, Case _case_transform = Case::COUNT )
			{
				return get_string( _entry, m_current_language, _case_transform );
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
			template< typename LanguageType >
			void set_current_language( LanguageType _language )
			{
				auto language_id = static_cast< uint32_t >( _language );

				if( language_id >= m_loc_data.m_languages.size() )
					return;

				m_current_language = language_id;
			}

			uint32_t get_language_id( std::string_view _language ) const;
			static uint32_t get_language_id( std::string_view _language, const StringVector& _languages );
			std::string_view get_language_name( uint32_t _language_id ) const;
			static std::string_view get_language_name( uint32_t _language_id, const StringVector& _languages );

		private:
			/**
			* @brief Retrieve the data of a single localisation entry.
			* @param _entry						The json value the data come from.
			* @param [out] _localisation_data	The localisation data to be filled.
			**/
			static void _load_entry( const Json::Value& _entry, LocalisationData& _localisation_data );

			LocalisationData m_loc_data;

			uint32_t	m_current_language{ 0 };	// Currently selected language for the software.
		};
	} // namespace Localisation
} // namespace fzn

extern FZN_EXPORT fzn::Localisation::Manager* g_pFZN_LocMgr;
