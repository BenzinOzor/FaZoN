#include <fstream>
#include <filesystem>

#include <FZN/Managers/FazonCore.h>

#include "FZN/Application/AppOptions.h"


namespace fzn
{

	AppOptions::AppOptions():
		m_option_window_size( 400.f, 0.f )
	{
	}

	/**
	* @brief Prepare the option window to be displayed. Setup its first state.
	**/
	void AppOptions::open_options()
	{
		m_show_window = true;
		m_edited = false;

		g_pFZN_InputMgr->BackupActionKeys();
	}

	/**
	* @brief Main display fonction.
	**/
	void AppOptions::display()
	{
		if( m_show_window == false )
			return;

		ImGui::SetNextWindowSize( m_option_window_size );

		if( ImGui::Begin( "Options", nullptr, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse ) )
		{
			m_first_column_width = ImGui::GetContentRegionAvail().x * m_table_first_column_ratio;

			// Calling the custom display to let the user set the window how they like.
			_display_custom_options();
		}

		ImGui::End();
	}

	/**
	* @brief Event management function.
	**/
	void AppOptions::on_event()
	{
		sf::Event sf_event = g_pFZN_WindowMgr->GetWindowEvent();

		switch( sf_event.type )
		{
			case sf::Event::Closed:
			{
				_save_options();
				return;
			};
			case sf::Event::Resized:
			{
				m_app_window_size = g_pFZN_WindowMgr->GetWindowSize();
			};
		};

		fzn::Event fzn_event = g_pFZN_Core->GetEvent();

		if( fzn_event.m_eType == fzn::Event::eActionKeyBindDone )
			m_edited = true;

		// Calling user custom event if there are any.
		_on_custom_event( sf_event, fzn_event );
	}

