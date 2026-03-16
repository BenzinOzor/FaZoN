#pragma once

#include <FZN/Application/FazonSuiteApp.h>
#include <FZN/Tools/Logging.h>


class ProjectManager : public fzn::FazonSuiteApp
{
public:
	ProjectManager();
	~ProjectManager();

protected:
	void _custom_menu_bar() override;
	void _custom_display() override;

private:
	void _retrieve_categories();
	bool _is_project_valid() const;

private:
	std::string m_solution_path{};

	StringVector m_categories;
	std::string m_selected_category{};

	std::string m_project_name{};
	std::string m_source_folder_name{ "Sources" };
};
