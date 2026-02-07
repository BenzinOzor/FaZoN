//------------------------------------------------------------------------
//Author : Philippe OFFERMANN
//Date : 11/01/2026
//Description : Entry point of the program
//------------------------------------------------------------------------

#include <FZN/Includes.h>
#include <FZN/Managers/DataManager.h>
#include <FZN/Managers/WindowManager.h>

#include <SFML/Graphics/RenderWindow.hpp>

#include "TR/TranslatR.h"


int main()
{
	fzn::FazonCore::ProjectDesc desc{ "TranslatR", FZNProjectType::Application };
	fzn::Tools::MaskRaiseFlag( desc.m_uModules, fzn::FazonCore::CoreModuleFlags_InputModule );

	fzn::FazonCore::CreateInstance( desc );

	//Changing the titles of the window and the console
	g_pFZN_Core->ConsoleTitle( g_pFZN_Core->GetProjectName().c_str() );
	//g_pFZN_Core->HideConsole();

	g_pFZN_Core->GreetingMessage();
	g_pFZN_Core->SetConsolePosition( sf::Vector2i( 10, 10 ) );

	//Loading of the resources that don't belong in a resource group and filling of the map containing the paths to the resources)
	g_pFZN_DataMgr->LoadResourceFile( DATAPATH( "XMLFiles/Resources" ) );

	g_pFZN_WindowMgr->AddWindow( 1920, 1080, sf::Style::Close | sf::Style::Resize, g_pFZN_Core->GetProjectName().c_str() );
	g_pFZN_WindowMgr->SetWindowFramerate(60);
	g_pFZN_WindowMgr->SetWindowClearColor( sf::Color::Black );
	
	g_pFZN_WindowMgr->SetIcon( DATAPATH( "Misc/TranslatR.png" ) );

	auto app = TR::TranslatR{};

	//Game loop (add callbacks to your functions so they can be called in there)
	g_pFZN_Core->GameLoop();
}
