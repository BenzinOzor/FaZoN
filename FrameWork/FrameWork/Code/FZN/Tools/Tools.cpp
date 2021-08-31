//------------------------------------------------------------------------
//Author : Philippe OFFERMANN
//Date : 21.12.13
//Description : Miscellaneous function gathering
//------------------------------------------------------------------------

#include "FZN/Includes.h"
#include "FZN/Managers/WindowManager.h"
#include "FZN/Managers/DataManager.h"
#include <iomanip>
#include <sstream>


namespace fzn
{
	namespace Tools
	{

		FZN_EXPORT sf::FloatRect ConvertIntRectToFloat( const sf::IntRect& _oIntRect )
		{
			return sf::FloatRect( (float)_oIntRect.left, (float)_oIntRect.top, (float)_oIntRect.width, (float)_oIntRect.height );
		}

		FZN_EXPORT sf::IntRect ConvertFloatRectToInt( const sf::FloatRect& _oIntRect )
		{
			return sf::IntRect( _oIntRect.left, _oIntRect.top, _oIntRect.width, _oIntRect.height );
		}

		FZN_EXPORT sf::FloatRect ConvertRectangleShapeToFloatRect( const sf::RectangleShape& _oShape )
		{
			return sf::FloatRect(	_oShape.getPosition().x - _oShape.getOrigin().x,
									_oShape.getPosition().y - _oShape.getOrigin().y,
									_oShape.getSize().x, _oShape.getSize().y );
		}

		bool MaskHasFlagRaised( const sf::Uint8& _uMask, const sf::Uint8& _uFlag )
		{
			return ( _uMask & _uFlag ) != 0;
		}

		bool MaskHasFlagRaised( const sf::Uint16& _uMask, const sf::Uint16& _uFlag )
		{
			return ( _uMask & _uFlag ) != 0;
		}

		void MaskRaiseFlag( sf::Uint8& _uMask, const sf::Uint8& _uFlag )
		{
			_uMask |= _uFlag;
		}

		void MaskRaiseFlag( sf::Uint16& _uMask, const sf::Uint16& _uFlag )
		{
			_uMask |= _uFlag;
		}

		void MaskLowerFlag( sf::Uint8& _uMask, const sf::Uint8& _uFlag )
		{
			_uMask &= ~_uFlag;
		}

		void MaskLowerFlag( sf::Uint16& _uMask, const sf::Uint16& _uFlag )
		{
			_uMask &= ~_uFlag;
		}

		int GetNbElementsInMask( const sf::Uint16& _uMask, const sf::Uint16& _uMaxValue )
		{
			int iOffset = 0;
			sf::Uint16 uValue = 1 << iOffset;

			while( uValue < _uMaxValue )
			{
				uValue = 1 << ++iOffset;
			}

			return iOffset;
		}

		std::string XMLStringAttribute( const tinyxml2::XMLElement* _pElement, const std::string& _sAttribute, const std::string& _sDefaultValue /*= ""*/ )
		{
			if( _pElement == nullptr )
				return _sDefaultValue;

			const char* sAttribute = _pElement->Attribute( _sAttribute.c_str() );

			return sAttribute == nullptr ? _sDefaultValue : sAttribute;
		}

		std::string GetSpacedString( const std::string& _sString, bool _bCapitalizeFirstLetter /*= false*/ )
		{
			std::string sRet = _sString;

			if( _bCapitalizeFirstLetter && sRet[ 0 ] >= 'a' && sRet[ 0 ] <= 'z' )
				sRet[ 0 ] -= ( 'a' - 'A' );

			for( int iChar = 1; iChar < sRet.size(); )
			{
				if( sRet[ iChar ] >= 'A' && sRet[ iChar ] <= 'Z' )
				{
					sRet.insert( iChar, " " );
					iChar += 2;
				}
				else if( sRet[ iChar ] == '_' )
					sRet[ iChar ] = ' ';
				else
					++iChar;
			}

			return sRet;
		}

		void InsertSpacesInString( std::string& _sString, bool _bCapitalizeFirstLetter /*= false */ )
		{
			_sString = GetSpacedString( _sString, _bCapitalizeFirstLetter );
		}

