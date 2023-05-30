#include "FZN/Includes.h"

#include "FZN/Display/BitmapFont.h"
#include "FZN/Display/BitmapText.h"
#include "FZN/Managers/DataManager.h"


namespace fzn
{
	BitmapText::BitmapText()
		: m_pFont( nullptr )
		, m_sText( "" )
		, m_oVertices( sf::PrimitiveType::Quads, 4 )
		, m_eAnchor( Anchor::eTopLeft )
		, m_eAlignment( Alignment::eLeft )
		, m_vBounds( 0.f, 0.f )
		, m_bVisible( true )
		, m_oColor( sf::Color::White )
		, m_bColorGlyphs( false )
	{
	}

	BitmapText::~BitmapText()
	{
	}

	void BitmapText::SetFont( BitmapFont* _pFont )
	{
		m_pFont = _pFont;
	}

	void BitmapText::SetText( const std::string& _sText )
	{
		if( m_pFont == nullptr || m_sText == _sText )
			return;

		const int iTextLength = _sText.length();

		if( m_sText.length() != iTextLength )
			m_oVertices.resize( iTextLength * 4 );

		m_sText = _sText;
		m_vBounds = { 0.f, 0.f };
		m_oLinesWidths.clear();
		m_oBitmapGlyphs.clear();

		float fX = 0.f;
		float fY = 0.f;
		int iNewLine = 0;
		int iVertex = 0;
		float fMaxHeight = 0.f;
		float fCurrentHeight = 0.f;
		const float fMinHeight = m_pFont->GetCharacterMinHeight();
		const float fSmallHeight = m_pFont->GetCharacterSmallHeight();
		const float fVerticalSpacing = m_pFont->GetVerticalSpacing();
		const float fCustomGlyphsOffset = m_pFont->GetCustomGlyphsOffset();

		m_oLinesWidths.push_back( std::pair< int, float >( 0, 0.f ) );

		int iChar = 0;
		while( iChar < (int)m_sText.size() )
		{
			const BitmapGlyph* pGlyph = nullptr;
			bool bIsCustomGlyph = false;

			if( m_sText[ iChar ] == '%' && ( iChar + 1 ) < (int)m_sText.size() && m_sText[ iChar + 1 ] == '%' )
			{
				int iGlyphEnd = m_sText.find( "%%", iChar + 2 );
				std::string sGlyphName = m_sText.substr( iChar + 2, iGlyphEnd - ( iChar + 2 ) );
				int iCharOffset = sGlyphName.length() + 4;

				int iComma = sGlyphName.find( "," );
				std::string sTag = "";

				if( iComma >= 0 )
				{
					sTag = sGlyphName.substr( iComma + 1 );
					sGlyphName = sGlyphName.substr( 0, iComma );
				}

				CustomBitmapGlyph* pCustomGlyph = g_pFZN_DataMgr->GetBitmapGlyph( sGlyphName, sTag );

				if( pCustomGlyph != nullptr && pCustomGlyph->m_pTexture != nullptr )
				{
					pGlyph = &pCustomGlyph->m_oGlyph;
					m_oBitmapGlyphs.push_back( std::pair< int, sf::Texture* >( iVertex, pCustomGlyph->m_pTexture ) );

					iChar += iCharOffset;
					bIsCustomGlyph = true;
				}
				else
				{
					m_sText.erase( iChar, iCharOffset );

					if( sGlyphName.empty() == false )
						m_sText.insert( iChar, "[" + sGlyphName + "]" );

					continue;
				}

			}
			else
			{
				if( m_sText[ iChar ] == '\n' )
				{
					++iNewLine;
					iVertex += 4;
					fX = 0.f;

					if( fMaxHeight == 0.f )
						fCurrentHeight += fMinHeight;
					else
						fCurrentHeight += fMaxHeight;

					fMaxHeight = 0.f;

					if( m_oLinesWidths.back().second == 0.f )
						m_oLinesWidths.back().first = iVertex;
					else
						m_oLinesWidths.push_back( std::pair< int, float >( iVertex, 0.f ) );

					++iChar;
					continue;
				}

				pGlyph = m_pFont->GetGlyph( m_sText[ iChar ] );

				if( pGlyph == nullptr )
				{
					pGlyph = m_pFont->GetGlyph( '_' );

					if( pGlyph == nullptr )
					{
						pGlyph = m_pFont->GetGlyph( ' ' );

						if( pGlyph == nullptr )
						{
							++iChar;
							continue;
						}
					}
				}

				++iChar;
			}

			if( bIsCustomGlyph )
			{
				fY = fCustomGlyphsOffset - pGlyph->m_fHeight * 0.5f;
			}
			else
			{
				if( pGlyph->m_bLowerCharacter )
					fY = fMinHeight - fSmallHeight;
				else if( pGlyph->m_fHeight < fMinHeight )
					fY = fMinHeight - pGlyph->m_fHeight;
				else
					fY = 0.f;
			}

			fY += iNewLine * fVerticalSpacing + fCurrentHeight;

			m_oVertices[ iVertex + 0 ].position = { fX,						fY };
			m_oVertices[ iVertex + 1 ].position = { fX + pGlyph->m_fWidth,	fY };
			m_oVertices[ iVertex + 2 ].position = { fX + pGlyph->m_fWidth,	fY + pGlyph->m_fHeight };
			m_oVertices[ iVertex + 3 ].position = { fX,						fY + pGlyph->m_fHeight };

			m_oVertices[ iVertex + 0 ].texCoords = { pGlyph->m_fLeft,						pGlyph->m_fTop };
			m_oVertices[ iVertex + 1 ].texCoords = { pGlyph->m_fLeft + pGlyph->m_fWidth,	pGlyph->m_fTop };
			m_oVertices[ iVertex + 2 ].texCoords = { pGlyph->m_fLeft + pGlyph->m_fWidth,	pGlyph->m_fTop + pGlyph->m_fHeight };
			m_oVertices[ iVertex + 3 ].texCoords = { pGlyph->m_fLeft,						pGlyph->m_fTop + pGlyph->m_fHeight };

			if( bIsCustomGlyph == false || m_bColorGlyphs )
			{
				m_oVertices[ iVertex + 0 ].color = m_oColor;
				m_oVertices[ iVertex + 1 ].color = m_oColor;
				m_oVertices[ iVertex + 2 ].color = m_oColor;
				m_oVertices[ iVertex + 3 ].color = m_oColor;
			}
			else
			{
				m_oVertices[ iVertex + 0 ].color = sf::Color::White;
				m_oVertices[ iVertex + 1 ].color = sf::Color::White;
				m_oVertices[ iVertex + 2 ].color = sf::Color::White;
				m_oVertices[ iVertex + 3 ].color = sf::Color::White;
			}

			fX += pGlyph->m_fWidth + m_pFont->GetKerning();

			m_oLinesWidths.back().second = fX;

			if( pGlyph->m_bLowerCharacter == false )
				InfThenAffect( fMaxHeight, pGlyph->m_fHeight );

			InfThenAffect( m_vBounds.x, fX );
			InfThenAffect( m_vBounds.y, iNewLine * fVerticalSpacing + fCurrentHeight + fMaxHeight );

			iVertex += 4;
		}

		if( iVertex < (int)m_oVertices.getVertexCount() )
			m_oVertices.resize( iVertex );

		m_oVertices.resize( m_oVertices.getVertexCount() - iNewLine * 4 );
	}

