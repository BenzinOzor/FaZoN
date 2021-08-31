#include "FZN/Includes.h"
#include "FZN/Tools/HermiteCubicSpline.h"

namespace fzn
{
	HermiteCubicSpline::HermiteCubicSpline()
	: m_iNbSections( 100 )
	, m_oVertices( sf::LineStrip, m_iNbSections + 1 )
	, m_bLoop( false )
	{
	}

	HermiteCubicSpline::~HermiteCubicSpline()
	{
	}


	void HermiteCubicSpline::Build( float _fDistance /*= -1.f*/ )
	{
		if( m_oControlPoints.size() < 2 || m_iNbSections == 0 )
			return;

		m_oVertices.resize( m_iNbSections + 1 );
		_GenerateTangents();

		float fStep = 1.f / m_iNbSections;
		float fCurrentU = 0.f;

		for( int iSection = 0 ; iSection <= m_iNbSections ; ++iSection, fCurrentU += fStep )
		{
			const float fClampedU = Math::Min( fCurrentU, 1.f );
			m_oVertices[iSection].position = _ProcessSteps( fClampedU );
		}

		if( _fDistance > 0.f && m_oVertices.getVertexCount() >= 2 )
		{
			_GenerateLookUpTable();
			_MakeUniform( _fDistance );
		}
	}


	const sf::VertexArray& HermiteCubicSpline::GetVertices() const
	{
		return m_oVertices;
	}

	void HermiteCubicSpline::SetControlPoints( const std::vector< sf::Vector2f >& _oPositions )
	{
		m_oControlPoints.clear();

		for( int iControlPoint = 0 ; iControlPoint < _oPositions.size() ; ++iControlPoint )
			m_oControlPoints.push_back( SplineControlPoint( _oPositions[iControlPoint] ) );
	}

	void HermiteCubicSpline::AddPoint( const sf::Vector2f & _vPosition, const sf::Vector2f& _vTangent /*= sf::Vector2f( 0.f, 0.f )*/ )
	{
		m_oControlPoints.push_back( SplineControlPoint( _vPosition, _vTangent ) );
	}

	void HermiteCubicSpline::SetControlPointPosition( unsigned int _iControlPoint, const sf::Vector2f& _vPosition )
	{
		if( _iControlPoint >= static_cast<unsigned>( m_oControlPoints.size() ) )
			return;

		m_oControlPoints[_iControlPoint].m_vPosition = _vPosition;
	}

	sf::Vector2f HermiteCubicSpline::GetControlPointPosition( unsigned int _iControlPoint ) const
	{
		if( _iControlPoint >= static_cast<unsigned>( m_oControlPoints.size() ) )
			return sf::Vector2f( std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN() );

		return m_oControlPoints[_iControlPoint].m_vPosition;
	}

	void fzn::HermiteCubicSpline::SetControlPointTangent( unsigned int _iControlPoint, const sf::Vector2f & _vTangent )
	{
		if( _iControlPoint >= static_cast<unsigned>( m_oControlPoints.size() ) )
			return;

		m_oControlPoints[_iControlPoint].m_vTangent = _vTangent;
	}

	sf::Vector2f HermiteCubicSpline::GetControlPointTangent( unsigned int _iControlPoint ) const
	{
		if( _iControlPoint >= static_cast<unsigned>( m_oControlPoints.size() ) )
			return sf::Vector2f( std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN() );

		return m_oControlPoints[_iControlPoint].m_vTangent;
	}

	void HermiteCubicSpline::SetSteps( int _iStep )
	{
		if( _iStep == 0 )
			return;

		m_iNbSections = _iStep;
		m_oVertices.resize( m_iNbSections + 1 );
	}

	void HermiteCubicSpline::SetLoop( bool _bLoop )
	{
		m_bLoop = _bLoop;
	}

	bool HermiteCubicSpline::IsLooping() const
	{
		return m_bLoop;
	}

	void HermiteCubicSpline::DebugDraw( bool _bDrawControlPoints /*= true*/, int _iWindow /*= MainWindow*/ )
	{
		if( m_oControlPoints.empty() )
			return;

		g_pFZN_WindowMgr->Draw( m_oVertices, _iWindow );

		if( _bDrawControlPoints == false )
			return;

		sf::CircleShape oControlPoint( 5.f );
		oControlPoint.setFillColor( sf::Color::Green );
		oControlPoint.setOrigin( sf::Vector2f( oControlPoint.getRadius(), oControlPoint.getRadius() ) );

		sf::VertexArray oLine( sf::PrimitiveType::LineStrip, 2 );
		oLine[0].color = sf::Color::Red;
		oLine[1].color = sf::Color::Red;

		for( int iControlPoint = 0 ; iControlPoint < m_oControlPoints.size() ; ++iControlPoint )
		{
			oLine[0].position = m_oControlPoints[iControlPoint].m_vPosition;
			oLine[1].position = oLine[0].position + m_oControlPoints[iControlPoint].m_vTangent;

			g_pFZN_WindowMgr->Draw( oLine );

			oControlPoint.setPosition( m_oControlPoints[iControlPoint].m_vPosition );
			g_pFZN_WindowMgr->Draw( oControlPoint );
		}

	}


