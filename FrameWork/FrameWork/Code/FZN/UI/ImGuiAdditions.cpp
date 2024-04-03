#include "Externals/ImGui/imgui.h"
#include "Externals/ImGui/imgui_stdlib.h"

#include "FZN/Tools/Tools.h"

#include "ImGuiAdditions.h"


namespace ImGui_fzn
{
	bool InputText( std::string_view _label, std::string& _buffer, ImGuiInputTextFlags flags /*= 0*/, ImGuiInputTextCallback callback /*= nullptr*/, void* user_data /*= nullptr */ )
	{
		auto region_width{ ImGui::GetContentRegionMax().x };
		auto half_region{ region_width * 0.5f };
		auto text_width{ ImGui::CalcTextSize( _label.data() ).x };

		ImGui::NewLine();
		ImGui::SameLine( half_region - text_width );
		ImGui::AlignTextToFramePadding();
		ImGui::Text( _label.data() );
		ImGui::SameLine();
		ImGui::SetNextItemWidth( half_region );
		return ImGui::InputText( fzn::Tools::Sprintf( "##%s", _label.data() ).c_str(), &_buffer, flags, callback, user_data );
	}

	bool InputTextTable( std::string_view _label, std::string& _buffer, ImGuiInputTextFlags flags /*= 0*/, ImGuiInputTextCallback callback /*= nullptr*/, void* user_data /*= nullptr */ )
	{
		auto text_width{ ImGui::CalcTextSize( _label.data() ).x };

		ImGui::TableNextColumn();
		ImGui::NewLine();
		ImGui::SameLine( ImGui::GetContentRegionMax().x - text_width );
		ImGui::AlignTextToFramePadding();
		ImGui::Text( _label.data() );
		ImGui::TableNextColumn();
		ImGui::SetNextItemWidth( ImGui::GetContentRegionMax().x );
		return ImGui::InputText( fzn::Tools::Sprintf( "##%s", _label.data() ).c_str(), &_buffer, flags, callback, user_data );
	}

}
