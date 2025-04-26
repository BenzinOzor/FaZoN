//------------------------------------------------------------------------
//Author : Philippe OFFERMANN
//Date : 03.03.16
//Description : Math tools and functions
//------------------------------------------------------------------------

#include "FZN/Includes.h"
//#include <xtgmath.h>


namespace fzn
{
	namespace Math
	{
		float Clamp_0_1( const float& _fValue )
		{
			if( _fValue < 0.f )
				return 0.f;

			if( _fValue > 1.f )
				return 1.f;

			return _fValue;
		}

		float DegToRad( const float& _fDeg )
		{
			return _fDeg * PI / 180.f;
		}

		float RadToDeg( const float& _fRad )
		{
			return _fRad * 180.f / PI;
		}


		int SimgaSum( const int& _iValue )
		{
			return ( Square( _iValue ) + _iValue ) / 2;
		}

		uint8_t get_number_of_digits( uint32_t _number )
		{
			if( _number == 0 )
				return 1;

			const auto number{ static_cast< double >( _number ) };

			return static_cast< uint8_t >( log10( number ) ) + 1;
		}

		/////////////////VECTORS FUNCTIONS/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Vector length calculation
		//Parameter : Concerned vector
		//Return value : Length of the vector
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		float VectorLength( const sf::Vector2f& _v )
		{
			return sqrt( _v.x*_v.x + _v.y*_v.y );
		}

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Vector length calculation (Square)
		//Parameter : Concerned vector
		//Return value : Length of the vector (Squared)
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		float VectorLengthSq( const sf::Vector2f& _v )
		{
			return Square( _v.x ) + Square( _v.y );
		}

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Vector normalisation
		//Parameter : Vector to normalize
		//Return value : Normalized vector
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		sf::Vector2f VectorNormalization( const sf::Vector2f& _v )
		{
			float fLength = VectorLength( _v );
			
			if( fLength == 0.f )
				return _v;

			return sf::Vector2f( _v.x / fLength, _v.y / fLength );
		}

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Vector normalisation
		//Parameter : Vector to normalize
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void VectorNormalize( sf::Vector2f& _vector )
		{
			float fLength = VectorLength( _vector );

			if( fLength == 0.f )
				return;

			_vector.x /= fLength;
			_vector.y /= fLength;
		}

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Vectors scalar product calculation
		//Parameters : Vectors for calculation
		//Return value : Scalar product
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		float VectorDot( const sf::Vector2f& _v1, const sf::Vector2f& _v2 )
		{
			return _v1.x*_v2.x + _v1.y*_v2.y;
		}

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Vector cross product calculation
		//Parameters : Concerned vectors
		//Return value : Perpendicular vector
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		sf::Vector3f VectorCross( const sf::Vector3f& _vector1, const sf::Vector3f& _vector2 )
		{
			sf::Vector3f vRet;

			vRet.x = _vector1.y * _vector2.z - _vector1.z * _vector2.y;
			vRet.y = - ( _vector1.x * _vector2.z - _vector1.z * _vector2.x );
			vRet.z = _vector1.x * _vector2.y - _vector1.y * _vector2.x;

			return vRet;
		}

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Vector cross product calculation
		//Parameters : Concerned 2D vectors
		//Return value : Perpendicular vector
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		sf::Vector2f VectorCross2D( const sf::Vector2f& _v1, const sf::Vector2f& _v2 )
		{
			sf::Vector3f vResult = VectorCross( { _v1.x, _v1.y, 0.f }, { _v2.x, _v2.y, 0.f } );

			return { vResult.x, vResult.y };
		}

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Angle between two vectors calculation
		//Parameters : Vectors for calculation
		//Return value : Angle in degrees
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		float VectorAngle( const sf::Vector2f& _vector1, const sf::Vector2f& _vector2 )
		{
			/*float fScalarProduct = _vector1.x*_vector2.x + _vector1.y*_vector2.y;
			float fLength1 = sqrt( _vector1.x*_vector1.x + _vector1.y*_vector1.y ), fLength2 = sqrt( _vector2.x*_vector2.x + _vector2.y*_vector2.y );

			return ( acos( fScalarProduct / ( fLength1*fLength2 ) ) * 180.f ) / PI;*/

			float fTanA = atan2( _vector1.y, _vector1.x );
			float fTanB = atan2( _vector2.y, _vector2.x );
			float fAngle = atan2( _vector1.y, _vector1.x ) - atan2( _vector2.y, _vector2.x );
			return fAngle * 180.f / PI;
		}