		std::string GetToLowerString( const std::string& _sString )
		{
			std::string sRet = _sString;

			std::for_each( sRet.begin(), sRet.end(), []( char& c )
			{
				c = tolower( c );
			} );

			return sRet;
		}

		void ToLower( std::string& _sString )
		{
			_sString = GetToLowerString( _sString );
		}

		std::string GetToUpperString( const std::string& _sString )
		{
			std::string sRet = _sString;

			std::for_each( sRet.begin(), sRet.end(), []( char& c )
			{
				c = toupper( c );
			} );

			return sRet;
		}

		void ToUpper( std::string& _sString )
		{
			_sString = GetToUpperString( _sString );
		}

		void ConvertSlashesInWindowsPath( std::string& _sString )
		{
			for( unsigned int iChar = 0; iChar < _sString.size(); ++iChar )
			{
				if( _sString[ iChar ] == '\\' )
					_sString[ iChar ] = '/';
			}
		}

		void ConvertToAntiSlashes( std::string& _sPath )
		{
			for( unsigned int iChar = 0; iChar < _sPath.size(); ++iChar )
			{
				if( _sPath[ iChar ] == '/' )
					_sPath[ iChar ] = '\\';
			}
		}

		std::string ConvertToAntiSlashes( const std::string& _sPath )
		{
			std::string sPath = _sPath;
			for( unsigned int iChar = 0; iChar < sPath.size(); ++iChar )
			{
				if( sPath[ iChar ] == '/' )
					sPath[ iChar ] = '\\';
			}

			return sPath;
		}

		std::string GetFileNameFromPath( const std::string& _sPath )
		{
			std::string sFileName( "" );

			int iSlashIndex = _sPath.find_last_of( "/" );

			int iBackSlashIndex = _sPath.find_last_of( "\\" );

			sFileName = _sPath.substr( Math::Max( iSlashIndex + 1, iBackSlashIndex + 1 ) );

			return sFileName.substr( 0, sFileName.find_last_of( "." ) );
		}

		/////////////////COLLISION FUNCTIONS/////////////////

		sf::Vector2f AABBAABBCollisionResponse( const sf::RectangleShape& _oBox1, const sf::RectangleShape& _oBox2, const sf::Vector2f& _vBox2Direction )
		{
			const sf::Vector2f& vBox1Pos = _oBox1.getPosition() - _oBox1.getOrigin();
			const sf::Vector2f& vBox2Pos = _oBox2.getPosition() - _oBox2.getOrigin() + _vBox2Direction;

			const sf::Vector2f& vBox1Extents = _oBox1.getSize();
			const sf::Vector2f& vBox2Extents = _oBox2.getSize();


			bool bThereIsCollision = vBox1Pos.x < vBox2Pos.x + vBox2Extents.x && vBox1Pos.x + vBox1Extents.x > vBox2Pos.x && vBox1Pos.y < vBox2Pos.y + vBox2Extents.y && vBox1Pos.y + vBox1Extents.y > vBox2Pos.y;
			if( !bThereIsCollision )
				return { 0.f, 0.f };


			const float fOverlapRight	= std::min( vBox1Pos.x + vBox1Extents.x, vBox2Pos.x + vBox2Extents.x );
			const float fOverlapLeft	= std::max( vBox1Pos.x, vBox2Pos.x );
			const float fOverlapBottom	= std::min( vBox1Pos.y + vBox1Extents.y, vBox2Pos.y + vBox2Extents.y );
			const float fOverlapTop		= std::max( vBox1Pos.y, vBox2Pos.y );

			sf::Vector2f vOverlapExtents( fOverlapRight - fOverlapLeft, fOverlapBottom - fOverlapTop );

			if( vOverlapExtents.x > vOverlapExtents.y )
				return { 0.f, -_vBox2Direction.y };
			else
				return { -_vBox2Direction.x, 0.f };
		}

