#ifndef __FZN_HERMITE_CUBIC_SPLINE_H__
#define __FZN_HERMITE_CUBIC_SPLINE_H__

#include <vector>

#include <SFML/Graphics/Drawable.hpp>

#include "FZN/Managers/WindowManager.h"

namespace fzn
{
	class FZN_EXPORT HermiteCubicSpline : public sf::Drawable
	{
	public:
		struct SplineControlPoint
		{
			SplineControlPoint( const sf::Vector2f& _vPosition = sf::Vector2f( 0.f, 0.f ), const sf::Vector2f& _vTangent = sf::Vector2f( 0.f, 0.f ) )
				: m_vPosition( _vPosition )
				, m_vTangent( _vTangent )
			{
			}
			sf::Vector2f m_vPosition;
			sf::Vector2f m_vTangent;
		};


		HermiteCubicSpline();
		~HermiteCubicSpline();

		void								Build( float _fDistance = -1.f );

		const std::vector< SplineControlPoint >&	GetControlPoints() const { return m_oControlPoints; }
		const sf::VertexArray&				GetVertices() const;
		void								ClearPoints() { m_oControlPoints.clear(); }
		void								SetControlPoints( const std::vector< sf::Vector2f >& _oPositions );
		void								AddPoint( const sf::Vector2f& _vPosition, const sf::Vector2f& _vTangent = sf::Vector2f( 0.f, 0.f ) );
		void								SetControlPointPosition( unsigned int _iControlPoint, const sf::Vector2f& _vPosition );
		sf::Vector2f						GetControlPointPosition( unsigned int _iControlPoint ) const;
		void								SetControlPointTangent( unsigned int _iControlPoint, const sf::Vector2f& _vTangent );
		sf::Vector2f						GetControlPointTangent( unsigned int _iControlPoint ) const;
		void								SetSteps( int _iStep );
		void								SetLoop( bool _bLoop );
		bool								IsLooping() const;

		void								DebugDraw( bool _bDrawControlPoints = true, int _iWindow = FZN_MainWindow );

		static void							GenerateTangents( std::vector< SplineControlPoint >& _oControlPoints, bool _bLoop = false );
		static void							GenerateControlPointTangent( SplineControlPoint& _oControlPoint, const sf::Vector2f& _vPreviousPointPos, const sf::Vector2f& _vNextPointPos );

	protected:
		virtual void						draw( sf::RenderTarget& _oTarget, sf::RenderStates _oStates ) const;

		void								_GenerateTangents();
		sf::Vector2f						_ProcessSteps( float _fProgression );
		void								_GenerateLookUpTable();
		void								_MakeUniform( const float _fDistance );

		std::vector< SplineControlPoint >	m_oControlPoints;
		int									m_iNbSections;
		sf::VertexArray						m_oVertices;
		bool								m_bLoop;

		std::map<float, float>				m_oLookUpTable;

		static constexpr float m_pHermiteCoef[16] =
		{
			2.f, -3.f, 0.f, 1.f,
			-2.f, 3.f, 0.f, 0.f,
			1.f, -2.f, 1.f, 0.f,
			1.f, -1.f, 0.f, 0.f
		};
	};
}

#endif //__FZN_HERMITE_CUBIC_SPLINE_H__