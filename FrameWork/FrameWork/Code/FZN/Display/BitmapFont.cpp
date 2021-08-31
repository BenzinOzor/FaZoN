#include "FZN/Includes.h"

#include <tinyXML2/tinyxml2.h>

#include "FZN/Display/BitmapFont.h"
#include "FZN/Managers/DataManager.h"


namespace fzn
{
	BitmapFont::BitmapFont()
	: m_pTexture( nullptr )
	, m_fKerning( 0.f )
	, m_fCharacterMinHeight( 0.f )
	, m_fCharacterSmallHeight( 0.f )
	, m_fVerticalSpacing( 0.f )
	, m_fCustomGlyphsOffset( 0.f )
	{
	}

	BitmapFont::~BitmapFont()
	{
	}

	void BitmapFont::LoadFromFile( const std::string& _sPath )
	{
		m_oGlyphs.clear();

		tinyxml2::XMLDocument resFile;

		if( resFile.LoadFile( _sPath.c_str() ) )
		{
			FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "Failure : %s.", resFile.ErrorName() );
			return;
		}

		tinyxml2::XMLElement* pFont = resFile.FirstChildElement( "Font" );

		if( pFont == nullptr )
		{
			FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "Failure : \"Font\" tag not found." );
			return;
		}

		std::string sName = Tools::XMLStringAttribute( pFont, "Name" );

		std::string sTexturePath = _sPath.substr( 0, _sPath.find_last_of( "/" ) + 1 );
		std::string sTextureFile = Tools::XMLStringAttribute( pFont, "TexturePath" );
		sTexturePath += sTextureFile;

		m_fKerning				= pFont->FloatAttribute( "Kerning" );
		m_fCharacterMinHeight	= pFont->FloatAttribute( "CharacterMinHeight" );
		m_fCharacterSmallHeight	= pFont->FloatAttribute( "CharacterSmallHeight" );
		m_fVerticalSpacing		= pFont->FloatAttribute( "VerticalSpacing" );
		m_fCustomGlyphsOffset	= pFont->FloatAttribute( "CustomGlyphsOffset" );

		m_pTexture = g_pFZN_DataMgr->LoadTexture( sName, sTexturePath );

		tinyxml2::XMLElement* pCharacter = pFont->FirstChildElement( "Character" );

		while( pCharacter != nullptr )
		{
			BitmapGlyph oGlyph;
			char cCharacter = *Tools::XMLStringAttribute( pCharacter, "Name" ).c_str();
			
			if( m_oGlyphs.find( cCharacter ) != m_oGlyphs.end() )
			{
				pCharacter = pCharacter->NextSiblingElement();
				continue;
			}

			oGlyph.m_fTop				= pCharacter->FloatAttribute( "Top" );
			oGlyph.m_fLeft				= pCharacter->FloatAttribute( "Left" );
			oGlyph.m_fWidth				= pCharacter->FloatAttribute( "Width" );
			oGlyph.m_fHeight			= pCharacter->FloatAttribute( "Height" );
			oGlyph.m_bLowerCharacter	= pCharacter->BoolAttribute( "Lower" );

			m_oGlyphs[ cCharacter ] = oGlyph;

			pCharacter = pCharacter->NextSiblingElement();
		}
	}

	sf::Texture* BitmapFont::GetTexture() const
	{
		return m_pTexture;
	}

	const BitmapGlyph* BitmapFont::GetGlyph( const char& _cCharacter ) const
	{
		std::unordered_map< char, BitmapGlyph >::const_iterator it = m_oGlyphs.find( _cCharacter );

		if( it != m_oGlyphs.end() )
		{
			return &it->second;
		}

		return nullptr;
	}

	float BitmapFont::GetKerning() const
	{
		return m_fKerning;
	}

	float BitmapFont::GetCharacterMinHeight() const
	{
		return m_fCharacterMinHeight;
	}

	float BitmapFont::GetCharacterSmallHeight() const
	{
		return m_fCharacterSmallHeight;
	}

	float BitmapFont::GetVerticalSpacing() const
	{
		return m_fVerticalSpacing;
	}

	float BitmapFont::GetCustomGlyphsOffset() const
	{
		return m_fCustomGlyphsOffset;
	}

}
