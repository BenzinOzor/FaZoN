#pragma once

#include <string>
#include <unordered_map>

#include "FZN/Defines.h"

namespace fzn
{
	class FZN_EXPORT BitmapFont
	{
	public:
		BitmapFont();
		~BitmapFont();

		void						LoadFromFile( const std::string& _sPath );

		sf::Texture*				GetTexture() const;
		const BitmapGlyph*			GetGlyph( const char& _cCharacter ) const;
		float						GetKerning() const;
		float						GetCharacterMinHeight() const;
		float						GetCharacterSmallHeight() const;
		float						GetVerticalSpacing() const;
		float						GetCustomGlyphsOffset() const;

	protected:
		sf::Texture*					m_pTexture;
		std::unordered_map< char, BitmapGlyph >	m_oGlyphs;
		float							m_fKerning;
		float							m_fCharacterMinHeight;
		float							m_fCharacterSmallHeight;
		float							m_fVerticalSpacing;
		float							m_fCustomGlyphsOffset;
	};
} // namespace fzn
