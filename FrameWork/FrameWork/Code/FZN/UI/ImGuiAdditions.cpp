#include <algorithm>

#include "Externals/ImGui/imgui.h"
#include "Externals/ImGui/imgui_stdlib.h"

#include "FZN/Tools/Tools.h"
#include "FZN/Managers/WindowManager.h"

#include "ImGuiAdditions.h"


namespace ImGui_fzn
{
	ImGuiFormatOptions s_ImGuiFormatOptions = ImGuiFormatOptions{};

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

	bool SliderBool( bool& _slider_value, std::string_view _label_left, std::string_view _label_right )
	{
		auto slider_temp_value{ _slider_value ? 1 : 0 };

		if( _label_left.empty() == false )
		{
			ImGui::AlignTextToFramePadding();
			ImGui::Text( _label_left.data() );
			ImGui::SameLine();
		}

		ImGui::SetNextItemWidth( 50.f );
		if( ImGui::SliderInt( _label_right.data(), &slider_temp_value, 0, 1, "" ) )
		{
			_slider_value = slider_temp_value;
			return true;
		}

		return false;
	}

	bool slider_uint8( std::string_view _label, uint8_t& _value, uint8_t _min, uint8_t _max, const char* _format /*= "%u"*/, ImGuiSliderFlags _flags /*= 0 */ )
	{
		return ImGui::SliderScalar( _label.data(), ImGuiDataType_U8, &_value, &_min, &_max, _format, _flags );
	}

	bool small_slider_float( const char* _label, float& _value, float _min, float _max, const char* _format /*= "%.3f"*/, ImGuiSliderFlags _flags /*= 0*/ )
	{
		ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, { 0.f, 0.f } );

		const bool pressed{ ImGui::SliderFloat( _label, &_value, _min, _max, _format, _flags ) };

