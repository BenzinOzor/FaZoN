//------------------------------------------------------------------------
//Author : Philippe OFFERMANN
//Date : 21.12.13
//Description : Miscellaneous function gathering
//------------------------------------------------------------------------

#ifndef _ENGINETOOLS_H_
#define _ENGINETOOLS_H_

#include <tinyXML2/tinyxml2.h>
#include <SFML/Config.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>

#include "FZN/Defines.h"


namespace sf
{
	class RectangleShape;
	class CircleShape;
	class Sprite;
	class Text;
}

namespace fzn
{
	namespace Tools
	{
		FZN_EXPORT sf::FloatRect	ConvertIntRectToFloat( const sf::IntRect& _oIntRect );
		FZN_EXPORT sf::IntRect		ConvertFloatRectToInt( const sf::FloatRect& _oIntRect );
		FZN_EXPORT sf::FloatRect	ConvertRectangleShapeToFloatRect( const sf::RectangleShape& _oShape );

		FZN_EXPORT bool MaskHasFlagRaised( const sf::Uint8& _uMask, const sf::Uint8& _uFlag );
		FZN_EXPORT bool MaskHasFlagRaised( const sf::Uint16& _uMask, const sf::Uint16& _uFlag );
		FZN_EXPORT void MaskRaiseFlag( sf::Uint8& _uMask, const sf::Uint8& _uFlag );
		FZN_EXPORT void MaskRaiseFlag( sf::Uint16& _uMask, const sf::Uint16& _uFlag );
		FZN_EXPORT void MaskLowerFlag( sf::Uint8& _uMask, const sf::Uint8& _uFlag );
		FZN_EXPORT void MaskLowerFlag( sf::Uint16& _uMask, const sf::Uint16& _uFlag );
		FZN_EXPORT int	GetNbElementsInMask( const sf::Uint16& _uMask, const sf::Uint16& _uMaxValue );

		FZN_EXPORT std::string XMLStringAttribute( const tinyxml2::XMLElement* _pElement, const std::string& _sAttribute, const std::string& _sDefaultValue = "" );

		FZN_EXPORT std::string	GetSpacedString( const std::string& _sString, bool _bCapitalizeFirstLetter = false );
		FZN_EXPORT void			InsertSpacesInString( std::string& _sString, bool _bCapitalizeFirstLetter = false );
		FZN_EXPORT std::string	GetToLowerString( const std::string& _sString );
		FZN_EXPORT void			ToLower( std::string& _sString );
		FZN_EXPORT std::string	GetToUpperString( const std::string& _sString );
		FZN_EXPORT void			ToUpper( std::string& _sString );

		FZN_EXPORT void			ConvertSlashesInWindowsPath( std::string& _sPath );
		FZN_EXPORT void			ConvertToAntiSlashes( std::string& _sPath );
		FZN_EXPORT std::string	ConvertToAntiSlashes( const std::string& _sPath );
		FZN_EXPORT std::string	GetFileNameFromPath( const std::string& _sPath );

		/////////////////COLLISION FUNCTIONS/////////////////

		enum class CollisionSide : sf::Int8
		{
			Left = 1,
			Right = Left << 1,
			Down = Left << 2,
			Up = Left << 3,

			NoCollision = 0
		};

		inline CollisionSide operator|( CollisionSide _A, CollisionSide _B )
		{
			return (CollisionSide)( ( sf::Int8 )_A | ( sf::Int8 )_B );
		}
		inline CollisionSide operator&( CollisionSide _A, CollisionSide _B )
		{
			return (CollisionSide)( ( sf::Int8 )_A & ( sf::Int8 )_B );
		}
		inline CollisionSide operator*( CollisionSide _A, bool _Boolean )
		{
			return (CollisionSide)( ( sf::Int8 )_A * ( sf::Int8 )_Boolean );
		}

		FZN_EXPORT sf::Vector2f AABBAABBCollisionResponse( const sf::RectangleShape& _oBox1, const sf::RectangleShape& _oBox2, const sf::Vector2f& _vBox2Direction );
		FZN_EXPORT sf::FloatRect AABBAABBCollisionOverlap( const sf::RectangleShape& _oBox1, const sf::RectangleShape& _oBox2 );
		FZN_EXPORT sf::FloatRect AABBAABBCollisionOverlap( const sf::FloatRect& _oBox1, const sf::FloatRect& _oBox2 );
		FZN_EXPORT sf::FloatRect AABBAABBCollisionOverlap( const sf::Vector2f& _vBox1Pos, const sf::Vector2f& _vBox1Extents, const sf::Vector2f& _vBox2Pos, const sf::Vector2f& _vBox2Extents );