		sf::FloatRect AABBAABBCollisionOverlap( const sf::RectangleShape& _oBox1, const sf::RectangleShape& _oBox2 )
		{
			return AABBAABBCollisionOverlap( _oBox1.getPosition() - _oBox1.getOrigin(), _oBox1.getSize(), _oBox2.getPosition() - _oBox2.getOrigin(), _oBox2.getSize() );
		}

		sf::FloatRect AABBAABBCollisionOverlap( const sf::FloatRect& _oBox1, const sf::FloatRect& _oBox2 )
		{
			return AABBAABBCollisionOverlap( { _oBox1.left, _oBox1.top }, { _oBox1.width, _oBox1.height }, { _oBox2.left, _oBox2.top }, { _oBox2.width, _oBox2.height } );
		}

		sf::FloatRect AABBAABBCollisionOverlap( const sf::Vector2f& _vBox1Pos, const sf::Vector2f& _vBox1Extents, const sf::Vector2f& _vBox2Pos, const sf::Vector2f& _vBox2Extents )
		{
			sf::FloatRect oRet( { 0.f, 0.f }, { 0.f, 0.f } );

			bool bThereIsCollision = _vBox1Pos.x < _vBox2Pos.x + _vBox2Extents.x && _vBox1Pos.x + _vBox1Extents.x > _vBox2Pos.x && _vBox1Pos.y < _vBox2Pos.y + _vBox2Extents.y && _vBox1Pos.y + _vBox1Extents.y > _vBox2Pos.y;
			if( !bThereIsCollision )
				return oRet;


			const float fOverlapRight = std::min( _vBox1Pos.x + _vBox1Extents.x, _vBox2Pos.x + _vBox2Extents.x );
			const float fOverlapLeft = std::max( _vBox1Pos.x, _vBox2Pos.x );
			const float fOverlapBottom = std::min( _vBox1Pos.y + _vBox1Extents.y, _vBox2Pos.y + _vBox2Extents.y );
			const float fOverlapTop = std::max( _vBox1Pos.y, _vBox2Pos.y );

			oRet.left = fOverlapLeft;
			oRet.top = fOverlapTop;
			oRet.width = fOverlapRight - fOverlapLeft;
			oRet.height = fOverlapBottom - fOverlapTop;

			return oRet;
		}

		sf::Vector2f CircleCircleCollisionResponse( const sf::CircleShape& _circle1, const sf::CircleShape& _circle2, const sf::Vector2f& _vCircleDirection )
		{
			const sf::Vector2f vCircle1Radius( _circle1.getRadius(), _circle1.getRadius() );
			const sf::Vector2f vCircle2Radius( _circle2.getRadius(), _circle2.getRadius() );
			const sf::Vector2f vCenterToCenter( ( _circle2.getPosition() - _circle2.getOrigin() + vCircle2Radius + _vCircleDirection ) - ( _circle1.getPosition() - _circle1.getOrigin() + vCircle1Radius ) );
			
			if( Math::VectorLengthSq( vCenterToCenter ) <= Math::Square( _circle1.getRadius() + _circle2.getRadius() ) )
				return Math::VectorNormalization( vCenterToCenter ) * Math::VectorLength( _vCircleDirection );
			else 
				return { 0.f, 0.f };
		}

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Collision test between two circleShapes
		//Parameters : Concerned circleShapes
		//Return value : The two shapes are in collision (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		INT8 CollisionCircleCircle( const sf::CircleShape& _c1, const sf::CircleShape& _c2 )
		{
			sf::Vector2f vCircle1Pos = _c1.getPosition() - _c1.getOrigin() + sf::Vector2f( _c1.getRadius(), _c1.getRadius() );
			sf::Vector2f vCircle2Pos = _c2.getPosition() - _c2.getOrigin() + sf::Vector2f( _c2.getRadius(), _c2.getRadius() );
			sf::Vector2f vDist = vCircle2Pos - vCircle1Pos;

			if( Math::VectorLengthSq( vDist ) <= Math::Square( _c1.getRadius() + _c2.getRadius() ) )
				return true;
			else return false;
		}

