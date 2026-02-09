#include <FZN/Managers/FazonCore.h>
#include <FZN/UI/ImGui.h>

#include "TR/Options.h"


namespace TR
{

	Options::Options() :
		AppOptions()
	{
		m_option_window_size.x = 500.f;
		g_pFZN_Core->AddCallback( this, &Options::on_event, fzn::DataCallbackType::Event );
		_load_options();
	}

	Options::~Options()
	{
		g_pFZN_Core->RemoveCallback( this, &Options::on_event, fzn::DataCallbackType::Event );
	}

	/**
	* @brief Prepare the option window to be displayed. Setup its first state.
	**/
	void Options::open_options()
	{
		AppOptions::open_options();

		m_data_backup = m_data;
	}

	/**
	* @brief Main display function for custom options data. Called by display().
	**/
	void Options::_display_custom_options()
	{
		if( _begin_option_table() )
		{
			_first_column_label( "Load most recent project", "Automatically open the most recent project on application startup." );
			_second_column_widget( [&]() -> bool
				{
					return ImGui::Checkbox( "##LoadRecentProject", &m_data.m_load_recent_project );
				} );

			ImGui::EndTable();
		}

		_display_bindings();

		ImGui_fzn::window_bottom_table( 2, [&]()
			{
				if( ImGui_fzn::deactivable_button( "Apply", m_edited == false, true, ImGui_fzn::default_widget_size ) )
					_confirm_options();

				ImGui::TableSetColumnIndex( 2 );
				if( ImGui::Button( "Cancel", ImGui_fzn::default_widget_size ) )
					_cancel_options();
			} );
	}

	/**
	* @brief Cancel edited options and restore old ones, then close window.
	**/
	void Options::_cancel_options()
	{
		AppOptions::_cancel_options();

		m_data = m_data_backup;
	}

	/**
	* @brief Custom load function called by the default one to let user read the given json root.
	* @param _root The json root to read the custom data it contains.
	**/
	void Options::_load_options_from_json( Json::Value& _root )
	{
		m_data.m_load_recent_project = _root[ "load_recent_project" ].asBool();
	}

	/**
	* @brief Custom save function called by the default one to let user fill the given json root.
	* @param _root The json root to fill with custom data.
	**/
	void Options::_save_options_to_json( Json::Value& _root )
	{
		_root[ "load_recent_project" ] = m_data.m_load_recent_project;
	}
} // TR