	void HermiteCubicSpline::GenerateTangents( std::vector< SplineControlPoint >& _oControlPoints, bool _bLoop /*= false*/ )
	{
		if( _oControlPoints.size() < 2 )
			return;

		int iSize = _oControlPoints.size();

		if( _oControlPoints[ 0 ].m_vTangent == sf::Vector2f( 0.f, 0.f ) )
		{
			if( _bLoop )
				GenerateControlPointTangent( _oControlPoints[ 0 ], _oControlPoints.back().m_vPosition, _oControlPoints[ 1 ].m_vPosition );
			else
				_oControlPoints[ 0 ].m_vTangent = _oControlPoints[ 1 ].m_vPosition - _oControlPoints[ 0 ].m_vPosition;
		}

		if( _oControlPoints.back().m_vTangent == sf::Vector2f( 0.f, 0.f ) )
		{
			if( _bLoop )
				GenerateControlPointTangent( _oControlPoints.back(), _oControlPoints[ iSize - 2 ].m_vPosition, _oControlPoints[ 0 ].m_vPosition );
			else
				_oControlPoints.back().m_vTangent = _oControlPoints.back().m_vPosition - _oControlPoints[ iSize - 2 ].m_vPosition;
		}

		for( int iControlPoint = 1; iControlPoint < iSize - 1; ++iControlPoint )
		{
			if( _oControlPoints[ iControlPoint ].m_vTangent != sf::Vector2f( 0.f, 0.f ) )
				continue;

			GenerateControlPointTangent( _oControlPoints[ iControlPoint ], _oControlPoints[ iControlPoint - 1 ].m_vPosition, _oControlPoints[ iControlPoint + 1 ].m_vPosition );
			/*const sf::Vector2f vPrevious = _oControlPoints[iControlPoint].m_vPosition - _oControlPoints[iControlPoint - 1].m_vPosition;
			const sf::Vector2f vNext = _oControlPoints[iControlPoint + 1].m_vPosition - _oControlPoints[iControlPoint].m_vPosition;

			_oControlPoints[iControlPoint].m_vTangent = ( vNext + vPrevious ) * 0.5f;
			Math::VectorNormalize( _oControlPoints[ iControlPoint ].m_vTangent );

			if( Math::VectorLengthSq( vNext ) > Math::VectorLengthSq( vPrevious ) )
				_oControlPoints[ iControlPoint ].m_vTangent *= Math::VectorLength( vPrevious );
			else
				_oControlPoints[ iControlPoint ].m_vTangent *= Math::VectorLength( vNext );*/

			//_oControlPoints[ iControlPoint ].m_vTangent *= 75.f;
		}
	}

	void HermiteCubicSpline::GenerateControlPointTangent( SplineControlPoint& _oControlPoint, const sf::Vector2f& _vPreviousPointPos, const sf::Vector2f& _vNextPointPos )
	{
		const sf::Vector2f vPrevious = _oControlPoint.m_vPosition - _vPreviousPointPos;
		const sf::Vector2f vNext = _vNextPointPos - _oControlPoint.m_vPosition;

		_oControlPoint.m_vTangent = ( vNext + vPrevious ) * 0.5f;
		Math::VectorNormalize( _oControlPoint.m_vTangent );

		if( Math::VectorLengthSq( vNext ) > Math::VectorLengthSq( vPrevious ) )
			_oControlPoint.m_vTangent *= Math::VectorLength( vPrevious );
		else
			_oControlPoint.m_vTangent *= Math::VectorLength( vNext );
	}

	void MulMatVector4( float* _pOut, const float* _pMat4, const float* _pVec4 )
	{
		_pOut[0] = _pMat4[0] * _pVec4[0] + _pMat4[1] * _pVec4[1] + _pMat4[2] * _pVec4[2] + _pMat4[3] * _pVec4[3];
		_pOut[1] = _pMat4[4] * _pVec4[0] + _pMat4[5] * _pVec4[1] + _pMat4[6] * _pVec4[2] + _pMat4[7] * _pVec4[3];
		_pOut[2] = _pMat4[8] * _pVec4[0] + _pMat4[9] * _pVec4[1] + _pMat4[10] * _pVec4[2] + _pMat4[11] * _pVec4[3];
		_pOut[3] = _pMat4[12] * _pVec4[0] + _pMat4[13] * _pVec4[1] + _pMat4[14] * _pVec4[2] + _pMat4[15] * _pVec4[3];
	}