		FZN_EXPORT sf::Vector2f CircleCircleCollisionResponse( const sf::CircleShape& _circle1, const sf::CircleShape& _circle2, const sf::Vector2f& _vCircleDirection );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Collision test between two circleShapes
		//Parameters : Concerned circleShapes
		//Return value : The two shapes are in collision (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		FZN_EXPORT INT8 CollisionCircleCircle( const sf::CircleShape& _circle1, const sf::CircleShape& _circle2 );
		FZN_EXPORT sf::Vector2f AABBCircleCollisionResponse( const sf::RectangleShape& _rectangle, const sf::CircleShape& _circle, const sf::Vector2f& _vCircleDirection );
		FZN_EXPORT sf::Vector2f AABBCircleCollisionOverlap( const sf::RectangleShape& _rectangle, const sf::CircleShape& _circle );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Collision test between an AABB (rectangleShape) and a circleShape
		//Parameter 1 : RectangleShape
		//Parameter 2 : CircleShape
		//Return value : The two shapes are in collision (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		FZN_EXPORT INT8 CollisionAABBCircle( const sf::RectangleShape& _rectangle, const sf::CircleShape& _circle );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Collision test between an AABB (floatRect) and a circleShape
		//Parameter 1 : floatRect
		//Parameter 2 : CircleShape
		//Return value : The two shapes are in collision (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		FZN_EXPORT INT8 CollisionAABBCircle( const sf::FloatRect& _floatRect, const sf::CircleShape& _circle );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Collision test between two AABB (rectangleShape)
		//Parameters : RectangleShapes
		//Return value : The two shapes are in collision (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		FZN_EXPORT INT8 CollisionAABBAABB( const sf::RectangleShape& _rect1, const sf::RectangleShape& _rect2 );
		FZN_EXPORT INT8 CollisionAABBAABB( const sf::RectangleShape& _rect1, const sf::RectangleShape& _rect2, const sf::Vector2f& _vBox2Direction  );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Collision test between two AABB (rectangleShape and floatRect)
		//Parameter 1 : RectangleShape
		//Parameter 2 : Float Rect
		//Return value : The two shapes are in collision (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		FZN_EXPORT INT8 CollisionAABBAABB( const sf::RectangleShape& _rect1, const sf::FloatRect& _rect2 );


		/////////////////CLOCK FUNCTIONS/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Minutes convertion
		//Parameter : Timer for calculation (in seconds)
		//Return value : Minutes
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		FZN_EXPORT int TimerAsMinuts( float _timer );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Seconds convertion
		//Parameter : Timer for calculation (in seconds)
		//Return value : Seconds
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		FZN_EXPORT int TimerAsSeconds( float _timer );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Hundredth convertion
		//Parameter : Timer for calculation
		//Return value : Hundreths
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		FZN_EXPORT int TimerAsHundredth( float _timer );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Time formating in a string
		//Parameter : Timer for calculation
		//Return value : String containing formated time
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		FZN_EXPORT std::string FormatedTimer( float _timer );


		/////////////////SPRITE FUNCTIONS/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Horizontal mirror effect on a sprite
		//Parameter 1 : Sprite to affect
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		FZN_EXPORT void SpriteFlipX( sf::Sprite& _sprite );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Horizontal mirror effect on a sprite
		//Parameter 1 : Sprite to affect
		//Parameter 2 : Flip to apply (0 : inversion / 1 : Force X positive scale / -1 : Force X negative scale)
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		FZN_EXPORT void SpriteFlipX( sf::Sprite& _sprite, int _flip );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Change the texture of the sprite
		//Parameter 1 : Concerned sprite
		//Parameter 2 : Texture to apply
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		FZN_EXPORT void SpriteChangeTexture( sf::Sprite& _sprite, const sf::Texture& _texture );


		/////////////////TEXT FUNCTIONS/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Text origin changement
		//Parameter 1 : Text to modify
		//Parameter 2 : X ratio
		//Parameter 3 : Y ratio
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		FZN_EXPORT void Text_SetRelativOrigin( sf::Text* _text, const float _xRatio, const float _yRatio );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Text origin changement
		//Parameter 1 : Text to modify
		//Parameter 2 : X and Y ratio in a vector
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		FZN_EXPORT void Text_SetRelativOrigin( sf::Text* _text, const sf::Vector2f& _vRatios );
		FZN_EXPORT std::string Sprintf( const char* pFormat, ... );


		/////////////////DISPLAY FUNCTIONS/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Line drawing
		//Parameter 1 : First point position
		//Parameter 2 : Second point position
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		FZN_EXPORT void DrawLine( const sf::Vector2f& _pos1, const sf::Vector2f& _pos2 );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Line drawing
		//Parameter 1 : First point position
		//Parameter 2 : Second point position
		//Parameter 3 : Color of the line
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		FZN_EXPORT void DrawLine( const sf::Vector2f& _pos1, const sf::Vector2f& _pos2, sf::Color _color );

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//String drawing
		//Parameter 1 : First point position
		//Parameter 2 : Location of the string on the screen
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		FZN_EXPORT void DrawString( const char* _string, const sf::Vector2f& _position );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//String drawing
		//Parameter 1 : First point position
		//Parameter 2 : Location of the string on the screen
		//Parameter 3 : Characters size
		//Parameter 4 : Color of the string
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		FZN_EXPORT void DrawString( const char* _string, const sf::Vector2f& _position, UINT _characterSize, sf::Color _color = sf::Color::White );
		FZN_EXPORT void DrawString( const char* _string, const sf::Vector2f& _position, UINT _characterSize, sf::Color _color = sf::Color::White, int _iWindowId = 0 );
	} //namespace Tools
} //namespace fzn

#endif //_ENGINETOOLS_H_