	/**
	* @brief Binding display function.
	**/
	void AppOptions::_display_bindings()
	{
		const bool popup_open{ g_pFZN_InputMgr->IsWaitingActionKeyBind() };
		std::string_view replaced_binding_name{};
		static std::string popup_name{};
		ImGui::SeparatorText( "Keybinds" );

		if( ImGui::BeginTable( "Keybinds", 3 ) )
		{
			ImGui::TableSetupColumn( "##Action", ImGuiTableColumnFlags_WidthFixed, m_first_column_width );
			ImGui::TableSetupColumn( "##Bind", ImGuiTableColumnFlags_WidthStretch );
			ImGui::TableSetupColumn( "##Del.", ImGuiTableColumnFlags_WidthFixed, ImGui::CalcTextSize( "Del." ).x + ImGui::GetStyle().FramePadding.x + ImGui::GetStyle().ItemSpacing.x );

			std::string action_binding{};
			bool no_binding{ false };

			for( const fzn::ActionKey& action_key : g_pFZN_InputMgr->GetActionKeys() )
			{
				ImGui::PushID( &action_key );
				ImGui::TableNextRow();
				_first_column_label( action_key.m_sName.c_str() );
				ImGui::TableNextColumn();

				action_binding = g_pFZN_InputMgr->GetActionKeyString( action_key.m_sName, true, 0, false );

				if( action_binding.empty() )
				{
					no_binding = true;
					action_binding = "<Empty>";
					ImGui::PushStyleColor( ImGuiCol_Text, ImGui_fzn::color::dark_gray );
				}
				else
					no_binding = false;

				if( ImGui::Button( action_binding.c_str(), { ImGui::GetContentRegionAvail().x, 0.f } ) )
				{
					g_pFZN_InputMgr->replace_action_key_bind( action_key.m_sName, fzn::InputManager::BindTypeFlag_All, 0 );
					replaced_binding_name = action_key.m_sName;
				}

				if( no_binding )
				{
					ImGui::PopStyleColor();
				}

				if( ImGui::IsItemHovered() )
					ImGui::SetTooltip( no_binding ? "Set" : "Replace" );

				ImGui::TableNextColumn();
				if( ImGui_fzn::deactivable_button( "Del.", no_binding, false, { ImGui::GetContentRegionAvail().x, 0.f } ) )
				{
					if( g_pFZN_InputMgr->RemoveActionKeyBind( action_key.m_sName, fzn::InputManager::BindType::eKey ) )
						m_edited = true;
				}

				if( ImGui::IsItemHovered() )
					ImGui::SetTooltip( "Delete shortcut" );

				ImGui::PopID();
			}

			ImGui::EndTable();
		}

		if( _begin_option_table() )
		{
			ImGui::PushStyleColor( ImGuiCol_Button, ImGui_fzn::color::dark_red );
			ImGui::PushStyleColor( ImGuiCol_ButtonActive, ImGui_fzn::color::light_red );
			ImGui::PushStyleColor( ImGuiCol_ButtonHovered, ImGui_fzn::color::red );

			_second_column_widget( [ & ]() -> bool
				{
					if( ImGui::Button( "Reset To Default", { ImGui::GetContentRegionAvail().x, 0.f } ) )
					{
						g_pFZN_InputMgr->restore_default_action_keys();
						return true;
					}

					return false;
				});

			ImGui::EndTable();

			ImGui::PopStyleColor( 3 );
		}

		if( popup_open != g_pFZN_InputMgr->IsWaitingActionKeyBind() )
		{
			popup_name = fzn::Tools::Sprintf( "Replace binding: %s", replaced_binding_name.data() );
			ImGui::OpenPopup( popup_name.c_str() );
		}

		if( g_pFZN_InputMgr->IsWaitingActionKeyBind() )
		{
			const ImVec2 title_size{ ImGui::CalcTextSize( popup_name.c_str() ) };
			static const ImVec2 text_size{ ImGui::CalcTextSize( "Press any key to replace this binding" ) };

			const float popup_width{ std::max( text_size.x, title_size.x ) + ImGui::GetStyle().WindowPadding.x * 2.f };
			sf::Vector2u window_size = g_pFZN_WindowMgr->GetWindowSize();

			ImGui::SetNextWindowPos( { window_size.x * 0.5f - popup_width * 0.5f, window_size.y * 0.5f - popup_width * 0.5f }, ImGuiCond_Appearing );
			ImGui::SetNextWindowSize( { popup_width, ImGui::GetFrameHeightWithSpacing() * 4.f + ImGui::GetTextLineHeightWithSpacing() } );

			if( ImGui::BeginPopupModal( popup_name.c_str(), nullptr, ImGuiWindowFlags_NoMove ) )
			{
				ImGui::NewLine();
				ImGui::Text( "Press any key to replace this binding" );

				ImGui::NewLine();
				ImGui::NewLine();
				ImGui::SameLine( ImGui::GetContentRegionAvail().x * 0.5f - ImGui_fzn::default_widget_size.x * 0.5f );

				if( ImGui::Button( "Cancel", ImGui_fzn::default_widget_size ) )
				{
					g_pFZN_InputMgr->cancel_action_key_rebind();
				}

				ImGui::EndPopup();
			}
		}
	}

	/**
	* @brief Validate edited options and close the window.
	**/
	void AppOptions::_confirm_options()
	{
		m_show_window = false;
		_save_options();
	}

	/**
	* @brief Cancel edited options and restore old ones, then close window.
	**/
	void AppOptions::_cancel_options()
	{
		m_show_window = false;
		g_pFZN_InputMgr->RestoreBackupActionKeys();
	}

	/**
	* @brief Load options data from default options.json in the save folder path.
	**/
	void AppOptions::_load_options()
	{
		const std::string option_file_path{ g_pFZN_Core->GetSaveFolderPath() + "/options.json" };
		auto file = std::ifstream{ option_file_path };

		if( file.is_open() == false )
		{
			// If the reason we could not open the option file is that it doesn't exist, create it.
			if( std::filesystem::exists( option_file_path ) == false )
			{
				m_app_window_size = g_pFZN_WindowMgr->GetWindowSize();
				_save_options();
			}

			return;
		}

		auto root = Json::Value{};

		file >> root;

		m_app_window_size.x = std::max( root[ "window_size" ][ 0 ].asUInt(), 800u );
		m_app_window_size.y = std::max( root[ "window_size" ][ 1 ].asUInt(), 600u );

		g_pFZN_WindowMgr->SetWindowSize( m_app_window_size );

		RECT desktop_size;
		const HWND desktop_handle = GetDesktopWindow();
		GetWindowRect( desktop_handle, &desktop_size );

		g_pFZN_WindowMgr->SetWindowPosition( { desktop_size.right / 2 - static_cast< int >( m_app_window_size.x ) / 2, desktop_size.bottom / 2 - static_cast< int >( m_app_window_size.y ) / 2 } );

		// Calling custom option data loading.
		_load_options_from_json( root );
	}

