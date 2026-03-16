#include "Externals/ImGui/imgui_internal.h"

#include "FZN/Application/Application.h"
#include "FZN/Managers/FazonCore.h"
#include "FZN/Managers/WindowManager.h"


namespace fzn
{

	Application::Application()
	{
		m_window_flags = ImGuiWindowFlags_MenuBar;
		m_window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		m_window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	void Application::display()
	{
		_handle_actions();

		const auto window_size = g_pFZN_WindowMgr->GetWindowSize();

		ImGui::SetNextWindowPos( { 0.f, 0.f } );
		ImGui::SetNextWindowSize( window_size );
		
		_push_style_vars_and_colors();

		ImGui::Begin( g_pFZN_Core->GetProjectName().c_str(), nullptr, m_window_flags );

		_display_menu_bar();
		_custom_display();

		ImGui::End();

		ImGuiContext& imgui_context = *GImGui;
		
		ImGui::PopStyleVar( imgui_context.StyleVarStack.Size );
		ImGui::PopStyleColor( imgui_context.ColorStack.Size );
	}

	void Application::_menu_item( const char* _label, std::function< void( void ) > _fct, bool _enabled /*= true*/, const char* _shortcut /*= nullptr */ )
	{
		if( ImGui::MenuItem( _label, _shortcut, false, _enabled ) )
			_fct();
	}

	void Application::_display_version()
	{
		const std::string version{ g_pFZN_Core->get_project_version() };

		if( version.empty() == false )
		{
			const ImVec2 version_size{ ImGui::CalcTextSize( version.c_str() ) };
			const sf::Vector2u window_size{ g_pFZN_WindowMgr->GetWindowSize() };

			ImGui::SameLine( window_size.x - ImGui::CalcTextSize( version.c_str() ).x - 2.f * ImGui::GetStyle().WindowPadding.x );
			ImGui::TextColored( ImGui_fzn::color::light_gray, version.c_str() );
		}
	}

	void Application::_push_style_vars_and_colors()
	{
		ImGui::PushStyleColor( ImGuiCol_WindowBg, ImVec4( 0.10f, 0.16f, 0.22f, 1.f ) );
		ImGui::PushStyleColor( ImGuiCol_CheckMark, ImVec4( 0.f, 1.f, 0.f, 1.f ) );
	}

	void Application::_display_menu_bar()
	{
		if( fzn::Tools::mask_has_flag_raised( m_window_flags, ImGuiWindowFlags_MenuBar ) && ImGui::BeginMainMenuBar() )
		{
			_custom_menu_bar();
			_display_version();

			ImGui::EndMainMenuBar();
		}
	}
} // fzn