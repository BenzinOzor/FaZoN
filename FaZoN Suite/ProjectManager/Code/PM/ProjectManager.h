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
};