	/**
	* @brief Save options data to default options.json in the save folder path.
	**/
	void AppOptions::_save_options()
	{
		auto file = std::ofstream{ g_pFZN_Core->GetSaveFolderPath() + "/options.json" };
		auto root = Json::Value{};
		Json::StyledWriter json_writer;

		root[ "window_size" ][ 0 ] = m_app_window_size.x;
		root[ "window_size" ][ 1 ] = m_app_window_size.y;

		// Calling custom option data saving.
		_save_options_to_json( root );

		file << json_writer.write( root );

		g_pFZN_InputMgr->SaveCustomActionKeysToFile();
	}

	/**
	* @brief Creation of the table containing the options fields.
	* The size of the first column is a ratio of the option window size, both setable via member variables.
	* @return True if the table is valid.
	**/
	bool AppOptions::_begin_option_table() const
	{
		const bool ret = ImGui::BeginTable( "options_table", 2 );

		ImGui::TableSetupColumn( "label", ImGuiTableColumnFlags_WidthFixed, m_first_column_width );
		ImGui::TableNextRow();

		return ret;
	}

	/**
	* @brief Display a label in the first column, with or without a helper tooltip.
	* @param _label The label to display.
	* @param _tooltip The tooltip to display when hovering the helper. If no tooltip is given, the helper won't be displayed.
	**/
	void AppOptions::_first_column_label( std::string_view _label, std::string_view _tooltip /*= {} */ )
	{
		_first_column_colored_label( ImGui::GetStyleColorVec4( ImGuiCol_Text ), _label, _tooltip );
	}

	/**
	* @brief Display a colored label in the first column, with or without a helper tooltip.
	* @param _color The color for the label.
	* @param _label The label to display.
	* @param _tooltip The tooltip to display when hovering the helper. If no tooltip is given, the helper won't be displayed.
	**/
	void AppOptions::_first_column_colored_label( const ImVec4& _color, std::string_view _label, std::string_view _tooltip /*= {} */ )
	{
		ImGui::TableSetColumnIndex( 0 );
		ImGui::SameLine( ImGui::GetStyle().IndentSpacing * 0.5f );
		ImGui::AlignTextToFramePadding();
		ImGui::TextColored( _color, _label.data() );

		if( _tooltip.empty() == false )
		{
			ImGui::SameLine();
			ImGui_fzn::helper_simple_tooltip( _tooltip.data() );
		}
	}

	/**
	* @brief Display a widget in the first column, with or without a helper tooltip.
	* @param _widget_fct The widget to display.
	* @param _tooltip The tooltip to display when hovering the helper. If no tooltip is given, the helper won't be displayed.
	**/
	void AppOptions::_first_column_widget( std::function<bool( void )> _widget_fct, std::string_view _tooltip /*= {} */ )
	{
		ImGui::TableSetColumnIndex( 0 );
		ImGui::SameLine( ImGui::GetStyle().IndentSpacing * 0.5f );
		
		_widget_fct();

		if( _tooltip.empty() == false )
		{
			ImGui::SameLine();
			ImGui_fzn::helper_simple_tooltip( _tooltip.data() );
		}
	}

	/**
	* @brief Display a widget in the second column, with or without a helper tooltip.
	* @param _widget_fct The widget to display.
	* @param _width The custom width of the widget. 0 will use the default size set by ImGui.
	* @param _tooltip The tooltip to display when hovering the helper. If no tooltip is given, the helper won't be displayed.
	**/
	void AppOptions::_second_column_widget( std::function<bool( void )> _widget_fct, float _width /*= 0.f*/, std::string_view _tooltip /*= {} */ )
	{
		ImGui::TableSetColumnIndex( 1 );

		if( _width > 0.f )
			ImGui::SetNextItemWidth( _width );

		m_edited |= _widget_fct();

		ImGui::TableNextRow();
	}
} // fzn