	std::string BitmapText::GetText() const
	{
		return m_sText;
	}

	void BitmapText::FormatText( const char* _message, ... )
	{
		const int iLength = 2048;
		char sMessage[ iLength ];
		va_list args;
		va_start( args, _message );

		vsprintf_s( sMessage, iLength - 1, _message, args );

		SetText( sMessage );
	}

	void BitmapText::SetAnchorAndAlignment( const Anchor& _eAnchor, const Alignment& _eAlignment )
	{
		if( _eAnchor >= Anchor::eNbAnchors || _eAlignment >= Alignment::eNbAlignments )
			return;

		m_eAnchor = _eAnchor;
		m_eAlignment = _eAlignment;
	}

	void BitmapText::SetAnchor( const Anchor& _eAnchor )
	{
		if( _eAnchor >= Anchor::eNbAnchors )
			return;

		m_eAnchor = _eAnchor;
	}

	void BitmapText::SetAlignment( const Alignment& _eAlignment )
	{
		if( _eAlignment >= Alignment::eNbAlignments )
			return;

		m_eAlignment = _eAlignment;
	}

	void BitmapText::SetVisible( bool _bVisible )
	{
		m_bVisible = _bVisible;
	}

	bool BitmapText::IsVisible() const
	{
		return m_bVisible;
	}

