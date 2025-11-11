#include <algorithm>

#include <SFML/Graphics/RenderTarget.hpp>

#include "Line.h"
#include "FZN/Tools/Math.h"

namespace fzn
{

	Line::Line( float _thickness /*= 1.f */ )
	{
		m_line.setPrimitiveType( sf::LineStrip );
		m_triangles.setPrimitiveType( sf::Triangles );
		set_thickness( _thickness );
	}

	Line::Line( const sf::VertexArray& _vertex_array, float _thickness /*= 1.f */ )
	{
		m_line.setPrimitiveType( sf::LineStrip );
		m_triangles.setPrimitiveType( sf::Triangles );
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
		m_triangles.clear();
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
		clear();

		switch( _vertex_array.getPrimitiveType() )
		{
			case sf::Lines:		_from_lines( _vertex_array );		break;
			case sf::LineStrip: _from_line_strip( _vertex_array );	break;
			default: break;
		}

		if( m_line.getVertexCount() == 0 )
			return;

		_build_triangles();
	}

	void Line::set_thickness( float _thickness )
	{
		if( _thickness <= 1.f )
			m_thickness = 1.f;
		else
			m_thickness = _thickness;

		_build_triangles();
	}

	void Line::set_color( const sf::Color& _color )
	{
		m_color = _color;
	}

	void Line::draw( sf::RenderTarget& _target, sf::RenderStates _states ) const
	{
		if( m_thickness <= 1.f )
			_target.draw( m_line, _states );
		else
			_target.draw( m_triangles, _states );
	}

	void Line::_from_lines( const sf::VertexArray& _lines )
	{
		if( _lines.getVertexCount() < 2 )
			return;

		// First, we get the first line
		m_line.append( { _lines[ 0 ].position, m_color } );
		m_line.append( { _lines[ 1 ].position, m_color } );

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
				m_line.append( { vertices.begin()->position, m_color } );

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

			m_line.append( { vertices[ point_id ].position, m_color } );

			auto next_valid_it{ vertices.erase( it_start_point ) };
			vertices.erase( next_valid_it );
		}
	}

	void Line::_from_line_strip( const sf::VertexArray& _line )
	{
	}

	void Line::_build_triangles()
	{
		if( m_thickness <= 1.f )
			return;

		for( int point_id{ 0 }; point_id < m_line.getVertexCount(); ++point_id )
		{
			// We are at the end of the line, we have finished.
			if( point_id + 1 >= m_line.getVertexCount() )
				return;

			// We don't want to compute the transparent points of the base line as we don't need triangles to be linked all the way through the line.
			if( m_line[ point_id ].color == sf::Color::Transparent || m_line[ point_id ].color == sf::Color::Transparent )
				continue;

			const sf::Vertex& current_vertex{ m_line[ point_id ] };
			const sf::Vertex& next_vertex{ m_line[ point_id + 1 ] };

			const sf::Vector2f segment{ next_vertex.position - current_vertex.position };
			sf::Vector2f perpendicular_right{ Math::vector_get_perpendicular( segment ) };
			Math::VectorNormalize( perpendicular_right );
			perpendicular_right *= m_thickness * 0.5f;

			m_triangles.append( { current_vertex.position + perpendicular_right, m_color } );
			m_triangles.append( { current_vertex.position - perpendicular_right, m_color } );
			m_triangles.append( { next_vertex.position + perpendicular_right, m_color } );

			m_triangles.append( { current_vertex.position - perpendicular_right, m_color } );
			m_triangles.append( { next_vertex.position + perpendicular_right, m_color } );
			m_triangles.append( { next_vertex.position - perpendicular_right, m_color } );
		}
	}

};