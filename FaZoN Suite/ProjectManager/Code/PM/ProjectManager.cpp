#include <filesystem>

#include <FZN/Managers/FazonCore.h>
#include <FZN/UI/ImGui.h>

#include "PM/ProjectManager.h"



ProjectManager::ProjectManager():
	FazonSuiteApp()
{
	g_pFZN_Core->AddCallback( this, &ProjectManager::display, fzn::DataCallbackType::Display );

	fzn::Tools::mask_lower_flag( m_window_flags, ImGuiWindowFlags_MenuBar );
}

ProjectManager::~ProjectManager()
{
	g_pFZN_Core->RemoveCallback( this, &ProjectManager::display, fzn::DataCallbackType::Display );
}

void ProjectManager::_custom_menu_bar()
{
}

void ProjectManager::_custom_display()
{
	if( ImGui::Button( "Locate Solution" ) )
	{
		m_solution_path = fzn::Tools::open_file( "(*.sln) Solution Files\0*.sln\0 (*.*) All files \0*.*\0", "Select Solution" );

		if( m_solution_path.empty() == false )
			_retrieve_categories();
	}

	ImGui::SameLine();
	ImGui::AlignTextToFramePadding();
	ImGui::Text( "Solution Path:" );
	ImGui::SameLine();
	ImGui::InputTextWithHint( "##solution_path", "<Solution Path>", &m_solution_path, ImGuiInputTextFlags_ReadOnly );

	if( m_solution_path.empty() || m_categories.empty() )
		return;

	ImGui::AlignTextToFramePadding();
	ImGui::Text( "Category" );
	ImGui::SameLine();
	ImGui::SetNextItemWidth( ImGui_fzn::default_widget_size.x );
	if( ImGui::BeginCombo( "##Category", m_selected_category.c_str() ) )
	{
		for( const std::string& category : m_categories )
		{
			if( ImGui::Selectable( category.c_str(), category == m_selected_category ) )
				m_selected_category = category;
		}

		ImGui::EndCombo();
	}

	ImGui::AlignTextToFramePadding();
	ImGui::Text( "Name" );
	ImGui::SameLine();
	ImGui::SetNextItemWidth( ImGui_fzn::default_widget_size.x );
	ImGui::InputTextWithHint( "##solution_path", "<Name>", &m_project_name, ImGuiInputTextFlags_ReadOnly );

	ImGui::AlignTextToFramePadding();
	ImGui::Text( "Sources folder name" );
	ImGui::SameLine();
	ImGui::SetNextItemWidth( ImGui_fzn::default_widget_size.x );
	ImGui::InputTextWithHint( "##solution_path", "<Sources>", &m_source_folder_name, ImGuiInputTextFlags_ReadOnly );
}

void ProjectManager::_retrieve_categories()
{
	m_categories.clear();
	m_selected_category.clear();

	std::filesystem::path solution_path = m_solution_path;
	solution_path.remove_filename();

	for( const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator( solution_path ) )
	{
		if( entry.is_directory() == false )
			continue;

		const std::string file_name = entry.path().filename().string();

		if( file_name.empty() || file_name[ 0 ] == '.' )
			continue;

		FZN_LOG( "%s", entry.path().filename().string().c_str() );
		m_categories.push_back( file_name );
	}
}

bool ProjectManager::_is_project_valid() const
{
	return m_project_name.empty() == false && m_source_folder_name.empty() == false;
}
