#ifndef _FAZON_DEFINES_H_
#define _FAZON_DEFINES_H_

#include <utility>
#include <string>
#include <vector>

#include <SFML/Config.hpp>


#ifdef FRAMEWORK_EXPORTS
#define FZN_EXPORT __declspec(dllexport)
#else
#define FZN_EXPORT __declspec(dllimport)
#endif

#define FZN_IN		
#define FZN_OUT			//Out
#define FZN_INOUT

#define SupThenAffect( A, B )			if( (A) > (B) ) { (A) = (B); }
#define InfThenAffect( A, B )			if( (A) < (B) ) { (A) = (B); }

#define CheckNullptr( Arg )				{ if( Arg == nullptr ) return; }
#define CheckNullptrReturn( Arg, Ret )	{ if( Arg == nullptr ) return Ret; }
#define CheckNullptrDelete( Arg )		{ if( Arg != nullptr ) { delete Arg; Arg = nullptr; } }

#define RandIncludeMax( min, max )		( rand() % ( max - min + 1 ) + min )
#define Rand( min, max )				( rand() % ( max - min ) + min )
#define CoinFlip						( rand() % 2 == 1 )

#define ToString( Arg )					#Arg

inline constexpr float		Flt_Max{ std::numeric_limits<float>::max() };
inline constexpr float		Flt_Eps{ std::numeric_limits<float>::epsilon() };
inline constexpr uint32_t	Uint32_Max{ std::numeric_limits<uint32_t>::max() };


typedef std::vector< std::string > StringVector;
typedef std::vector< std::string_view > StringViewVector;

namespace sf
{
	class Texture;
}

enum class FZNProjectType : sf::Uint8
{
	Application,
	Game,
	COUNT_PROJECT_TYPES
};

struct BitmapGlyph
{
	float			m_fTop = 0.f;
	float			m_fLeft = 0.f;
	float			m_fWidth = 0.f;
	float			m_fHeight = 0.f;
	bool			m_bLowerCharacter = false;
};

struct CustomBitmapGlyph
{
	std::string m_sName = "";
	std::string m_sTag = "";

	BitmapGlyph m_oGlyph;
	sf::Texture* m_pTexture = nullptr;
};

#endif //_FAZON_DEFINES_H_