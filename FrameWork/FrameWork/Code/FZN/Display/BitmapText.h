#pragma once

#include <string>
#include <vector>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>


#include "FZN/Defines.h"

namespace fzn
{
	class BitmapFont;

	class FZN_EXPORT BitmapText : public sf::Drawable, public sf::Transformable
	{
	public:
		enum Anchor
		{
			eTopLeft,
			eTopCenter,
			eTopRight,
			eMiddleLeft,
			eMiddleCenter,
			eMiddleRight,
			eBottomLeft,
			eBottomCenter,
			eBottomRight,
			eNbAnchors,
		};
		enum Alignment
		{
			eLeft,
			eCenter,
			eRight,
			eNbAlignments,
		};

		BitmapText();
		~BitmapText();

		void					SetFont( BitmapFont* _pFont );
		void					SetText( const std::string& _sText );
		std::string				GetText() const;
		void					FormatText( const char* _message, ... );
		void					SetAnchorAndAlignment( const Anchor& _eAnchor, const Alignment& _eAlignment );
		void					SetAnchor( const Anchor& _eAnchor );
		void					SetAlignment( const Alignment& _eAlignment );
		void					SetVisible( bool _bVisible );
		bool					IsVisible() const;
		void					SetColor( const sf::Color& _oColor );
		sf::Color				GetColor() const;
		void					ColorLetter( const sf::Color& _oColor, int _iLetterIndex );
		void					SetColorGlyphs( bool _bColor );
		sf::FloatRect			GetBounds() const;

	protected:
		virtual void			draw( sf::RenderTarget& _oTarget, sf::RenderStates _oStates) const;

		sf::Vector2f			_GetAnchorOffset() const;
		sf::Vector2f			_GetLineAlignmentOffset( const sf::Vector2f& _vCurrentOffset, float _fLineWidth ) const;

		BitmapFont*				m_pFont;
		std::string				m_sText;
		sf::VertexArray			m_oVertices;
		std::vector< std::pair< int, sf::Texture* > > m_oBitmapGlyphs;
		Anchor					m_eAnchor;
		Alignment				m_eAlignment;
		sf::Vector2f			m_vBounds;
		std::vector< std::pair< int, float > >	m_oLinesWidths;
		bool					m_bVisible;
		sf::Color				m_oColor;
		bool					m_bColorGlyphs;
	};
} // namespace fzn