		sf::Vector2f AABBCircleCollisionResponse( const sf::RectangleShape& _rectangle, const sf::CircleShape& _circle, const sf::Vector2f& _vCircleDirection )
		{
			float fRadius = _circle.getRadius();

			sf::Vector2f circle( _circle.getPosition() - _circle.getOrigin() + sf::Vector2f( fRadius, fRadius ) + _vCircleDirection );
			sf::FloatRect rectBounds = _rectangle.getGlobalBounds();

			float rectHalfHeight = rectBounds.height * 0.5f;
			float rectHalfWidth = rectBounds.width * 0.5f;

			float circleDistX = fabs( circle.x - ( rectBounds.left + rectHalfWidth ) );
			float circleDistY = fabs( circle.y - ( rectBounds.top + rectHalfHeight ) );

			if( circleDistX > ( rectHalfWidth + fRadius ) || circleDistY > ( rectHalfHeight + fRadius ) )
				return { 0.f, 0.f };

			bool bCollision = circleDistX <= rectHalfWidth || circleDistY <= rectHalfHeight;
			
			if( bCollision == false )
			{
				float fCornerDistSqr = Math::Square( circleDistX - rectHalfWidth ) + Math::Square( circleDistY - rectHalfHeight );

				bCollision = fCornerDistSqr <= Math::Square( fRadius );
			}

			if( bCollision == false )
				return { 0.f, 0.f };

			const sf::Vector2f vCircleTopLeft = _circle.getPosition() - _circle.getOrigin();

			const float fOverlapRight = std::min( rectBounds.left + rectBounds.width, vCircleTopLeft.x + fRadius * 2.f );
			const float fOverlapLeft = std::max( rectBounds.left, vCircleTopLeft.x );
			const float fOverlapBottom = std::min( rectBounds.top + rectBounds.height, vCircleTopLeft.y + fRadius * 2.f );
			const float fOverlapTop = std::max( rectBounds.top, vCircleTopLeft.y );

			sf::Vector2f vOverlapExtents( fOverlapRight - fOverlapLeft, fOverlapBottom - fOverlapTop );

			if( vOverlapExtents.x > vOverlapExtents.y )
				return { 0.f, -_vCircleDirection.y };
			else
				return { -_vCircleDirection.x, 0.f };
		}