	void BitmapText::SetColor( const sf::Color& _oColor )
	{
		m_oColor = _oColor;

		int iBitmapGlyph = 0;
		for( unsigned int iVertex = 0; iVertex < m_oVertices.getVertexCount(); )
		{
			if( iBitmapGlyph < (int)m_oBitmapGlyphs.size() && iVertex == m_oBitmapGlyphs[ iBitmapGlyph ].first )
			{
				++iBitmapGlyph;

				iVertex += 4;
				continue;
			}

			m_oVertices[ iVertex ].color = m_oColor;
			++iVertex;
		}
	}

	sf::Color BitmapText::GetColor() const
	{
		return m_oColor;
	}

	void BitmapText::ColorLetter( const sf::Color& _oColor, int _iLetterIndex )
	{
		int iVertexIndex = _iLetterIndex * 4;

		if( iVertexIndex < 0 || iVertexIndex >= (int)m_oVertices.getVertexCount() )
			return;

		m_oVertices[ iVertexIndex + 0 ].color = _oColor;
		m_oVertices[ iVertexIndex + 1 ].color = _oColor;
		m_oVertices[ iVertexIndex + 2 ].color = _oColor;
		m_oVertices[ iVertexIndex + 3 ].color = _oColor;
	}

	void BitmapText::SetColorGlyphs( bool _bColor )
	{
		m_bColorGlyphs = _bColor;
	}

	sf::FloatRect BitmapText::GetBounds() const
	{
		return sf::FloatRect( getPosition().x, getPosition().y, m_vBounds.x, m_vBounds.y );
	}

	/*		eTopLeft,
			eTopCenter,
			eTopRight,
			eMiddleLeft,
			eMiddleCenter,
			eMiddleRight,
			eBottomLeft,
			eBottomCenter,
			eBottomRight,
			eNbAnchors,
			*/

	BitmapText::Anchor BitmapText::GetAnchorFromString( const std::string& _rAnchor )
	{
		if( _rAnchor == "TopLeft" )
			return BitmapText::Anchor::eTopLeft;

		if( _rAnchor == "TopCenter" )
			return BitmapText::Anchor::eTopCenter;

		if( _rAnchor == "TopRight" )
			return BitmapText::Anchor::eTopRight;

		if( _rAnchor == "MiddleLeft" )
			return BitmapText::Anchor::eMiddleLeft;

		if( _rAnchor == "MiddleCenter" )
			return BitmapText::Anchor::eMiddleCenter;

		if( _rAnchor == "MiddleRight" )
			return BitmapText::Anchor::eMiddleRight;

		if( _rAnchor == "BottomLeft" )
			return BitmapText::Anchor::eBottomLeft;

		if( _rAnchor == "BottomCenter" )
			return BitmapText::Anchor::eBottomCenter;

		if( _rAnchor == "BottomRight" )
			return BitmapText::Anchor::eBottomRight;

		return BitmapText::Anchor::eMiddleLeft;
	}

	std::string BitmapText::GetAnchorFromString( BitmapText::Anchor _eAnchor )
	{
		switch( _eAnchor )
		{
		case fzn::BitmapText::eTopLeft:
			return "TopLeft";
		case fzn::BitmapText::eTopCenter:
			return "TopCenter";
		case fzn::BitmapText::eTopRight:
			return "TopRight";
		case fzn::BitmapText::eMiddleLeft:
			return "MiddleLeft";
		case fzn::BitmapText::eMiddleRight:
			return "MiddleRight";
		case fzn::BitmapText::eBottomLeft:
			return "BottomLeft";
		case fzn::BitmapText::eBottomCenter:
			return "BottomCenter";
		case fzn::BitmapText::eBottomRight:
			return "BottomRight";
		case fzn::BitmapText::eMiddleCenter:
		default:
			return "MiddleCenter";
		}
	}

