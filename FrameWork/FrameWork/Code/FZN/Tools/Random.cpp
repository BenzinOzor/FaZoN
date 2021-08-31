#include "FZN/Includes.h"
#include "FZN/Tools/Random.h"

namespace fzn
{
	//-------------------------------------------------------------------------------------------------
	/// Default constructor.
	//-------------------------------------------------------------------------------------------------
	Random::Random() : m_oRandomEngine( ( sf::Uint32 )time( nullptr ) )
	{
	}

	//-------------------------------------------------------------------------------------------------
	/// Constructor.
	/// @param	_uSeed	: Seed for the random number generation.
	//-------------------------------------------------------------------------------------------------
	Random::Random( sf::Uint32 _uSeed ) : m_oRandomEngine( _uSeed )
	{
	}

	//-------------------------------------------------------------------------------------------------
	/// Destructor.
	//-------------------------------------------------------------------------------------------------
	Random::~Random()
	{
	}

	//-------------------------------------------------------------------------------------------------
	/// Returns a random value
	/// @param	_uMinValue	: Minimum random value.
	/// @param	_uMinValue	: Maximum random value (included).
	//-------------------------------------------------------------------------------------------------
	sf::Uint32 Random::operator()( sf::Uint32 _uMinValue /*= 0*/, sf::Uint32 _uMaxValue /*= UINT32_MAX*/ )
	{
		return GetValue( _uMinValue, _uMaxValue );
	}

	//-------------------------------------------------------------------------------------------------
	/// Returns a random value
	/// @param	_uMinValue	: Minimum random value.
	/// @param	_uMinValue	: Maximum random value (included).
	//-------------------------------------------------------------------------------------------------
	sf::Uint32 Random::GetValue( sf::Uint32 _uMinValue /*= 0*/, sf::Uint32 _uMaxValue /*= UINT32_MAX*/ )
	{
		if( _uMaxValue < UINT32_MAX )
			_uMaxValue++;

		return m_oRandomEngine() % ( _uMaxValue - _uMinValue ) + _uMinValue;
	}

	//-------------------------------------------------------------------------------------------------
	/// Returns true or false.
	//-------------------------------------------------------------------------------------------------
	bool Random::GetBool()
	{
		return GetValue( 0, 1 ) == 1;
	}
}