		sf::Vector2f AABBCircleCollisionOverlap( const sf::RectangleShape& _rectangle, const sf::CircleShape& _circle )
		{
			float fRadius = _circle.getRadius();

			sf::Vector2f circle( _circle.getPosition() - _circle.getOrigin() + sf::Vector2f( fRadius, fRadius ) );
			sf::FloatRect rectBounds = _rectangle.getGlobalBounds();

			float rectHalfHeight = rectBounds.height * 0.5f;
			float rectHalfWidth = rectBounds.width * 0.5f;

			float circleDistX = fabs( circle.x - ( rectBounds.left + rectHalfWidth ) );
			float circleDistY = fabs( circle.y - ( rectBounds.top + rectHalfHeight ) );

			if( circleDistX > ( rectHalfWidth + fRadius ) || circleDistY > ( rectHalfHeight + fRadius ) )
				return { 0.f, 0.f };

			bool bCollision = circleDistX <= rectHalfWidth || circleDistY <= rectHalfHeight;

			if( bCollision == false )
			{
				float fCornerDistSqr = Math::Square( circleDistX - rectHalfWidth ) + Math::Square( circleDistY - rectHalfHeight );

				bCollision = fCornerDistSqr <= Math::Square( fRadius );
			}

			if( bCollision == false )
				return { 0.f, 0.f };

			const sf::Vector2f vCircleTopLeft = _circle.getPosition() - _circle.getOrigin();

			const float fOverlapRight = std::min( rectBounds.left + rectBounds.width, vCircleTopLeft.x + fRadius * 2.f );
			const float fOverlapLeft = std::max( rectBounds.left, vCircleTopLeft.x );
			const float fOverlapBottom = std::min( rectBounds.top + rectBounds.height, vCircleTopLeft.y + fRadius * 2.f );
			const float fOverlapTop = std::max( rectBounds.top, vCircleTopLeft.y );

			return { fOverlapRight - fOverlapLeft, fOverlapBottom - fOverlapTop };
		}

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Collision test between an AABB (rectangleShape) and a circleShape
		//Parameter 1 : RectangleShape
		//Parameter 2 : CircleShape
		//Return value : The two shapes are in collision (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		INT8 CollisionAABBCircle( const sf::RectangleShape& _rect, const sf::CircleShape& _circle )
		{
			float fRadius = _circle.getRadius();

			sf::Vector2f circle( _circle.getPosition() - _circle.getOrigin() + sf::Vector2f( fRadius, fRadius ) );
			sf::FloatRect rectBounds = _rect.getGlobalBounds();

			float rectHalfHeight = rectBounds.height * 0.5f;
			float rectHalfWidth = rectBounds.width * 0.5f;

			float circleDistX = fabs( circle.x - ( rectBounds.left + rectHalfWidth ) );
			float circleDistY = fabs( circle.y - ( rectBounds.top + rectHalfHeight ) );

			if( circleDistX > ( rectHalfWidth + fRadius ) || circleDistY > ( rectHalfHeight + fRadius ) )
				return false;

			if( circleDistX <= rectHalfWidth || circleDistY <= rectHalfHeight )
				return true;

			float fCornerDistSqr = Math::Square( circleDistX - rectHalfWidth ) + Math::Square( circleDistY - rectHalfHeight );

			return fCornerDistSqr <= Math::Square( fRadius );
		}

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Collision test between an AABB (floatRect) and a circleShape
		//Parameter 1 : floatRect
		//Parameter 2 : CircleShape
		//Return value : The two shapes are in collision (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		INT8 CollisionAABBCircle( const sf::FloatRect& _rect, const sf::CircleShape& _circle )
		{
			float fRadius = _circle.getRadius();

			sf::Vector2f circle( _circle.getPosition() - _circle.getOrigin() + sf::Vector2f( fRadius, fRadius ) );

			float rectHalfHeight = _rect.height * 0.5f;
			float rectHalfWidth = _rect.width * 0.5f;

			float circleDistX = fabs( circle.x - ( _rect.left + rectHalfWidth ) );
			float circleDistY = fabs( circle.y - ( _rect.top + rectHalfHeight ) );

			if( circleDistX > ( rectHalfWidth + fRadius ) || circleDistY > ( rectHalfHeight + fRadius ) )
				return false;

			if( circleDistX <= rectHalfWidth || circleDistY <= rectHalfHeight )
				return true;

			float fCornerDistSqr = Math::Square( circleDistX - rectHalfWidth ) + Math::Square( circleDistY - rectHalfHeight );

			return fCornerDistSqr <= Math::Square( fRadius );
		}

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Collision test between two AABB (rectangleShape)
		//Parameters : RectangleShapes
		//Return value : The two shapes are in collision (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		INT8 CollisionAABBAABB( const sf::RectangleShape& _rect1, const sf::RectangleShape& _rect2 )
		{
			sf::Vector2f rect1 = _rect1.getPosition() - _rect1.getOrigin();
			sf::Vector2f rect1Size = _rect1.getSize();
			sf::Vector2f rect2 = _rect2.getPosition() - _rect2.getOrigin();
			sf::Vector2f rect2Size = _rect2.getSize();

			return ( rect1.x < rect2.x + rect2Size.x && rect1.x + rect1Size.x > rect2.x && rect1.y < rect2.y + rect2Size.y && rect1.y + rect1Size.y > rect2.y );
		}

		INT8 CollisionAABBAABB( const sf::RectangleShape& _rect1, const sf::RectangleShape& _rect2, const sf::Vector2f& _vBox2Direction )
		{
			sf::FloatRect oRect2;
			
			oRect2.top = _rect2.getPosition().y + _vBox2Direction.y;
			oRect2.left = _rect2.getPosition().x + _vBox2Direction.x;
			oRect2.width = _rect2.getSize().x;
			oRect2.height = _rect2.getSize().y;

			return CollisionAABBAABB( _rect1, oRect2 );
		}

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Collision test between two AABB (rectangleShape and floatRect)
		//Parameter 1 : RectangleShape
		//Parameter 2 : Float Rect
		//Return value : The two shapes are in collision (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		INT8 CollisionAABBAABB( const sf::RectangleShape& _rect1, const sf::FloatRect& _rect2 )
		{
			sf::Vector2f rect1 = _rect1.getPosition();
			sf::Vector2f rect1Size = _rect1.getSize();

			return ( rect1.x < _rect2.left + _rect2.width && rect1.x + rect1Size.x > _rect2.left && rect1.y < _rect2.top + _rect2.height && rect1.y + rect1Size.y > _rect2.top );
		}