	void BitmapText::draw( sf::RenderTarget& _oTarget, sf::RenderStates _oStates ) const
	{
		if( m_pFont == nullptr || m_oVertices.getVertexCount() == 0 || m_bVisible == false )
			return;

		_oStates.transform *= getTransform();

		_oStates.texture = m_pFont->GetTexture();

		sf::Vector2f vAnchorOffset = _GetAnchorOffset();

		sf::VertexArray oAlignedVertices( m_oVertices );
		int iNextLine = 0;
		int iBitmapGlyph = 0;
		sf::Vector2f vAlignmentOffset = vAnchorOffset;

		for( unsigned int iVertex = 0; iVertex < oAlignedVertices.getVertexCount(); iVertex += 4 )
		{
			// New line.
			if( iNextLine < (int)m_oLinesWidths.size() && iVertex == m_oLinesWidths[ iNextLine ].first )
			{
				vAlignmentOffset = _GetLineAlignmentOffset( vAnchorOffset, m_oLinesWidths[ iNextLine ].second );
				++iNextLine;
			}

			oAlignedVertices[ iVertex + 0 ].position -= vAlignmentOffset;
			oAlignedVertices[ iVertex + 1 ].position -= vAlignmentOffset;
			oAlignedVertices[ iVertex + 2 ].position -= vAlignmentOffset;
			oAlignedVertices[ iVertex + 3 ].position -= vAlignmentOffset;

			if( iBitmapGlyph < (int)m_oBitmapGlyphs.size() && iVertex == m_oBitmapGlyphs[ iBitmapGlyph ].first )
			{
				sf::RenderStates oStates = _oStates;
				oStates.texture = m_oBitmapGlyphs[ iBitmapGlyph ].second;

				++iBitmapGlyph;

				sf::VertexArray oGlyphVerices( sf::PrimitiveType::Quads, 4 );

				oGlyphVerices[ 0 ] = oAlignedVertices[ iVertex + 0 ];
				oGlyphVerices[ 1 ] = oAlignedVertices[ iVertex + 1 ];
				oGlyphVerices[ 2 ] = oAlignedVertices[ iVertex + 2 ];
				oGlyphVerices[ 3 ] = oAlignedVertices[ iVertex + 3 ];

				oAlignedVertices[ iVertex + 0 ].texCoords = { 0.f, 0.f };
				oAlignedVertices[ iVertex + 1 ].texCoords = { 0.f, 0.f };
				oAlignedVertices[ iVertex + 2 ].texCoords = { 0.f, 0.f };
				oAlignedVertices[ iVertex + 3 ].texCoords = { 0.f, 0.f };

				if( oStates.texture != nullptr )
				{
					_oTarget.draw( oGlyphVerices, oStates );
					continue;
				}
			}
		}

		_oTarget.draw( oAlignedVertices, _oStates );
	}

	sf::Vector2f BitmapText::_GetAnchorOffset() const
	{
		sf::Vector2f vOffset( 0.f, 0.f );

		switch( m_eAnchor )
		{
		case eTopLeft:
			return { 0.f, 0.f };
		case eTopCenter:
			vOffset.x = round( m_vBounds.x * 0.5f );
			break;
		case eTopRight:
			vOffset.x = round( m_vBounds.x );
			break;
		case eMiddleLeft:
			vOffset.y = round( m_vBounds.y * 0.5f );
			break;
		case eMiddleCenter:
			vOffset.x = round( m_vBounds.x * 0.5f );
			vOffset.y = round( m_vBounds.y * 0.5f );
			break;
		case eMiddleRight:
			vOffset.x = round( m_vBounds.x );
			vOffset.y = round( m_vBounds.y * 0.5f );
			break;
		case eBottomLeft:
			vOffset.y = round( m_vBounds.y );
			break;
		case eBottomCenter:
			vOffset.x = round( m_vBounds.x * 0.5f );
			vOffset.y = round( m_vBounds.y );
			break;
		case eBottomRight:
			vOffset.x = round( m_vBounds.x );
			vOffset.y = round( m_vBounds.y );
			break;
		};

		return vOffset;
	}

	sf::Vector2f BitmapText::_GetLineAlignmentOffset( const sf::Vector2f& _vCurrentOffset, float _fLineWidth ) const
	{
		if( m_eAlignment == Alignment::eLeft || m_vBounds.x == _fLineWidth )
			return _vCurrentOffset;

		if( m_eAlignment == Alignment::eCenter )
			return _vCurrentOffset - sf::Vector2f( ( m_vBounds.x - _fLineWidth ) * 0.5f, 0.f );

		return _vCurrentOffset - sf::Vector2f( m_vBounds.x - _fLineWidth, 0.f );
	}
}
