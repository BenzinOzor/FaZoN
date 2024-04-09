#pragma once

#include "Externals/ImGui/imgui.h"

#include "FZN/Defines.h"


namespace ImGui_fzn
{
	namespace color
	{
		static const auto invalid	= ImVec4{ -1.f, -1.f, -1.f, -1.f };

		static const auto black		= ImVec4{ 0.f, 0.f, 0.f, 1.f };
		static const auto white		= ImVec4{ 1.f, 1.f, 1.f, 1.f };
		static const auto red		= ImVec4{ 1.f, 0.f, 0.f, 1.f };
		static const auto green		= ImVec4{ 0.f, 1.f, 0.f, 1.f };
		static const auto blue		= ImVec4{ 0.f, 0.f, 1.f, 1.f };
		static const auto yellow	= ImVec4{ 1.f, 1.f, 0.f, 1.f };
		static const auto pink		= ImVec4{ 1.f, 0.f, 1.f, 1.f };
		static const auto cyan		= ImVec4{ 0.f, 1.f, 1.f, 1.f };

		static const auto link		= ImVec4{ 0.05f, 0.53f, 0.78f, 1.f };
	}

	struct ImGuiFormatOptions
	{
		std::string m_sBeginTag = "[[";
		std::string m_sEndTag = "]]";
		std::string m_sTagOptionsEnd = ":";
		std::string m_sTagOptionsSeparator = ";";
		std::string m_sTagOptionParameterBegin = "(";
		std::string m_sTagOptionParameterEnd = ")";
		std::string m_sTagOptionParameterSeparator = ",";

		std::string m_sOption_Bold = "b";
		std::string m_sOption_Color = "c";

		ImFont* m_pFontRegular = nullptr;
		ImFont* m_pFontBold = nullptr;
	};

	extern FZN_EXPORT ImGuiFormatOptions s_ImGuiFormatOptions;

	FZN_EXPORT bool InputText( std::string_view _label, std::string& _buffer, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = nullptr, void* user_data = nullptr );
	FZN_EXPORT bool InputTextTable( std::string_view _label, std::string& _buffer, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = nullptr, void* user_data = nullptr );

	FZN_EXPORT bool SliderBool( bool& _slider_value, std::string_view _label_left, std::string_view _label_right );

	FZN_EXPORT void BoldText( const std::string& _sText, ImVec4 _color = color::white );
	FZN_EXPORT void CustomFontText( const std::string& _sText, ImFont* _pFont, ImVec4 _color = color::white );

	FZN_EXPORT std::string convert_markdown_to_imgui_format( const std::string_view _text, const ImGui_fzn::ImGuiFormatOptions* _format_options = nullptr );
	//FZN_EXPORT std::string create_color_tag( const ImVec4& _color, const ImGui_fzn::ImGuiFormatOptions* _format_options = nullptr );
}