		float VectorAngle360( const sf::Vector2f& _vector1, const sf::Vector2f& _vector2 )
		{
			float fTanA = atan2( _vector1.y, _vector1.x );
			float fTanB = atan2( _vector2.y, _vector2.x );
			float fAngle = atan2( _vector1.y, _vector1.x ) - atan2( _vector2.y, _vector2.x );
			float fAngleDeg = fAngle * 180.f / PI;

			if( fAngleDeg < 0.f )
				fAngleDeg = 180.f + fAngleDeg * -1.f;

			return fAngleDeg;
		}

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Rotate a vector by a given angle (Rad)
		//Parameter 1 : Vector to rotate
		//Parameter 2 : Angle of rotation
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void VectorRotateR( sf::Vector2f& _vector, float _angle )
		{
			float X = cosf( _angle ) * _vector.x - sinf( _angle ) * _vector.y;
			_vector.y = sinf( _angle ) * _vector.x + cosf( _angle ) * _vector.y;
			_vector.x = X;
		}

		void VectorSetRotationR( sf::Vector2f& _vector, float _angle )
		{
			_vector.x = cosf( _angle );
			_vector.y = sinf( _angle );
		}

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Rotate a vector by a given angle (Deg)
		//Parameter 1 : Vector to rotate
		//Parameter 2 : Angle of rotation
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void VectorRotateD( sf::Vector2f& _vector, float _angle )
		{
			VectorRotateR( _vector, DegToRad( _angle ) );
		}

		void VectorSetRotationD( sf::Vector2f& _vector, float _angle )
		{
			VectorSetRotationR( _vector, DegToRad( _angle ) );
		}

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Truncate a vector to a given length
		//Parameter 1 : Vector to truncate
		//Parameter 2 : Length to compare with
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void VectorTruncate( sf::Vector2f& _vector, float _maxLength )
		{
			if( VectorLengthSq( _vector ) > Square( _maxLength ) )
			{
				VectorNormalize( _vector );
				_vector *= _maxLength;
			}
		}

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Truncate a vector to a given length
		//Parameter 1 : Vector to truncate
		//Parameter 2 : Length to compare with
		//Return value : Trucated vector
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		sf::Vector2f VectorTruncated( const sf::Vector2f& _vector, float _maxLength )
		{
			sf::Vector2f vector( _vector );
			if( VectorLengthSq( vector ) > Square( _maxLength ) )
			{
				VectorNormalize( vector );
				vector *= _maxLength;
				return vector;
			}
			else return vector;
		}

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Indicate if a point is on the right or on the left of a given vector.
		//Parameter 1 : Vector
		//Parameter 2 : Point
		//Return value : True if the point is on the right, false if on the left.
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		bool VectorRightOrLeft( const sf::Vector2f& _vA, const sf::Vector2f& _vB )
		{
			return ( _vA.x * _vB.y - _vA.y * _vB.x ) > 0.f;
		}

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Indicate if two vectors go in the same direction by computing their dot product.
		//Parameters : Vectors to test
		//Return value : True if they go in the same direction or are perpendicular (dot >= 0).
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		bool VectorsSameDirection( const sf::Vector2f& _vVectorA, const sf::Vector2f& _vVectorB )
		{
			return VectorDot( _vVectorA, _vVectorB ) >= 0.f;
		}

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Indicate if two vectors are perpendicular by computing their dot product.
		//Parameters : Vectors to test
		//Return value : True if they are perpendicular (dot is 0).
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		bool VectorsPerpendicular( const sf::Vector2f& _vVectorA, const sf::Vector2f& _vVectorB )
		{
			return VectorDot( _vVectorA, _vVectorB ) == 0.f;
		}

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Compute the perpendicular vector to the given vector and direction.
		//Parameter 1 : Vector to which the resulting one will be perpendicular
		//Parameter 2 : Direction in which the perpendicular vector must go
		//Return value : Perpendicular vector
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		sf::Vector2f VectorsGetPerpendicular( const sf::Vector2f& _vVector, const sf::Vector2f& _vDirection )
		{
			sf::Vector3f vSegment3D{ _vVector.x, _vVector.y, 0.f };
			sf::Vector3f vCross = VectorCross( vSegment3D, { _vDirection.x, _vDirection.y, 0.f } );
			sf::Vector3f vResult3D = VectorCross( vCross, vSegment3D );

			return { vResult3D.x, vResult3D.y };
		}