		/////////////////CLOCK FUNCTIONS/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Minutes convertion
		//Parameter : Timer for calculation (in seconds)
		//Return value : Minutes
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		int	TimerAsMinuts( float _fTimer )
		{
			/* Transformation du timer en minute. */

			const unsigned int iMin = (int)( _fTimer / 60.0f );
			return iMin;
		}

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Seconds convertion
		//Parameter : Timer for calculation (in seconds)
		//Return value : Seconds
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		int	TimerAsSeconds( float _fTimer )
		{
			/* Transformation du timer en seconde. */

			const unsigned int iSec = (int)_fTimer % 60;
			return iSec;
		}

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Hundredth convertion
		//Parameter : Timer for calculation
		//Return value : Hundreths
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		int TimerAsHundredth( float _fTimer )
		{
			/* Transformation du timer en centieme de seconde. */

			const unsigned int iCent = (int)( ( _fTimer - ( TimerAsMinuts( _fTimer ) * 60 ) - TimerAsSeconds( _fTimer ) ) * 100 );
			return iCent;
		}

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Time formating in a string
		//Parameter : Timer for calculation
		//Return value : String containing formated time
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		std::string FormatedTimer( float _fTimer )
		{
			std::stringstream szTime( "" );
			szTime << std::setfill( '0' ) << std::setw( 2 ) << TimerAsMinuts( _fTimer )
				<< " : " << std::setfill( '0' ) << std::setw( 2 ) << TimerAsSeconds( _fTimer )
				<< " : " << std::setfill( '0' ) << std::setw( 2 ) << TimerAsHundredth( _fTimer );
			return szTime.str();
		}


		/////////////////SPRITE FUNCTIONS/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Horizontal mirror effect on a sprite
		//Parameter 1 : Sprite to affect
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void SpriteFlipX( sf::Sprite& _sprite )
		{
			sf::Vector2f scale( _sprite.getScale().x, _sprite.getScale().y );
			sf::IntRect spriteRect = _sprite.getTextureRect();

			_sprite.setTextureRect( sf::IntRect( spriteRect.left + spriteRect.width, spriteRect.top, -spriteRect.width, spriteRect.height ) );
		}

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Horizontal mirror effect on a sprite
		//Parameter 1 : Sprite to affect
		//Parameter 2 : Flip to apply (0 : inversion / 1 : Force X positive scale / -1 : Force X negative scale)
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void SpriteFlipX( sf::Sprite& _sprite, int _iFlip )
		{
			sf::Vector2f scale( _sprite.getScale() );
			sf::IntRect spriteRect( _sprite.getTextureRect() );

			if( scale.x == 1.f && ( _iFlip == 0 || _iFlip == -1 ) )
			{
				_sprite.setScale( -1, scale.y );
				_sprite.setOrigin( _sprite.getOrigin().x + spriteRect.width, _sprite.getOrigin().y );
			}
			else if( scale.x == -1.f && ( _iFlip == 0 || _iFlip == 1 ) )
			{
				_sprite.setScale( 1, scale.y );
				_sprite.setOrigin( _sprite.getOrigin().x - spriteRect.width, _sprite.getOrigin().y );
			}
		}

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Change the texture of the sprite
		//Parameter 1 : Concerned sprite
		//Parameter 2 : Texture to apply
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void SpriteChangeTexture( sf::Sprite& _sprite, const sf::Texture& _texture )
		{
			_sprite.setTextureRect( sf::IntRect( sf::Vector2i( 0, 0 ), sf::Vector2i( _texture.getSize() ) ) );
			_sprite.setTexture( _texture );
		}


