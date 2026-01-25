#include <functional>

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

	TranslatR::TranslatR()
	{
		g_pFZN_Core->AddCallback( this, &TranslatR::display, fzn::DataCallbackType::Display );

		auto& oIO = ImGui::GetIO();

		oIO.ConfigWindowsMoveFromTitleBarOnly = true;
		oIO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		oIO.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ImGui_fzn::s_ImGuiFormatOptions.m_pFontRegular = oIO.Fonts->AddFontFromFileTTF( DATAPATH( "Display/CascadiaMono.ttf" ), 14.f );
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

		if( m_entries.empty() == false )
		{
			_display_languages();
			_display_entries();
		}

		ImGui::End();

		ImGui::PopStyleVar( 1 );
		ImGui::PopStyleColor( 8 );
	}

	/**
	* @brief Display the window menu bar.
	**/
	void TranslatR::_display_menu_bar()
	{
		auto menu_item = [ & ]( const char* _label, bool _disable, std::function< void( void ) > _fct )
			{
				if( _disable )
					ImGui::BeginDisabled();

				if( ImGui::MenuItem( _label ) )
					_fct();

				if( _disable )
					ImGui::EndDisabled();
			};

		if( ImGui::BeginMainMenuBar() )
		{
			if( ImGui::BeginMenu( "File" ) )
			{
				const bool entries_path_invalid = m_entries_path.empty();
				//const bool no_games = m_splits_mgr.are_there_games() == false;
				if( ImGui::BeginMenu( "Entries List" ) )
				{
					menu_item( "Create...", false, [ & ]() { _create_json(); } );
					ImGui_fzn::simple_tooltip_on_hover( "Close current file and create a new one." );

					menu_item( "Load...", false, [ & ]() { _load_json(); } );
					menu_item( "Save", entries_path_invalid, [ & ]() { _save_json(); } );
					ImGui_fzn::simple_tooltip_on_hover( fzn::Tools::Sprintf( "Loaded file path: %s", "<example path>" ) );

					menu_item( "Save As...", entries_path_invalid, [ & ]() { _save_json_as(); } );

					ImGui::EndMenu();
				}
				if( ImGui::BeginMenu( "Enum File" ) )
				{
					ImGui::EndMenu();
				}
				if( ImGui::BeginMenu( "TranslatR Project" ) )
				{
					menu_item( "Close", false, [ & ]() {} );
					ImGui::EndMenu();
				}

				ImGui::Separator();
				menu_item( "Options...", false, [ & ]() {} );

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

	/**
	* @brief Display and manage available languages.
	**/
	void TranslatR::_display_languages()
	{

	}

	/**
	* @brief Display and manage localisation entries.
	**/
	void TranslatR::_display_entries()
	{
		const uint32_t nb_columns{ m_languages.size() + 2 };		// One column per language and two for entries IDs (number) and name (future enum)
		uint32_t entry_id{ 0 };

		ImGui::Button( "Add Language" );
		ImGui::PushStyleColor( ImGuiCol_FrameBg, ImGui_fzn::color::transparent );

		ImGui::BeginChild( "LocEntriesChild", ImVec2{ 0.f, ImGui::GetContentRegionAvail().y } );
		if( ImGui::BeginTable( "LocEntries", nb_columns, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_RowBg ) )
		{
			ImGui::TableSetupColumn( "#" );
			ImGui::TableSetupColumn( "Entry" );
			
			for( const std::string& language : m_languages )
				ImGui::TableSetupColumn( language.c_str() );

			ImGui::TableSetupScrollFreeze( 0, 1 );
			ImGui::TableHeadersRow();

			for( fzn::Localisation::Entry& entry : m_entries )
			{
				ImGui::PushID( &entry );

				ImGui::TableNextColumn();
				ImGui::Text( "%d", entry_id );

				ImGui::TableNextColumn();
				ImGui::SetNextItemWidth( ImGui::GetContentRegionAvail().x );
				ImGui::InputTextWithHint("##EntryName", "<Entry Name>", &entry.m_name );

				uint8_t translation_id{ 0 };
				for( std::string& translation : entry.m_translations )
				{
					ImGui::TableNextColumn();
					ImGui::SetNextItemWidth( ImGui::GetContentRegionAvail().x );
					ImGui::InputTextWithHint( fzn::Tools::Sprintf( "##%s_translation_%d", entry.m_name.c_str(), translation_id ).c_str(), "<No Translation>", &translation );

					if( translation.empty() == false )
						ImGui_fzn::simple_tooltip_on_hover( translation );
					++translation_id;
				}

				++entry_id;
				ImGui::PopID();
			}

			ImGui::EndTable();
		}

		ImGui::EndChild();
		ImGui::PopStyleColor();
	}

	void TranslatR::_create_json()
	{

	}

	void TranslatR::_save_json()
	{

	}

	void TranslatR::_save_json_as()
	{

	}

	void TranslatR::_load_json()
	{
		m_entries_path = fzn::Tools::open_file(	"(*.json) Localisation Entries List\0*.json\0"
														"(*.*) All files \0*.*\0" );

		fzn::Localisation::Manager::load_entries( m_entries_path, m_entries, m_languages );
	}

} // namespace TR