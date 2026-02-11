#include <functional>
#include <fstream>
#include <algorithm>

#include <FZN/Managers/FazonCore.h>
#include <FZN/Tools/Logging.h>
#include <FZN/Tools/Math.h>
#include <FZN/Tools/Tools.h>
#include <FZN/UI/ImGui.h>

#include "TR/TranslatR.h"

namespace TR
{
	static constexpr uint32_t version_major = 0;
	static constexpr uint32_t version_minor = 0;
	static constexpr uint32_t version_feature = 0;
	static constexpr uint32_t version_bugfix = 0;
	static constexpr bool WIP_version = true;

	inline constexpr ImVec4 darker_yellow{ 0.3f, 0.17f, 0.04f, 0.4f };
	inline constexpr ImVec4 darker_red{ 0.25f, 0.05f, 0.05f, 0.4f };

	static constexpr ImVec2 DefaultWidgetSize{ 150.f, 0.f };


	TranslatR::TranslatR()
	{
		g_pFZN_Core->AddCallback( this, &TranslatR::display, fzn::DataCallbackType::Display );

		auto& oIO = ImGui::GetIO();

		oIO.ConfigWindowsMoveFromTitleBarOnly = true;
		oIO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		oIO.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ImGui_fzn::s_ImGuiFormatOptions.m_pFontRegular = oIO.Fonts->AddFontFromFileTTF( DATAPATH( "Display/CascadiaMono.ttf" ), 14.f );
		ImGui_fzn::s_ImGuiFormatOptions.m_pFontBold = oIO.Fonts->AddFontFromFileTTF( DATAPATH( "Display/CascadiaMono-Bold.otf" ), 14.f );
		ImGui_fzn::s_ImGuiFormatOptions.m_pFontItalic = oIO.Fonts->AddFontFromFileTTF( DATAPATH( "Display/CascadiaMono-Italic.otf" ), 14.f );

		if( m_options.get_data().m_load_recent_project )
			m_file_manager.open_most_recent_project( m_loc_data );
	}

	TranslatR::~TranslatR()
	{
		g_pFZN_Core->RemoveCallback( this, &TranslatR::display, fzn::DataCallbackType::Display );
	}

	/**
	* @brief Main ImGui display.
	**/
	void TranslatR::display()
	{
		const sf::Vector2u window_size = g_pFZN_WindowMgr->GetWindowSize();

		ImGui::SetNextWindowPos( { 0.f, 0.f } );
		ImGui::SetNextWindowSize( window_size );
		ImGui::PushStyleVar( ImGuiStyleVar_::ImGuiStyleVar_IndentSpacing, 20.f );
		ImGui::PushStyleVar( ImGuiStyleVar_::ImGuiStyleVar_WindowRounding, 0.f );
		ImGui::PushStyleVar( ImGuiStyleVar_::ImGuiStyleVar_WindowBorderSize, 0.f );
		ImGui::PushStyleVar( ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, ImVec2( 0.0f, 0.0f ) );
		ImGui::PushStyleColor( ImGuiCol_WindowBg, ImVec4( 0.10f, 0.16f, 0.22f, 1.f ) );
		ImGui::PushStyleColor( ImGuiCol_CheckMark, ImVec4( 0.f, 1.f, 0.f, 1.f ) );

		// Overriding those so we don't have any alpha trouble when changing the background of the games section
		ImGui::PushStyleColor( ImGuiCol_FrameBg, ImVec4{ 0.13f, 0.23f, 0.36f, 1.f } );
		ImGui::PushStyleColor( ImGuiCol_FrameBgHovered, ImVec4{ 0.16f, 0.33f, 0.53f, 1.f } );
		ImGui::PushStyleColor( ImGuiCol_FrameBgActive, ImVec4{ 0.21f, 0.45f, 0.73f, 1.f } );
		ImGui::PushStyleColor( ImGuiCol_Button, ImVec4{ 0.16f, 0.33f, 0.53f, 1.f } );
		ImGui::PushStyleColor( ImGuiCol_ButtonHovered, ImVec4{ 0.26f, 0.59f, 0.98f, 1.f } );
		ImGui::PushStyleColor( ImGuiCol_ButtonActive, ImVec4{ 0.06f, 0.53f, 0.98f, 1.f } );

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar;
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		ImGui::Begin( "Splits Manager", nullptr, window_flags );
		ImGui::PopStyleVar( 3 );

		_display_menu_bar();

		_display_entries();

		_add_language_popup();

		m_options.display();

		ImGui::End();

		ImGui::PopStyleVar( 1 );
		ImGui::PopStyleColor( 8 );

		_handle_actions();
	}