		ImGui::PopStyleVar();
		return pressed;
	}

	bool small_slider_uint8( std::string_view _label, uint8_t& _value, uint8_t _min, uint8_t _max, const char* _format /*= "%u"*/, ImGuiSliderFlags _flags /*= 0 */ )
	{
		ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, { 0.f, 0.f } );

		const bool pressed{ slider_uint8( _label, _value, _min, _max, _format, _flags ) };

		ImGui::PopStyleVar();
		return pressed;
	}

	bool is_color_valid( const ImVec4& _color )
	{
		if( _color.x < 0.f || _color.y < 0.f || _color.z < 0.f || _color.w < 0.f )
			return false;

		return true;
	}

	std::string create_color_tag( const ImVec4& _color, const ImGuiFormatOptions* _format_options /*= nullptr */ )
	{
		if( is_color_valid( _color ) == false )
			return {};

		const ImGui_fzn::ImGuiFormatOptions& format_options = _format_options != nullptr ? *_format_options : ImGui_fzn::s_ImGuiFormatOptions;
		auto color_tag = std::string{ "" };

		std::string sColorTag = format_options.m_sOption_Color;

		sColorTag += format_options.m_sTagOptionParameterBegin;
		sColorTag += fzn::Tools::Sprintf( "%.2f%s%.2f%s%.2f", _color.x, format_options.m_sTagOptionParameterSeparator.c_str(), _color.y, format_options.m_sTagOptionParameterSeparator.c_str(), _color.z );
		sColorTag += format_options.m_sTagOptionParameterEnd;

		return color_tag;
	}

	void remove_markdown_tag( std::string& _text, const std::string_view _tag_begin, const std::string_view _tag_end )
	{
		if( _text.empty() || _tag_begin.empty() || _tag_end.empty() )
			return;

		auto tag_begin_pos = size_t{ 0 };
		auto tag_end_pos = size_t{ 0 };
		auto insert = std::string{};

		tag_begin_pos = _text.find( _tag_begin, tag_begin_pos );

		while( tag_begin_pos != std::string::npos )
		{
			tag_end_pos = _text.find( _tag_begin, tag_begin_pos + _tag_begin.size() );

			// if we don't find any end tag, it's useless to continue to look for other tags as there won't be closing ones ever again.
			if( tag_end_pos == std::string::npos )
				return;

			_text.replace( tag_begin_pos, _tag_begin.size(), "" );
			tag_end_pos -= _tag_begin.size();

			_text.replace( tag_end_pos, _tag_end.size(), "" );

			tag_begin_pos = _text.find( _tag_begin, tag_end_pos + _tag_end.size() );
		}
	}

	void format_markdown_bold_tag( std::string& _text, const ImGuiFormatOptions* _format_options /*= nullptr */ )
	{
		if( _text.empty() )
			return;

		const auto& imgui_format_options = _format_options != nullptr ? *_format_options : s_ImGuiFormatOptions;

		auto tag_bold = std::string{ "**" };
		auto tag_begin_pos = size_t{ 0 };
		auto tag_end_pos = size_t{ 0 };
		auto insert = std::string{};
		auto option_bold = imgui_format_options.m_sBeginTag + imgui_format_options.m_sOption_Bold + imgui_format_options.m_sTagOptionsEnd;

		tag_begin_pos = _text.find( tag_bold, tag_begin_pos );

		while( tag_begin_pos != std::string::npos )
		{
			tag_end_pos = _text.find( tag_bold, tag_begin_pos + tag_bold.size() );

			// if we don't find any end tag, it's useless to continue to look for other tags as there won't be closing ones ever again.
			if( tag_end_pos == std::string::npos )
				return;

			_text.replace( tag_begin_pos, tag_bold.size(), option_bold.c_str() );
			tag_end_pos -= tag_bold.size();
			tag_end_pos += option_bold.size();

			_text.replace( tag_end_pos, tag_bold.size(), imgui_format_options.m_sEndTag );

			tag_begin_pos = _text.find( tag_bold, tag_end_pos + tag_bold.size() );
		}
	}

	std::string convert_markdown_to_imgui_format( const std::string_view _text, const ImGuiFormatOptions* _format_options /*= nullptr */ )
	{
		auto formated_text = std::string{ _text };

		const auto& imgui_format_options = _format_options != nullptr ? *_format_options : s_ImGuiFormatOptions;

		auto replace_markdown = [ &formated_text ]( const std::string_view _tag_begin, const std::string_view _tag_end )
			{
				auto word_pos = size_t{ 0 };
				auto insert = std::string{};

				word_pos = formated_text.find( _tag_begin, word_pos );

				while( word_pos != std::string::npos )
				{

				}
			};

		remove_markdown_tag( formated_text, "<", ">" );
		format_markdown_bold_tag( formated_text, &imgui_format_options );

		return formated_text;
	}

	ImColor get_color( ImGuiCol_ _color_id )
	{
		if( _color_id >= ImGuiCol_::ImGuiCol_COUNT )
			return {};

		return ImGui::GetStyle().Colors[ _color_id ];
	}

	void rect_filled( const sf::FloatRect& _rect, const ImVec4& _color )
	{
		auto* draw_list{ ImGui::GetWindowDrawList() };

		if( draw_list == nullptr )
			return;

		const auto rect_pos_min = ImVec2{ _rect.left, _rect.top };
		const auto rect_pos_max = ImVec2{ rect_pos_min.x + _rect.width, rect_pos_min.y + _rect.height };

		draw_list->AddRectFilled( rect_pos_min, rect_pos_max, ImGui::ColorConvertFloat4ToU32( _color ) );
	}

	bool square_button( const char* _label, float _size /*= 0.f */ )
	{
		return ImGui::Button( _label, { _size <= 0.f ? ImGui::GetFrameHeight() : _size, 0.f } );
	}

	bool deactivable_button( const char* _label, bool _disable, const ImVec2& _size /*= { 0.f, 0.f } */ )
	{
		if( _disable )
			ImGui::BeginDisabled();

		const bool result = ImGui::Button( _label, _size );

		if( _disable )
			ImGui::EndDisabled();

		return result;
	}

	ImVec4 color_diff_alpha( const ImVec4& _color, float _new_alpha )
	{
		return { _color.x, _color.x, _color.x, _new_alpha };
	}

	bool Filter( std::string& _buffer, const char* _hint )
	{
		bool bFilterChanged{ false };

		if( ImGui::BeginTable( "##Filter", 2 ) )
		{
			ImGui::TableSetupColumn( "A", ImGuiTableColumnFlags_WidthFixed );
			ImGui::TableSetupColumn( "B", ImGuiTableColumnFlags_WidthStretch );

			ImGui::TableNextColumn();
			if( deactivable_button( "Clear", _buffer.empty() ) )
				_buffer.clear();

			ImGui::TableNextColumn();
			ImGui::PushItemWidth( -1 );
			bFilterChanged = ImGui::InputTextWithHint( "#Search", _hint, &_buffer );
			ImGui::PopItemWidth();

			ImGui::EndTable();
		}

		return bFilterChanged;
	}

	void simple_tooltip_on_hover( const std::string_view _tooltip )
	{
		if( ImGui::IsItemHovered() )
			ImGui::SetTooltip( _tooltip.data() );
	}

}
