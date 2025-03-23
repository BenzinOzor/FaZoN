#include <algorithm>

#include <SFML/Graphics/RenderTarget.hpp>

#include "Line.h"

namespace fzn
{

	Line::Line( float _thickness /*= 1.f */ )
	{
		m_line.setPrimitiveType( sf::LineStrip );
		set_thickness( _thickness );
	}

	Line::Line( const sf::VertexArray& _vertex_array, float _thickness /*= 1.f */ )
	{
		m_line.setPrimitiveType( sf::LineStrip );
		set_thickness( _thickness );
	
		from_vertex_array( _vertex_array );
	}

	sf::Vertex& Line::operator[]( std::size_t _index )
	{
		return m_line[ _index ];
	}

	const sf::Vertex& Line::operator[]( std::size_t _index ) const
	{
		return m_line[ _index ];
	}

	void Line::clear()
	{
		m_line.clear();
	}

	bool Line::is_empty() const
	{
		return m_line.getVertexCount() == 0;
	}

	void Line::append( const sf::Vertex& vertex )
	{
		// Temp
		m_line.append( vertex );
	}

	void Line::from_vertex_array( const sf::VertexArray& _vertex_array )
	{
		m_line.clear();

		switch( _vertex_array.getPrimitiveType() )
		{
			case sf::Lines:		_from_lines( _vertex_array );		break;
			case sf::LineStrip: _from_line_strip( _vertex_array );	break;
			default: break;
		}
	}

	void Line::set_thickness( float _thickness )
	{
		if( _thickness <= 1.f )
			m_thickness = 1.f;
		else
			m_thickness = _thickness;
	}

	void Line::draw( sf::RenderTarget& _target, sf::RenderStates _states ) const
	{
		_target.draw( m_line, _states );
	}

	void Line::_from_lines( const sf::VertexArray& _lines )
	{
		if( _lines.getVertexCount() < 2 )
			return;

		// First, we get the first line
		m_line.append( _lines[ 0 ] );
		m_line.append( _lines[ 1 ] );

		if( _lines.getVertexCount() == 2 )
			return;

		// auto it = std::ranges::find( m_pixels_descs, _quad_index, &PixelDesc::m_quad_index );
		std::vector< sf::Vertex > vertices( _lines.getVertexCount() - 2 );
		
		for( std::size_t vertex{ 0 }; vertex < vertices.size(); ++vertex )
			vertices[ vertex ] = _lines[ vertex + 2 ];

		auto get_last_vertex = [&]()
		{
			return m_line[ m_line.getVertexCount() - 1 ];
		};

		int point_id{ 0 };
		std::vector< sf::Vertex >::iterator it_start_point;
		sf::Vertex transparent_vertex{ {0.f, 0.f}, sf::Color::Transparent };

		while( vertices.empty() == false )
		{
			const sf::Vertex& last_vertex{ get_last_vertex() };

			// We look for an other vertex with the same position as the last we added to the line.
			auto it{ std::ranges::find_if( vertices, [&last_vertex]( const sf::Vertex& _vertex )
			{
				if( round( last_vertex.position.x ) != round( _vertex.position.x ) )
					return false;

				if( round( last_vertex.position.y ) != round( _vertex.position.y ) )
					return false;

				return true;
			}) };

			if( it == vertices.end() )
			{
				if( vertices.empty() )
					return;

				// If there are still vertices to be found, it means there is nothing near what we added last, but we still have to add points.
				// So we add a transparent section to link what we found before with what remains to compute.
				// (Transparent because we don't want this link to show on the final result.)
				transparent_vertex.position = last_vertex.position;
				m_line.append( transparent_vertex );
				transparent_vertex.position = vertices.begin()->position;
				m_line.append( transparent_vertex );
				m_line.append( *vertices.begin() );

				continue;
			}

			int vertex_index{ it - vertices.begin() };

			// We determined the index of the point we found, now we want to know which other vertex is linked to it.
			// Vertices are added 2 by 2 to form lines, so if the index we found is even, we found the "start" of the line and the linked vertex is the next one.
			if( vertex_index % 2 == 0 )
			{
				point_id = vertex_index + 1;
				it_start_point = vertices.begin() + vertex_index;
			}
			// If the index is odd, we found the "end" of the line and the linked vertex is the previous one.
			else
			{
				point_id = vertex_index - 1;
				it_start_point = vertices.begin() + point_id;
			}

			m_line.append( vertices[ point_id ] );

			auto next_valid_it{ vertices.erase( it_start_point ) };
			vertices.erase( next_valid_it );
		}
	}

	void Line::_from_line_strip( const sf::VertexArray& _line )
	{
	}
};