	/**
	* @brief Actions tracking and handling.
	**/
	void TranslatR::_handle_actions()
	{
		if( g_pFZN_InputMgr->IsActionPressed( "Open Project" ) )
		{
			m_file_manager.open_project_file( m_loc_data );
		}
		else if( g_pFZN_InputMgr->IsActionPressed( "Save Project" ) )
		{
			m_file_manager.save_project();
		}
		else if( g_pFZN_InputMgr->IsActionPressed( "Save Project As" ) )
		{
			m_file_manager.save_project_as();
		}
		else if( g_pFZN_InputMgr->IsActionPressed( "Close Project" ) )
		{
			m_file_manager.close_project( m_loc_data );
		}
		else if( g_pFZN_InputMgr->IsActionPressed( "Open Entries" ) )
		{
			m_file_manager.open_entries_file( m_loc_data );
		}
		else if( g_pFZN_InputMgr->IsActionPressed( "Save Entries" ) )
		{
			m_file_manager.save_entries( m_loc_data );
		}
		else if( g_pFZN_InputMgr->IsActionPressed( "Save Entries As" ) )
		{
			m_file_manager.save_entries_as( m_loc_data );
		}
		else if( g_pFZN_InputMgr->IsActionPressed( "Close Entries" ) )
		{
			m_file_manager.close_entries( m_loc_data );
		}
		else if( g_pFZN_InputMgr->IsActionPressed( "Generate Enum File" ) )
		{
			m_file_manager.generate_enum_file( m_loc_data );
		}
		else if( g_pFZN_InputMgr->IsActionPressed( "Generate Enum File As" ) )
		{
			m_file_manager.generate_enum_file_as( m_loc_data );
		}
	}

	/**
	* @brief Display the window menu bar.
	**/
	void TranslatR::_display_menu_bar()
	{
		if( ImGui::BeginMainMenuBar() )
		{
			m_file_manager.display_menu_bar_items( m_loc_data );

			if( ImGui::BeginMenu( "Edit" ) )
			{
				if( ImGui::MenuItem( "Add Language(s)" ) )
					_show_new_language_popup();

				ImGui::Separator();
				if( ImGui_fzn::colored_menu_item( ImGui_fzn::color::dark_red, "Clear All Entries", "", false, m_loc_data.has_entries() ) )
					m_loc_data.m_entries.clear();
				ImGui_fzn::simple_tooltip_on_hover( "Clear all entries from the table, keep the available languages" );

				if( ImGui_fzn::colored_menu_item( ImGui_fzn::color::dark_red, "Remove All Languages", "", false, m_loc_data.has_entries() ) )
					_remove_all_languages();
				ImGui_fzn::simple_tooltip_on_hover( "Remove all available languages, keep the entries" );

				if( ImGui_fzn::colored_menu_item( ImGui_fzn::color::dark_red, "Clear All", "", false, m_loc_data.has_entries() ) )
					m_loc_data.clear();
				ImGui_fzn::simple_tooltip_on_hover( "Clear everything, entries and languages" );

				ImGui::Separator();
				if( ImGui::MenuItem( "Options..." ) )
					m_options.open_options();

				ImGui::EndMenu();
			}

			const std::string version{ fzn::Tools::Sprintf( "Ver. %d.%d.%d.%d%s", version_major, version_minor, version_feature, version_bugfix, WIP_version ? " - WIP" : "" ) };
			const ImVec2 version_size{ ImGui::CalcTextSize( version.c_str() ) };
			const sf::Vector2u window_size{ g_pFZN_WindowMgr->GetWindowSize() };

			ImGui::SameLine( window_size.x - ImGui::CalcTextSize( version.c_str() ).x - 2.f * ImGui::GetStyle().WindowPadding.x );
			ImGui::TextColored( ImGui_fzn::color::light_gray, version.c_str() );

			ImGui::EndMainMenuBar();
		}
	}

