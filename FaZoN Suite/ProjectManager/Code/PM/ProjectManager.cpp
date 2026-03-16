#include <FZN/Managers/FazonCore.h>

#include "PM/ProjectManager.h"



ProjectManager::ProjectManager():
	FazonSuiteApp()
{
	g_pFZN_Core->AddCallback( this, &ProjectManager::display, fzn::DataCallbackType::Display );
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
	
}