		/////////////////TEXT FUNCTIONS/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Text origin changement
		//Parameter 1 : Text to modify
		//Parameter 2 : X ratio
		//Parameter 3 : Y ratio
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void Text_SetRelativOrigin( sf::Text* _pText, const float _fRatioX, const float _fRatioY )
		{
			_pText->setOrigin( _pText->getGlobalBounds().width * _fRatioX, _pText->getGlobalBounds().height * _fRatioY );
		}

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Text origin changement
		//Parameter 1 : Text to modify
		//Parameter 2 : X and Y ratio in a vector
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void Text_SetRelativOrigin( sf::Text* _pText, const sf::Vector2f& _fOrigin )
		{
			_pText->setOrigin( _pText->getGlobalBounds().width * _fOrigin.x, _pText->getGlobalBounds().height * _fOrigin.y );
		}

		std::string Sprintf( const char* pFormat, ... )
		{
			std::string sOut;
			va_list oArgs;
			va_start( oArgs, pFormat );
			int iLen;
			static char sTmp[ 16384 ];

			iLen = _vsnprintf_s( sTmp, 16384, pFormat, oArgs );

			sOut.assign( sTmp, iLen );

			return sOut;
		}


		/////////////////DISPLAY FUNCTIONS/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Line drawing
		//Parameter 1 : First point position
		//Parameter 2 : Second point position
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void DrawLine( const sf::Vector2f& _pos1, const sf::Vector2f& _pos2 )
		{
			sf::VertexArray line( sf::PrimitiveType::LinesStrip, 2 );

			line[ 0 ].color = sf::Color::Green;
			line[ 1 ].color = sf::Color::Green;

			line[ 0 ].position = _pos1;
			line[ 1 ].position = _pos2;

			g_pFZN_WindowMgr->Draw( line );
		}

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Line drawing
		//Parameter 1 : First point position
		//Parameter 2 : Second point position
		//Parameter 3 : Color of the line
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void DrawLine( const sf::Vector2f& _pos1, const sf::Vector2f& _pos2, sf::Color _color )
		{
			sf::VertexArray line( sf::PrimitiveType::LinesStrip, 2 );

			line[ 0 ].color = _color;
			line[ 1 ].color = _color;

			line[ 0 ].position = _pos1;
			line[ 1 ].position = _pos2;

			g_pFZN_WindowMgr->Draw( line );
		}

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//String drawing
		//Parameter 1 : First point position
		//Parameter 2 : Location of the string on the screen
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void DrawString( const char* _string, const sf::Vector2f& _position )
		{
			/*sf::Text text( _string, *Data->LoadFont( "defaultFont", DATAPATH( "Display/Fonts/defaultFont.ttf" ) ) );
			text.setPosition( _position );

			g_pFZN_WindowMgr->Draw( text );*/
		}

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//String drawing
		//Parameter 1 : First point position
		//Parameter 2 : Location of the string on the screen
		//Parameter 3 : Characters size
		//Parameter 4 : Color of the string
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void DrawString( const char* _string, const sf::Vector2f& _position, UINT _characterSize, sf::Color _color )
		{
			sf::Text text( _string, *g_pFZN_DataMgr->LoadFont( "defaultFont", DATAPATH( "Display/Fonts/defaultFont.ttf" ) ) );
			text.setPosition( _position );
			text.setCharacterSize( _characterSize );
			text.setFillColor( _color );

			g_pFZN_WindowMgr->Draw( text );
		}

		void DrawString( const char* _string, const sf::Vector2f& _position, UINT _characterSize, sf::Color _color, int iWindowId /*= MainWindow*/ )
		{
			sf::Text text( _string, *g_pFZN_DataMgr->LoadFont( "defaultFont", DATAPATH( "Display/Fonts/defaultFont.ttf" ) ) );
			text.setPosition( _position );
			text.setCharacterSize( _characterSize );
			text.setFillColor( _color );

			g_pFZN_WindowMgr->Draw( text, iWindowId );
		}
	} //namespace Tools
} //namespace fzn