	void HermiteCubicSpline::draw( sf::RenderTarget& _oTarget, sf::RenderStates _oStates ) const
	{
		if( m_oControlPoints.empty() )
			return;

		_oTarget.draw( m_oVertices );

		sf::CircleShape oControlPoint( 5.f );
		oControlPoint.setFillColor( sf::Color::Green );
		oControlPoint.setOrigin( sf::Vector2f( oControlPoint.getRadius(), oControlPoint.getRadius() ) );

		sf::VertexArray oLine( sf::PrimitiveType::LineStrip, 2 );
		oLine[ 0 ].color = sf::Color::Red;
		oLine[ 1 ].color = sf::Color::Red;

		for( int iControlPoint = 0; iControlPoint < m_oControlPoints.size(); ++iControlPoint )
		{
			oLine[ 0 ].position = m_oControlPoints[ iControlPoint ].m_vPosition;
			oLine[ 1 ].position = oLine[ 0 ].position + m_oControlPoints[ iControlPoint ].m_vTangent;

			_oTarget.draw( oLine );

			oControlPoint.setPosition( m_oControlPoints[ iControlPoint ].m_vPosition );
			_oTarget.draw( oControlPoint );
		}
	}

	void HermiteCubicSpline::_GenerateTangents()
	{
		GenerateTangents( m_oControlPoints, m_bLoop );
	}

	sf::Vector2f HermiteCubicSpline::_ProcessSteps( float _fProgression )
	{
		float fScaledProgression = _fProgression * ( m_oControlPoints.size() - 1 ); //Progression from 0  to nb CP - 1.

		if( m_bLoop )
			fScaledProgression = _fProgression * ( m_oControlPoints.size() );

		float fClampedProgression = Math::Max( fScaledProgression - 1.f, 0.f );
		fClampedProgression = ceilf( fClampedProgression );							//First point => ClampedProgression / Second point => ClampedProgression + 1

		float fFinalProgression = fScaledProgression - fClampedProgression;			//Final progression : [0, 1] between current points.

		float pU[4] = { Math::Cube( fFinalProgression ), Math::Square( fFinalProgression ), fFinalProgression, 1 };
		float pOut[4] = { 0.f, 0.f, 0.f, 0.f };

		MulMatVector4( pOut, m_pHermiteCoef, pU );

		int iP1 = static_cast<int>( fClampedProgression );
		int iP2 = iP1 + 1;

		if( iP2 == m_oControlPoints.size() )
			iP2 = 0;

		sf::Vector2f vPoint = pOut[0] * m_oControlPoints[iP1].m_vPosition + pOut[1] * m_oControlPoints[iP2].m_vPosition;
		vPoint += pOut[2] * m_oControlPoints[iP1].m_vTangent + pOut[3] * m_oControlPoints[iP2].m_vTangent;

		return vPoint;
	}

	void HermiteCubicSpline::_GenerateLookUpTable()
	{
		m_oLookUpTable.clear();

		m_oLookUpTable[0.f] = 0.f;
		float fCurrentDistance = 0.f;
		const float fStep = 1.f / m_iNbSections;

		for( unsigned int uVertex = 1 ; uVertex < m_oVertices.getVertexCount() ; ++uVertex )
		{
			const sf::Vector2f vCurrentDistance = m_oVertices[uVertex].position - m_oVertices[uVertex - 1].position;
			fCurrentDistance += Math::VectorLength( vCurrentDistance );
			m_oLookUpTable[fCurrentDistance] = Math::Min( uVertex * fStep, 1.f );
		}
	}

	void HermiteCubicSpline::_MakeUniform( const float _fDistance )
	{
		if( _fDistance <= 0.f )
			return;

		const sf::Vector2f vLastPointPosition = m_oVertices[ m_oVertices.getVertexCount() - 1].position;
		m_oVertices.clear();
		float fCurrentDistance = 0.f;
		std::map<float, float>::const_iterator itUpper = m_oLookUpTable.upper_bound( fCurrentDistance );
		std::map<float, float>::const_iterator itLower;
	
		while( itUpper != m_oLookUpTable.cend() )
		{
			itLower = itUpper;
			itLower--;

			const float fInterpolationCoeff = ( fCurrentDistance - itLower->first ) / ( itUpper->first - itLower->first );
			const float fU = itLower->second * ( 1.f - fInterpolationCoeff ) + itUpper->second * fInterpolationCoeff;

			m_oVertices.append( sf::Vertex( _ProcessSteps( fU ) ) );

			fCurrentDistance += _fDistance;
			itUpper = m_oLookUpTable.upper_bound( fCurrentDistance );
		}


		m_oVertices.append( vLastPointPosition );
	}
}
