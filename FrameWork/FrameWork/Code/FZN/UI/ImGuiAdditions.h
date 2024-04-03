#pragma once

#include "Externals/ImGui/imgui.h"

#include "FZN/Defines.h"


namespace ImGui_fzn
{
	FZN_EXPORT bool InputText( std::string_view _label, std::string& _buffer, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = nullptr, void* user_data = nullptr );
	FZN_EXPORT bool InputTextTable( std::string_view _label, std::string& _buffer, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = nullptr, void* user_data = nullptr );
};