		sf::Vector2f vector_get_perpendicular( const sf::Vector2f& _vector, bool _clockwise /*= true*/ )
		{
			if( _clockwise )
				return { - _vector.y, _vector.x };

			return { _vector.y, - _vector.x };
		}

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Look for the farthest point in a shape in a given direction.
		//Parameter 1 : The shape containing the points
		//Parameter 2 : The direction to be tested
		//Return value : The farthest point in the given direction
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		sf::Vector2f GetFarthestPointInDirection( const sf::Shape* _pShape, const sf::Vector2f& _vDirection )
		{
			if( _pShape == nullptr )
				return { 0.f, 0.f };

			float fMaxProj = -FLT_MAX;
			float fDot{ 0.f };
			sf::Vector2f vFarthestPoint{ 0.f, 0.f };
			sf::Vector2f vCurrentPoint{ 0.f, 0.f };

			const sf::Transform& rTransform = _pShape->getTransform();

			for( int iPoint = 0; iPoint < _pShape->getPointCount(); ++iPoint )
			{
				vCurrentPoint = rTransform.transformPoint( _pShape->getPoint( iPoint ) );
				fDot = VectorDot( _vDirection, vCurrentPoint );

				if( fMaxProj < fDot )
				{
					fMaxProj = fDot;
					vFarthestPoint = vCurrentPoint;
				}
			}

			return vFarthestPoint;
		}

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Returns the farthest point in a direction in a Minkowski Sum of two shapes
		//Parameter 1 & 2 : The two shapes to do the Minkowski Sum on
		//Parameter 3 : The direction to be tested
		//Return value : The farthest point in the given direction
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		sf::Vector2f GetSupportPoint( const sf::Shape* _pShapeA, const sf::Shape* _pShapeB, const sf::Vector2f& _vDirection )
		{
			// We don't have to do the entire Minkowski Sum of the two shapes,
			// we just have to find the farthest points of shape A and B in the given direction and its negative and substract them.
			// https://dyn4j.org/2010/04/gjk-gilbert-johnson-keerthi/

			if( _pShapeA == nullptr || _pShapeB == nullptr )
				return { 0.f, 0.f };

			// Get points on the edge of the shapes in opposite directions
			const sf::Vector2f vPointA = GetFarthestPointInDirection( _pShapeA, _vDirection );
			const sf::Vector2f vPointB = GetFarthestPointInDirection( _pShapeB, -_vDirection );

			return vPointA - vPointB;
		}

		FZN_EXPORT float Interpolate( float _fInMin, float _fInMax, float _fOutMin, float _fOutMax, float _fValue, bool _bClamp /*= true */ )
		{
			const float fRatio = ( _fValue - _fInMin ) / ( _fInMax - _fInMin );
			float fResult = _fOutMin + fRatio * ( _fOutMax - _fOutMin );

			if( _bClamp )
			{
				if( _fOutMin < _fOutMax )
					fResult = Clamp( fResult, _fOutMin, _fOutMax );
				else
					fResult = Clamp( fResult, _fOutMax, _fOutMin );
			}

			return fResult;
		}

		FZN_EXPORT sf::Vector2f Interpolate( sf::Vector2f _vInMin, sf::Vector2f _vInMax, sf::Vector2f _vOutMin, sf::Vector2f _vOutMax, sf::Vector2f _vValue, bool _bClamp /*= true */ )
		{
			float fX = Interpolate( _vInMin.x, _vInMax.x, _vOutMin.x, _vOutMax.x, _vValue.x, _bClamp );
			float fY = Interpolate( _vInMin.y, _vInMax.y, _vOutMin.y, _vOutMax.y, _vValue.y, _bClamp );

			return sf::Vector2f( fX, fY );
		}

		FZN_EXPORT sf::Vector2f Interpolate( float _fInMin, float _fInMax, sf::Vector2f _vOutMin, sf::Vector2f _vOutMax, float _fValue, bool _bClamp /*= true */ )
		{
			float fX = Interpolate( _fInMin,_fInMax, _vOutMin.x, _vOutMax.x, _fValue, _bClamp );
			float fY = Interpolate( _fInMin, _fInMax, _vOutMin.y, _vOutMax.y, _fValue, _bClamp );

			return sf::Vector2f( fX, fY );
		}

	} //namespace Math
} //namespace fzn
