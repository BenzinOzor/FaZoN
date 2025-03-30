#pragma once

#include "Externals/ImGui/imgui.h"

#include "FZN/Defines.h"


namespace ImGui_fzn
{
	namespace color
	{
		inline constexpr ImVec4 invalid			{ -1.f, -1.f, -1.f, -1.f };
		inline constexpr ImVec4 transparent		{ 0.f, 0.f, 0.f, 0.f };

		inline constexpr ImVec4 black			{ 0.f, 0.f, 0.f, 1.f };
		inline constexpr ImVec4 white			{ 1.f, 1.f, 1.f, 1.f };

		inline constexpr ImVec4 gray			{ 0.6f, 0.6f, 0.6f, 1.f };
		inline constexpr ImVec4 bright_gray		{ 0.8f, 0.8f, 0.8f, 1.f };
		inline constexpr ImVec4 light_gray		{ 0.75f, 0.75f, 0.75f, 1.f };
		inline constexpr ImVec4 dark_gray		{ 0.5f, 0.5f, 0.5f, 1.f };

		inline constexpr ImVec4 red 			{ 0.8f, 0.2f, 0.2f, 1.f };
		inline constexpr ImVec4 bright_red		{ 1.f, 0.f, 0.f, 1.f };
		inline constexpr ImVec4 light_red		{ 0.95f, 0.3f, 0.3f, 1.f };
		inline constexpr ImVec4 dark_red		{ 0.45f, 0.15f, 0.15f, 1.f };

		inline constexpr ImVec4 green			{ 0.2f, 0.8f, 0.2f, 1.f };
		inline constexpr ImVec4 bright_green	{ 0.f, 1.f, 0.f, 1.f };
		inline constexpr ImVec4 light_green		{ 0.3f, 0.95f, 0.3f, 1.f };
		inline constexpr ImVec4 dark_green		{ 0.15f, 0.45f, 0.15f, 1.f };

		inline constexpr ImVec4 blue			{ 0.2f, 0.8f, 0.2f, 1.f };
		inline constexpr ImVec4 bright_blue		{ 0.f, 0.f, 1.f, 1.f };
		inline constexpr ImVec4 light_blue		{ 0.3f, 0.95f, 0.3f, 1.f };
		inline constexpr ImVec4 dark_blue		{ 0.15f, 0.45f, 0.15f, 1.f };

		inline constexpr ImVec4 yellow			{ 0.8f, 0.8f, 0.2f, 1.f };
		inline constexpr ImVec4 bright_yellow	{ 1.f, 1.f, 0.f, 1.f };
		inline constexpr ImVec4 light_yellow	{ 1.f, 0.78f, 0.19f, 1.f };
		inline constexpr ImVec4 dark_yellow		{ 0.5f, 0.37f, 0.08f, 1.f };

		inline constexpr ImVec4 magenta			{ 0.8f, 0.2f, 0.8f, 1.f };
		inline constexpr ImVec4 bright_magenta	{ 1.f, 0.f, 1.f, 1.f };
		inline constexpr ImVec4 light_magenta	{ 1.f, 0.f, 0.5f, 1.f };
		inline constexpr ImVec4 dark_magenta	{ 0.5f, 0.f, 0.2f, 1.f };

		inline constexpr ImVec4 cyan			{ 0.2f, 0.8f, 0.8f, 1.f };
		inline constexpr ImVec4 bright_cyan		{ 0.f, 1.f, 1.f, 1.f };
		inline constexpr ImVec4 light_cyan		{ 0.f, 1.f, 0.65f, 1.f };
		inline constexpr ImVec4 dark_cyan		{ 0.f, 0.5f, 0.3f, 1.f };

		inline constexpr ImVec4 orange			{ 0.8f, 0.5f, 0.2f, 1.f };

		inline constexpr ImVec4 link			{ 0.05f, 0.53f, 0.78f, 1.f };
	}

	enum class ItemColor
	{
		gray,
		red,
		green,
		blue,
		yellow,
		magenta,
		cyan,
		COUNT
	};

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

	FZN_EXPORT bool deactivable_button( const char* _label, bool _disable, const ImVec2& _size = { 0.f, 0.f } );

	FZN_EXPORT ImVec4 color_diff_alpha( const ImVec4& _color, float _new_alpha );

	FZN_EXPORT bool Filter( std::string& _buffer, const char* _hint );
}
