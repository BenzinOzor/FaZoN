#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include "FZN/Defines.h"


namespace fzn
{
	class FZN_EXPORT Line : public sf::Drawable
	{
	public:
		Line( float _thickness = 1.f );
		Line( const sf::VertexArray& _vertex_array, float _thickness = 1.f );

		sf::Vertex& operator []( std::size_t _index );
		const sf::Vertex& operator []( std::size_t _index ) const;

		void clear();
		bool is_empty() const;
		void append( const sf::Vertex& vertex );

		void from_vertex_array( const sf::VertexArray& _vertex_array );
		void set_thickness( float _thickness );
		void set_color( const sf::Color& _color );

	private:
		virtual void draw( sf::RenderTarget& _target, sf::RenderStates _states ) const;

		void _from_lines( const sf::VertexArray& _lines );
		void _from_line_strip( const sf::VertexArray& _line );

		void _build_triangles();

		sf::VertexArray m_line;
		sf::VertexArray m_triangles;
		float m_thickness{ 1.f };
		sf::Color m_color{ sf::Color::White };
	};
};
