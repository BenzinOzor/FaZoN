#pragma once

#include "Externals/ImGui/imgui.h"

#include "FZN/Defines.h"


namespace ImGui_fzn
{
	namespace color
	{
		static const auto invalid = ImVec4{ -1.f, -1.f, -1.f, -1.f };
		static const auto transparent = ImVec4{ 0.f, 0.f, 0.f, 0.f };

		static const auto black		= ImVec4{ 0.f, 0.f, 0.f, 1.f };
		static const auto gray		= ImVec4{ 0.5f, 0.5f, 0.5f, 1.f };
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

	FZN_EXPORT bool small_slider_float( const char* label, float* v, float v_min, float v_max, const char* format );

	template< typename ...Args >
	void custom_font_text_colored( ImFont* _font, const ImColor& _color, std::string_view _text, Args... _args )
	{
		if( _font == nullptr )
		{
			ImGui::TextColored( _color, _text.data(), _args... );
			return;
		}

		ImGui::PushFont( _font );

		ImGui::TextColored( _color, _text.data(), _args... );

		ImGui::PopFont();
	}

	template< typename ...Args >
	void bold_text_colored( const ImColor& _color, std::string_view _text, Args... _args )
	{
		custom_font_text_colored( s_ImGuiFormatOptions.m_pFontBold, _color, _text, _args... );
	}

	template< typename ...Args >
	void bold_text( std::string_view _text, Args... _args )
	{
		custom_font_text_colored( s_ImGuiFormatOptions.m_pFontBold, color::white, _text, _args... );
	}

	FZN_EXPORT std::string convert_markdown_to_imgui_format( const std::string_view _text, const ImGui_fzn::ImGuiFormatOptions* _format_options = nullptr );
	//FZN_EXPORT std::string create_color_tag( const ImVec4& _color, const ImGui_fzn::ImGuiFormatOptions* _format_options = nullptr );

	FZN_EXPORT ImColor get_color( ImGuiCol_ _color_id );

	FZN_EXPORT void rect_filled( const sf::FloatRect& _rect, const ImVec4& _color );

	FZN_EXPORT bool square_button( const char* _label, float _size = 0.f );
}
