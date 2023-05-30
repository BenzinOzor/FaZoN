//------------------------------------------------------------------------
//Author : Philippe OFFERMANN
//Date : 03.03.16
//Description : Math tools and functions
//------------------------------------------------------------------------

#ifndef _ENGINEMATH_H_
#define _ENGINEMATH_H_


namespace fzn
{
	namespace Math
	{
		const float PI		= 3.14159f;
		const float PIdiv2	= PI * 0.5f;
		const float PIdiv4	= PI * 0.25f;
		const float EPSILON = 0.00001f;

		template <typename T>	T		Square( const T& _arg )									{ return _arg * _arg; }
		template <typename T>	T		Cube( const T& _arg )									{ return _arg * _arg * _arg; }
		template <typename T>	T		Abs( const T& _arg )									{ return _arg >= 0 ? _arg : -_arg; }
		template <typename T>	bool	IsZeroByEpsilon( const T& _arg )						{ return _arg > -EPSILON && _arg < EPSILON; }
		template <typename T>	bool	IsEven( const T& _arg )									{ return !( _arg & 0x1 ); }
		template <typename T>	bool	IsOdd( const T& _arg )									{ return _arg & 0x1; }
		template <typename T>	T		Min( const T& _argA, const T& _argB )					{ return _argA < _argB ? _argA : _argB; }
		template <typename T>	T		Max( const T& _argA, const T& _argB )					{ return _argA > _argB ? _argA : _argB; }
		template <typename T>	T		Clamp( const T& _value, const T& _min, const T& _max )	{ return _value > _min ? ( _value < _max ? _value : _max ) : _min;}
		template <typename T>	T		Clamp_0( const T& _value )								{ return _value < 0 ? 0 : _value; }
		FZN_EXPORT				float	Clamp_0_1( const float& _fValue );
		FZN_EXPORT				float	DegToRad( const float& _fDeg );
		FZN_EXPORT				float	RadToDeg( const float& _fRad );
		FZN_EXPORT				int		SimgaSum( const int& _iValue );


		/////////////////VECTORS FUNCTIONS/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Vector length calculation
		//Parameter : Concerned vector
		//Return value : Length of the vector
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		FZN_EXPORT float VectorLength( const sf::Vector2f& _vector );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Vector length calculation (Square)
		//Parameter : Concerned vector
		//Return value : Length of the vector (Squared)
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		FZN_EXPORT float VectorLengthSq( const sf::Vector2f& _vector );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Vector normalisation
		//Parameter : Vector to normalize
		//Return value : Normalized vector
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		FZN_EXPORT sf::Vector2f VectorNormalization( const sf::Vector2f& _vector );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Vector normalisation
		//Parameter : Vector to normalize
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		FZN_EXPORT void VectorNormalize( sf::Vector2f& _vector );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Vectors dot product calculation
		//Parameters : Vectors for calculation
		//Return value : dot product
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		FZN_EXPORT float VectorDot( const sf::Vector2f& _vector1, const sf::Vector2f& _vector2 );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Vector cross product calculation
		//Parameter : Concerned vector
		//Return value : Length of the vector
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		FZN_EXPORT sf::Vector3f VectorCross( const sf::Vector3f& _vector1, const sf::Vector3f& _vector2 );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Angle between two vectors calculation
		//Parameters : Vectors for calculation
		//Return value : Angle in degrees
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		FZN_EXPORT float VectorAngle( const sf::Vector2f& _vector1, const sf::Vector2f& _vector2 );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Rotate a vector by a given angle (Rad)
		//Parameter 1 : Vector to rotate
		//Parameter 2 : Angle of rotation
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		FZN_EXPORT void VectorRotateR( sf::Vector2f& _vector, float _angle );
		FZN_EXPORT void VectorSetRotationR( sf::Vector2f& _vector, float _angle );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Rotate a vector by a given angle (Deg)
		//Parameter 1 : Vector to rotate
		//Parameter 2 : Angle of rotation
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		FZN_EXPORT void VectorRotateD( sf::Vector2f& _vector, float _angle );
		FZN_EXPORT void VectorSetRotationD( sf::Vector2f& _vector, float _angle );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Truncate a vector to a given length
		//Parameter 1 : Vector to truncate
		//Parameter 2 : Length to compare with
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		FZN_EXPORT void VectorTruncate( sf::Vector2f& _vector, float _maxLength );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Truncate a vector to a given length
		//Parameter 1 : Vector to truncate
		//Parameter 2 : Length to compare with
		//Return value : Trucated vector
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		FZN_EXPORT sf::Vector2f VectorTruncated( const sf::Vector2f& _vector, float _maxLength );
		
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Indicate if a point is on the right or on the left of a given vector.
		//Parameter 1 : Vector
		//Parameter 2 : Point
		//Return value : True if the point is on the right, false if on the left.
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		FZN_EXPORT bool VectorRightOrLeft( const sf::Vector2f& _vA, const sf::Vector2f& _vB );

		FZN_EXPORT float Interpolate( float _fInMin, float _fInMax, float _fOutMin, float _fOutMax, float _fValue, bool _bClamp = true );
		FZN_EXPORT sf::Vector2f Interpolate( sf::Vector2f _vInMin, sf::Vector2f _vInMax, sf::Vector2f _vOutMin, sf::Vector2f _vOutMax, sf::Vector2f _vValue, bool _bClamp = true );
		FZN_EXPORT sf::Vector2f Interpolate( float _fInMin, float _fInMax, sf::Vector2f _vOutMin, sf::Vector2f _vOutMax, float _fValue, bool _bClamp = true );
	}
} //namespace fzn

#endif //_ENGINEMATH_H_