	static float get_max_entry_width( float _letter_width, const fzn::Localisation::Entries& _entries )
	{
		float max_width{ 0.f };

		for( const fzn::Localisation::Entry& entry : _entries )
		{
			// Approximate text size to be lightweight.
			if( _letter_width * entry.m_name.size() > max_width )
				max_width = _letter_width * entry.m_name.size();
		}

		return max_width;
	}

	/**
	* @brief Display and manage localisation entries.
	**/
	void TranslatR::_display_entries()
	{
		const uint32_t nb_columns{ m_loc_data.m_languages.size() + 2 };		// One column per language and two for entries IDs (number) and name (future enum)
		const float letter_width{ ImGui::CalcTextSize( "W" ).x };
		const float first_column_width{ fzn::Math::get_number_of_digits( m_loc_data.m_entries.size() ) * letter_width };
		uint32_t entry_id{ 0 };
		static int hovered_column{ -1 };
		static int hovered_row{ -1 };
		int language_id_to_erase{ -1 };
		static std::string last_hovered_language{};

		ImGui::PushStyleColor( ImGuiCol_FrameBg, ImGui_fzn::color::transparent );

		ImGui::BeginChild( "LocEntriesChild", ImVec2{ 0.f, ImGui::GetContentRegionAvail().y } );
		if( ImGui::BeginTable( "LocEntries", nb_columns, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY ) )
		{
			ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2{ 0.f, 0.f } );
			ImGui::TableSetupColumn( "#", ImGuiTableColumnFlags_NoHide | ImGuiTableColumnFlags_WidthFixed, first_column_width );
			ImGui::TableSetupColumn( "Entry", ImGuiTableColumnFlags_NoHide | ImGuiTableColumnFlags_WidthFixed, get_max_entry_width( letter_width, m_loc_data.m_entries ) );

			for( const std::string& language : m_loc_data.m_languages )
				ImGui::TableSetupColumn( language.c_str(), ImGuiTableColumnFlags_WidthFixed, 600.f );

			ImGui::TableSetupScrollFreeze( 2, 1 );
			ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
			for( uint32_t column_id = 0; column_id < nb_columns; column_id++ )
			{
				if( !ImGui::TableSetColumnIndex( column_id ) )
					continue;
				ImGui::TableHeader( ImGui::TableGetColumnName( column_id ) );
				
				if( ImGui::IsItemHovered() )
				{
					hovered_column = column_id;
					last_hovered_language = ImGui::TableGetColumnName( column_id );
				}
			}

			// Append to context menu only of hovering a language.
			if( hovered_column >= 2 && ImGui::TableBeginContextMenuPopup( ImGui::GetCurrentTable() ) )
			{
					ImGui::Separator();
					ImGui::PushStyleColor( ImGuiCol_HeaderHovered, ImGui_fzn::color::dark_red );

					if( ImGui::MenuItem( fzn::Tools::Sprintf( "Remove '%s'", last_hovered_language.c_str() ).c_str() ) )
						language_id_to_erase = hovered_column - 2;

					ImGui::PopStyleColor();
				ImGui::EndPopup();
			}

			StringVector missing_translations;
			missing_translations.reserve( m_loc_data.m_languages.size() );
			std::string missing_translations_tooltip;
			ImVec4 missing_translations_text_color{};
			ImVec4 missing_translations_bg_color{};
			float missing_translations_ratio{ 0.f };

			for( fzn::Localisation::Entry& entry : m_loc_data.m_entries )
			{
				missing_translations.clear();
				missing_translations_tooltip.clear();

				ImGui::PushID( &entry );

				ImGui::TableNextColumn();
				ImGui::AlignTextToFramePadding();
				ImGui::Text( "%d", entry_id );

				ImGui::TableNextColumn();
				ImGui::SetNextItemWidth( ImGui::GetContentRegionAvail().x );
				uint32_t nb_mmissing_translations{ 0 };
				uint8_t translation_id{ 0 };

				// First pass on the translations to determine which languages are missing. It has to be done here so we know if we have to change the entry text color or not.
				for( std::string& translation : entry.m_translations )
				{
					if( translation.empty() )
					{
						missing_translations.push_back( m_loc_data.m_languages[ translation_id ] );
						++nb_mmissing_translations;
					}
					++translation_id;
				}

				if( nb_mmissing_translations > 0 )
				{
					missing_translations_ratio = static_cast< float >( nb_mmissing_translations ) / entry.m_translations.size();
					missing_translations_text_color = ImLerp( ImGui_fzn::color::light_yellow, ImGui_fzn::color::light_red, missing_translations_ratio );
					ImGui::PushStyleColor( ImGuiCol_Text, missing_translations_text_color );
					ImGui::PushFont( ImGui_fzn::s_ImGuiFormatOptions.m_pFontBold );
				}

				ImGui::InputTextWithHint( "##EntryName", "<Entry Name>", &entry.m_name );

				if( nb_mmissing_translations > 0 )
				{
					ImGui::PopFont();
					ImGui::PopStyleColor();
				}

				const bool entry_hovered{ ImGui::IsItemHovered() };

				translation_id = 0;
				for( std::string& translation : entry.m_translations )
				{
					ImGui::TableNextColumn();
					ImGui::SetNextItemWidth( ImGui::GetContentRegionAvail().x );

					const bool no_translation{ translation.empty() };

					if( no_translation )
						ImGui::PushFont( ImGui_fzn::s_ImGuiFormatOptions.m_pFontItalic );

					ImGui::InputTextWithHint( fzn::Tools::Sprintf( "##%s_translation_%d", entry.m_name.c_str(), translation_id ).c_str(), "<No Translation>", &translation );

					if( no_translation )
						ImGui::PopFont();
					else
						ImGui_fzn::simple_tooltip_on_hover( translation.c_str() );
					
					++translation_id;
				}

				if( missing_translations.empty() == false )
				{
					if( entry_hovered )
					{
						for( const std::string& language : missing_translations )
						{
							fzn::Tools::sprintf_cat( missing_translations_tooltip, "%s%s", missing_translations_tooltip.empty() ? "" : ", ", language.c_str() );
						}

						ImGui::SetTooltip( "%d missing translations: %s", missing_translations.size(), missing_translations_tooltip.c_str() );
					}
				}

				if( ImGui::TableGetHoveredRow() - 1 == entry_id )
				{
					ImGui::GetCurrentTable()->RowBgColor[ 1 ] = ImGui::GetColorU32( ImGui::GetStyleColorVec4( ImGuiCol_Header ) );
					hovered_row = entry_id;
				}
				else if( missing_translations.empty() == false )
				{
					missing_translations_bg_color = ImLerp( darker_yellow, darker_red, missing_translations_ratio );
					ImGui::GetCurrentTable()->RowBgColor[ 1 ] = ImGui::GetColorU32( missing_translations_bg_color );
				}

				++entry_id;
				ImGui::PopID();
			}

			_display_new_entry( entry_id );

			if( ImGui::TableGetHoveredRow() - 1 == entry_id )
			{
				ImGui::GetCurrentTable()->RowBgColor[ 1 ] = ImGui::GetColorU32( ImGui::GetStyleColorVec4( ImGuiCol_Header ) );
				hovered_row = -1;
			}

			if( hovered_row >= 0 && hovered_row < m_loc_data.m_entries.size() )
			{
				if( ImGui::IsMouseReleased( ImGuiMouseButton_Right ) )
					ImGui::OpenPopup( "entry_right_click" );
				if( ImGui::BeginPopup( "entry_right_click" ) )
				{
					ImGui::PushStyleColor( ImGuiCol_HeaderHovered, ImGui_fzn::color::dark_red );

					if( ImGui::MenuItem( fzn::Tools::Sprintf( "Remove '%s'", m_loc_data.m_entries[ hovered_row ].m_name.c_str() ).c_str() ) )
					{
						m_loc_data.m_entries.erase( m_loc_data.m_entries.begin() + hovered_row );
						hovered_row = -1;
					}

					ImGui::PopStyleColor();
					ImGui::EndPopup();
				}
			}

			ImGui::PopStyleVar();
			ImGui::EndTable();
		}

		ImGui::EndChild();
		ImGui::PopStyleColor();

		if( language_id_to_erase >= 0 )
		{
			_remove_language( language_id_to_erase );
			last_hovered_language.clear();
			hovered_column = -1;
		}
	}

	/**
	* @brief Display the last table line that is used to add an entry to the localisation.
	* @param _row_id The last table row number
	**/
	void TranslatR::_display_new_entry( uint32_t _row_id )
	{
		ImGui::TableNextColumn();
		ImGui::AlignTextToFramePadding();
		ImGui::Text( "%d", _row_id );

		ImGui::TableNextColumn();
		ImGui::SetNextItemWidth( ImGui::GetContentRegionAvail().x );
		ImGui::InputTextWithHint( "##EntryName", "<New Entry>", &m_new_entry );

		if( ImGui::IsItemDeactivatedAfterEdit() && m_new_entry.empty() == false )
		{
			fzn::Localisation::Entry new_entry{ m_new_entry };
			new_entry.m_translations.resize( m_loc_data.m_languages.size(), "" );
			m_loc_data.m_entries.push_back( std::move( new_entry ) );
			m_new_entry.clear();
		}
	}

	/**
	* @brief Enable the new language popup and clear new language strings.
	**/
	void TranslatR::_show_new_language_popup()
	{
		m_show_new_language_popup = true;
		m_new_languages.clear();
		m_new_language.clear();
	}

	/**
	* @brief Display and manage the new language popup.
	**/
	void TranslatR::_add_language_popup()
	{
		if( m_show_new_language_popup == false )
			return;

		if( m_show_new_language_popup )
		{
			ImGui::OpenPopup( "Add New Language(s)" );

			ImVec2 popup_size{};

			popup_size.x = 350.f + ImGui::GetStyle().WindowPadding.x * 2.f;
			popup_size.y = ImGui::GetStyle().WindowPadding.y * 2.f + ImGui::GetFrameHeightWithSpacing() * 2.f + ImGui::GetTextLineHeightWithSpacing() * 2.f + ImGui::GetStyle().ItemSpacing.y;
			popup_size.y += m_new_languages.size() * ImGui::GetFrameHeightWithSpacing();
			sf::Vector2u window_size = g_pFZN_WindowMgr->GetWindowSize();

			ImGui::SetNextWindowPos( { window_size.x * 0.5f - popup_size.x * 0.5f, window_size.y * 0.5f - popup_size.y * 0.5f }, ImGuiCond_FirstUseEver );
			ImGui::SetNextWindowSize( popup_size );

			if( ImGui::BeginPopupModal( "Add New Language(s)", &m_show_new_language_popup, ImGuiWindowFlags_NoResize ) )
			{
				bool one_item_edited{ false };
				for( std::string& language : m_new_languages )
				{
					ImGui::PushID( &language );
					ImGui::SetNextItemWidth( ImGui::GetContentRegionAvail().x );
					ImGui::InputTextWithHint( "##language_name", "<New Language Name>", &language );
					one_item_edited |= ImGui::IsItemDeactivatedAfterEdit();
					ImGui::PopID();
				}

				if( one_item_edited )
					std::erase_if( m_new_languages, []( const std::string& _language ) { return _language.empty(); } );

				ImGui::SetNextItemWidth( ImGui::GetContentRegionAvail().x );
				ImGui::InputTextWithHint( "##language_name", "<New Language Name>", &m_new_language );
				
				if( ImGui::IsItemDeactivatedAfterEdit() && m_new_language.empty() == false )
				{
					m_new_languages.push_back( m_new_language );
					m_new_language.clear();
				}

				ImGui::NewLine();

				if( ImGui::BeginTable( "BottomTable", 3 ) )
				{
					ImGui::TableSetupColumn( "Empty", ImGuiTableColumnFlags_WidthStretch );

					for( uint8_t column{ 1 }; column < 3; ++column )
						ImGui::TableSetupColumn( fzn::Tools::Sprintf( "Button %u", column ).c_str(), ImGuiTableColumnFlags_WidthFixed );

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex( 1 );

					if( ImGui_fzn::deactivable_button( "Apply", m_new_languages.empty(), true, DefaultWidgetSize ) )
					{
						_add_new_languages();
						m_new_languages.clear();
						m_show_new_language_popup = false;
					}

					ImGui::TableSetColumnIndex( 2 );
					if( ImGui::Button( "Cancel", DefaultWidgetSize ) )
					{
						m_new_languages.clear();
						m_show_new_language_popup = false;
					}

					ImGui::EndTable();
				}

				ImGui::EndPopup();
			}
		}
	}

	/**
	* @brief Validate the new languages entered in the popup and add them to the projects available languages.
	**/
	void TranslatR::_add_new_languages()
	{
		std::erase_if( m_new_languages, [&]( const std::string& _language )
			{
				if( std::ranges::find( m_loc_data.m_languages, _language ) != m_loc_data.m_languages.end() )
					return true;

				return _language.empty();
			} );

		for( const std::string& language : m_new_languages )
		{
			m_loc_data.m_languages.push_back( language );
		}

		for( fzn::Localisation::Entry& entry : m_loc_data.m_entries )
		{
			entry.m_translations.resize( m_loc_data.m_languages.size() );
		}
	}

	/**
	* @brief Remove the given language from the projects available ones and from all the entries.
	* @param _language_id The ID of the language in the languages vector.
	**/
	void TranslatR::_remove_language( uint32_t _language_id )
	{
		if( _language_id >= m_loc_data.m_languages.size() )
			return;

		const std::string language_to_remove = m_loc_data.m_languages[ _language_id ];

		FZN_DBLOG( "Removing '%s'...", language_to_remove.c_str() );

		for( fzn::Localisation::Entry& entry : m_loc_data.m_entries )
			entry.m_translations.erase( entry.m_translations.begin() + _language_id );

		m_loc_data.m_languages.erase( m_loc_data.m_languages.begin() + _language_id );

		FZN_DBLOG( "'%s' removed successfully.", language_to_remove.c_str() );
	}

	/**
	* @brief Remove all the languages from the project and clear them from all the entries.
	**/
	void TranslatR::_remove_all_languages()
	{
		if( m_loc_data.m_languages.empty() )
			return;

		for( fzn::Localisation::Entry& entry : m_loc_data.m_entries )
			entry.m_translations.clear();

		m_loc_data.m_languages.clear();
	}

} // namespace TR
