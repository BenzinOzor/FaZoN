//------------------------------------------------------------------------
//Author : Philippe OFFERMANN
//Date : 21.12.13
//Description : Miscellaneous function gathering
//------------------------------------------------------------------------

#include "FZN/Includes.h"
#include "FZN/Managers/WindowManager.h"
#include "FZN/Managers/DataManager.h"

#include <Externals/ImGui/imgui.h>
#include <Externals/ImGui/imgui_internal.h>
#include <Externals/ImGui/imgui-SFML.h>

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
			return sf::IntRect( (int)_oIntRect.left, (int)_oIntRect.top, (int)_oIntRect.width, (int)_oIntRect.height );
		}

		FZN_EXPORT sf::FloatRect ConvertRectangleShapeToFloatRect( const sf::RectangleShape& _oShape )
		{
			return sf::FloatRect(	_oShape.getPosition().x - _oShape.getOrigin().x,
									_oShape.getPosition().y - _oShape.getOrigin().y,
									_oShape.getSize().x, _oShape.getSize().y );
		}

		sf::ConvexShape ConvertShapePtrToConvexShape( const sf::Shape* _pShape )
		{
			if( _pShape == nullptr )
				return sf::ConvexShape();

			sf::ConvexShape _oRetShape( _pShape->getPointCount() );

			for( size_t iPoint = 0; iPoint < _pShape->getPointCount(); ++iPoint )
				_oRetShape.setPoint( iPoint, _pShape->getPoint( iPoint ) );

			return _oRetShape;
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

			for( int iChar = 1; iChar < (int)sRet.size(); )
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
		bool CollisionCircleCircle( const sf::CircleShape& _c1, const sf::CircleShape& _c2 )
		{
			sf::Vector2f vCircle1Pos = _c1.getPosition() - _c1.getOrigin() + sf::Vector2f( _c1.getRadius(), _c1.getRadius() );
			sf::Vector2f vCircle2Pos = _c2.getPosition() - _c2.getOrigin() + sf::Vector2f( _c2.getRadius(), _c2.getRadius() );
			
			return CollisionCircleCircle( vCircle1Pos, _c1.getRadius(), vCircle2Pos, _c2.getRadius() );
		}

		bool CollisionCircleCircle( const sf::CircleShape& _circle1, const sf::Vector2f& _vCircle2Pos, const float& _fCircle2Radius )
		{
			sf::Vector2f vCircle1Pos = _circle1.getPosition() - _circle1.getOrigin() + sf::Vector2f( _circle1.getRadius(), _circle1.getRadius() );
			
			return CollisionCircleCircle( vCircle1Pos, _circle1.getRadius(), _vCircle2Pos, _fCircle2Radius );
		}

		bool CollisionCircleCircle( const sf::Vector2f& _vCircle1Pos, const float& _fCircle1Radius, const sf::Vector2f& _vCircle2Pos, const float& _fCircle2Radius )
		{
			sf::Vector2f vDist = _vCircle2Pos - _vCircle1Pos;

			return Math::VectorLengthSq( vDist ) <= Math::Square( _fCircle1Radius + _fCircle2Radius );
		}

		bool CollisionCirclePoint( const sf::CircleShape& _circle1, const sf::Vector2f& _vPointPos )
		{
			sf::Vector2f vCircle1Pos = _circle1.getPosition() - _circle1.getOrigin() + sf::Vector2f( _circle1.getRadius(), _circle1.getRadius() );

			sf::Vector2f vDist = _vPointPos - vCircle1Pos;

			return Math::VectorLengthSq( vDist ) <= Math::Square( _circle1.getRadius() );
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

		sf::Vector2f AABBCircleCollisionOverlap( const sf::RectangleShape& _oRectangle, const sf::CircleShape& _oCircle )
		{
			return AABBCircleCollisionOverlap( _oRectangle.getGlobalBounds(), _oCircle );
		}

		sf::Vector2f AABBCircleCollisionOverlap( const sf::FloatRect& _oRectangle, const sf::CircleShape& _oCircle )
		{
			float fRadius = _oCircle.getRadius();

			sf::Vector2f circle( _oCircle.getPosition() - _oCircle.getOrigin() + sf::Vector2f( fRadius, fRadius ) );

			float rectHalfHeight = _oRectangle.height * 0.5f;
			float rectHalfWidth = _oRectangle.width * 0.5f;

			float circleDistX = fabs( circle.x - ( _oRectangle.left + rectHalfWidth ) );
			float circleDistY = fabs( circle.y - ( _oRectangle.top + rectHalfHeight ) );

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

			const sf::Vector2f vCircleTopLeft = _oCircle.getPosition() - _oCircle.getOrigin();

			const float fOverlapRight = std::min( _oRectangle.left + _oRectangle.width, vCircleTopLeft.x + fRadius * 2.f );
			const float fOverlapLeft = std::max( _oRectangle.left, vCircleTopLeft.x );
			const float fOverlapBottom = std::min( _oRectangle.top + _oRectangle.height, vCircleTopLeft.y + fRadius * 2.f );
			const float fOverlapTop = std::max( _oRectangle.top, vCircleTopLeft.y );

			return { fOverlapRight - fOverlapLeft, fOverlapBottom - fOverlapTop };
		}

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Collision test between an AABB (rectangleShape) and a circleShape
		//Parameter 1 : RectangleShape
		//Parameter 2 : CircleShape
		//Return value : The two shapes are in collision (true) or not
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		bool CollisionAABBCircle( const sf::RectangleShape& _rect, const sf::CircleShape& _circle )
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
		bool CollisionAABBCircle( const sf::FloatRect& _rect, const sf::CircleShape& _circle )
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
		bool CollisionAABBAABB( const sf::RectangleShape& _rect1, const sf::RectangleShape& _rect2 )
		{
			sf::Vector2f rect1 = _rect1.getPosition() - _rect1.getOrigin();
			sf::Vector2f rect1Size = _rect1.getSize();
			sf::Vector2f rect2 = _rect2.getPosition() - _rect2.getOrigin();
			sf::Vector2f rect2Size = _rect2.getSize();

			return ( rect1.x < rect2.x + rect2Size.x && rect1.x + rect1Size.x > rect2.x && rect1.y < rect2.y + rect2Size.y && rect1.y + rect1Size.y > rect2.y );
		}

		bool CollisionAABBAABB( const sf::RectangleShape& _rect1, const sf::RectangleShape& _rect2, const sf::Vector2f& _vBox2Direction )
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
		bool CollisionAABBAABB( const sf::RectangleShape& _rect1, const sf::FloatRect& _rect2 )
		{
			sf::Vector2f rect1 = _rect1.getPosition();
			sf::Vector2f rect1Size = _rect1.getSize();

			return ( rect1.x < _rect2.left + _rect2.width && rect1.x + rect1Size.x > _rect2.left && rect1.y < _rect2.top + _rect2.height && rect1.y + rect1Size.y > _rect2.top );
		}
		bool CollisionAABBAABB( const sf::FloatRect& _rect1, const sf::FloatRect& _rect2 )
		{
			return ( _rect1.left < _rect2.left + _rect2.width && _rect1.left + _rect1.width > _rect2.left && _rect1.top < _rect2.top + _rect2.height && _rect1.top + _rect1.height > _rect2.top );
		}

		bool LineIntersectsCircle( const sf::Vector2f& _vLineP1, const sf::Vector2f& _vLineP2, const sf::CircleShape& _oCircle )
		{
			float fRadius = _oCircle.getRadius();

			const sf::Vector2f vCirclePos( _oCircle.getPosition() - _oCircle.getOrigin() + sf::Vector2f( fRadius, fRadius ) );

			if( Math::VectorLengthSq( vCirclePos - _vLineP1 ) <= Math::Square( fRadius ) )
				return true;
			
			if( Math::VectorLengthSq( vCirclePos - _vLineP2 ) <= Math::Square( fRadius ) )
				return true;

			const sf::Vector2f vLine = _vLineP2 - _vLineP1;
			const sf::Vector2f vLineToCircle = Math::VectorNormalization( vCirclePos - _vLineP1 );

			const float fDot = Math::VectorDot( vLine, vLineToCircle );

			if( fDot < 0.f )
				return false;

			sf::Vector2f vProjection = _vLineP1 + vLineToCircle * fDot;

			if( Math::VectorLengthSq( vProjection - vCirclePos ) <= Math::Square( fRadius ) )
				return true;

			return false;
		}

		float Cross2D( const sf::Vector2f& _vA, const sf::Vector2f& _vB, const sf::Vector2f& _vPoint )
		{
			return ( _vB.x - _vA.x ) * ( _vPoint.y - _vA.y ) - ( _vB.y - _vA.y ) * ( _vPoint.x - _vA.x );
		}

		bool CollisionOBBPoint( const sf::Shape& _daOBB, const sf::Vector2f& _vPoint )
		{
			if( _daOBB.getPointCount() < 3 )
				return false;

			const sf::Transform& rTransform = _daOBB.getTransform();

			float fLastCross = Cross2D( rTransform.transformPoint( _daOBB.getPoint( 0 ) ), rTransform.transformPoint( _daOBB.getPoint( 1 ) ), _vPoint );
			int iOBBNextPoint = 0;

			for( int iOBBPoint = 1; iOBBPoint < _daOBB.getPointCount(); ++iOBBPoint )
			{
				iOBBNextPoint = ( iOBBPoint + 1 ) % _daOBB.getPointCount();
				float fCross = Cross2D( rTransform.transformPoint( _daOBB.getPoint( iOBBPoint ) ), rTransform.transformPoint( _daOBB.getPoint( iOBBNextPoint ) ), _vPoint );

				// If the multiplication results in a negative number, the two values have an opposite sign and so, the point is out of the bounding box.
				if( fCross * fLastCross < 0.f )
					return false;

				if( fCross != 0.f )
					fLastCross = fCross;
			}

			return true;
		}

		bool CollisionOBBPoint( const std::vector< sf::Vector2f >& _daOBB, const sf::Vector2f& _vPoint )
		{
			if( _daOBB.size() < 3 )
				return false;

			float fLastCross = Cross2D( _daOBB[ 0 ], _daOBB[ 1 ], _vPoint );
			int iOBBNextPoint = 0;

			for( int iOBBPoint = 1; iOBBPoint < _daOBB.size(); ++iOBBPoint )
			{
				iOBBNextPoint = ( iOBBPoint + 1 ) % _daOBB.size();
				float fCross = Cross2D( _daOBB[ iOBBPoint ], _daOBB[ iOBBNextPoint ], _vPoint );

				// If the multiplication results in a negative number, the two values have an opposite sign and so, the point is out of the bounding box.
				if( fCross * fLastCross < 0.f )
					return false;

				fLastCross = fCross;
			}

			return true;
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
				<< "'" << std::setfill( '0' ) << std::setw( 2 ) << TimerAsSeconds( _fTimer )
				<< "." << std::setfill( '0' ) << std::setw( 2 ) << TimerAsHundredth( _fTimer );
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
		/*void DrawString( const char* _string, const sf::Vector2f& _position, unsigned int _characterSize, sf::Color _color )
		{
			sf::Text text( _string, *g_pFZN_DataMgr->LoadFont( "defaultFont", DATAPATH( "Display/Fonts/defaultFont.ttf" ) ) );
			text.setPosition( _position );
			text.setCharacterSize( _characterSize );
			text.setFillColor( _color );

			g_pFZN_WindowMgr->Draw( text );
		}*/

		void DrawString( const char* _string, const sf::Vector2f& _position, unsigned int _characterSize, sf::Color _color, int iWindowId /*= MainWindow*/ )
		{
			sf::Text text( _string, *g_pFZN_DataMgr->LoadFont( "defaultFont", DATAPATH( "Display/Fonts/defaultFont.ttf" ) ) );
			text.setPosition( _position );
			text.setCharacterSize( _characterSize );
			text.setFillColor( _color );

			g_pFZN_WindowMgr->Draw( text, iWindowId );
		}

		void DrawImGuiTextWithOptions( const ImGuiFormatOptions& _rFormatOptions, const std::string& _sTagContent, bool& _bSameLine, int& _iChar, int _iCharOffset, int _iOptionsEndIndex )
		{
			std::string sOptions = _sTagContent.substr( 0, _iOptionsEndIndex );
			std::string sText = _sTagContent.substr( _iOptionsEndIndex + 1 );

			_bSameLine = true;

			bool bBold = false;
			ImVec4 oColor( -1.f, -1.f, -1.f, -1.f );

			int iSeparatorIndex = std::string::npos;
			size_t iOptionIndex = std::string::npos;
			std::string sOption = "";

			do
			{
				iSeparatorIndex = sOptions.find_first_of( _rFormatOptions.m_sTagOptionsSeparator );

				if( iSeparatorIndex == std::string::npos )
				{
					sOption = sOptions;
					sOptions.clear();
				}
				else
				{
					sOption = sOptions.substr( 0, iSeparatorIndex );
					sOptions = sOptions.substr( iSeparatorIndex + 1 );
				}

				if( sOption.empty() == false )
				{
					if( sOption == _rFormatOptions.m_sOption_Bold )
						bBold = true;
					else if( sOption.find( _rFormatOptions.m_sOption_Color ) != std::string::npos )
					{
						const size_t iOptionBeginOffset = _rFormatOptions.m_sOption_Color.size() + _rFormatOptions.m_sTagOptionParameterBegin.size();
						std::string sRGB = sOption.substr( iOptionBeginOffset, sOption.size() - _rFormatOptions.m_sTagOptionParameterEnd.size() );
						
						oColor = GetImColorFromString( sRGB, &_rFormatOptions );
					}
				}

			} while( sOptions.empty() == false );


			if( _bSameLine )
				ImGui::SameLine();


			if( bBold && _rFormatOptions.m_pFontBold != nullptr )
				ImGui::PushFont( _rFormatOptions.m_pFontBold );

			if( IsColorValid( oColor ) )
				ImGui::PushStyleColor( ImGuiCol_::ImGuiCol_Text, oColor );
				
			ImGui::Text( sText.c_str() );

			if( IsColorValid( oColor ) )
				ImGui::PopStyleColor( 1 );

			if( bBold && _rFormatOptions.m_pFontBold != nullptr )
				ImGui::PopFont();

			_iChar += _iCharOffset;
		}

		void FormatImGuiText( const std::string& _sText, const ImGuiFormatOptions* _pFormatOptions /*= nullptr*/ )
		{
			if( _sText.empty() )
				return;

			auto CheckTag = []( const std::string& _sTag, const std::string& _sText, int _iCurrentChar )
			{
				int iTagChar = 0;
				int iChar = _iCurrentChar;

				while( iTagChar < _sTag.size() && iChar < _sText.size() )
				{
					if( _sText[ iChar ] != _sTag[ iTagChar ] )
						return false;

					++iTagChar;
					++iChar;
				};

				// If iTagChar reached the end of the tag, it means the given string matches the tag.
				return iTagChar == _sTag.size();
			};

			auto DrawImGuiText = [] ( const std::string& _sText, int _iChar, int _iSubStringBegin, bool _bSameLine, std::string& _sSubString )
			{
				if( _iChar > 0 )
				{
					_sSubString = _sText.substr( _iSubStringBegin, _iChar - _iSubStringBegin );

					if( _bSameLine )
						ImGui::SameLine();

					ImGui::Text( _sSubString.c_str() );
				}
			};

			float fSpacing = ImGui::GetStyle().ItemSpacing.x;
			ImGui::GetStyle().ItemSpacing.x = 0.f;

			std::string sText = _sText;
			std::string sSubString( "" );
			int iSubstringBegin = 0;
			bool bSameLine = false;

			const sf::Texture* pTexture = nullptr;
			const ImGuiFormatOptions& rFormatOptions = _pFormatOptions != nullptr ? *_pFormatOptions : WindowManager::s_ImGuiFormatOptions;

			int iChar = 0;
			while( iChar < (int)sText.size() )
			{
				if( CheckTag( rFormatOptions.m_sBeginTag, sText, iChar ) )
				{
					int iGlyphEnd = sText.find( rFormatOptions.m_sEndTag, iChar + rFormatOptions.m_sBeginTag.size() );

					if( iGlyphEnd < 0 )
					{
						++iChar;
						continue;
					}

					std::string sTagContent = sText.substr( iChar + rFormatOptions.m_sBeginTag.size(), iGlyphEnd - ( iChar + rFormatOptions.m_sEndTag.size() ) );

					int iTagOption = sTagContent.find_first_of( rFormatOptions.m_sTagOptionsEnd );
					int iCharOffset = sTagContent.length() + rFormatOptions.m_sBeginTag.size() + rFormatOptions.m_sEndTag.size();

					DrawImGuiText( sText, iChar, iSubstringBegin, bSameLine, sSubString );

					if( iTagOption != std::string::npos )
					{
						if( iChar == 0 )
							ImGui::Text("");

						DrawImGuiTextWithOptions( rFormatOptions, sTagContent, bSameLine, iChar, iCharOffset, iTagOption );
					}
					else
					{
						pTexture = g_pFZN_DataMgr->GetTexture( sTagContent );

						if( pTexture != nullptr )
						{
							ImGui::SameLine();
							ImGui::Image( *pTexture );

							iChar += iCharOffset;

							if( CheckTag( "\\n", sText, iChar ) )
							{
								bSameLine = false;
								iChar += 2;
								continue;
							}
							else
								bSameLine = true;
						}
						else
						{
							sText.erase( iChar, iCharOffset );

							if( sTagContent.empty() == false )
								sText.insert( iChar, sTagContent );

							iChar += ( iCharOffset - ( rFormatOptions.m_sBeginTag.size() + rFormatOptions.m_sEndTag.size() ) );
							continue;
						}
					}

					iSubstringBegin = iChar;
				}
				else if( CheckTag( "\\n", sText, iChar ) )
				{
					DrawImGuiText( sText, iChar, iSubstringBegin, bSameLine, sSubString );

					bSameLine = false;

					iChar += 2;
					iSubstringBegin = iChar;
					continue;
				}
				else
					++iChar;
			};

			DrawImGuiText( sText, iChar, iSubstringBegin, bSameLine, sSubString );

			ImGui::GetStyle().ItemSpacing.x = fSpacing;
		}

		void BoldImGuiText( const std::string& _sText )
		{
			CustomFontImGuiText( _sText, WindowManager::s_ImGuiFormatOptions.m_pFontBold );
		}

		void CustomFontImGuiText( const std::string& _sText, ImFont* _pFont )
		{
			if( _pFont == nullptr )
			{
				ImGui::Text( _sText.c_str() );
				return;
			}

			ImGui::PushFont( _pFont );

			ImGui::Text( _sText.c_str() );

			ImGui::PopFont();
		}

		ImVec4 GetImColorFromString( const std::string& _sColor, const ImGuiFormatOptions* _pFormatOptions /*= nullptr*/ )
		{
			auto GetColorFromString = []( const std::string& _sColor )
			{
				if( _sColor.empty() )
					return -1.f;

				float fColor = atof( _sColor.c_str() );

				if( fColor > 1.f )
					fColor /= 255.f;

				return fColor;
			};

			auto GetNextColor = [&]( std::string& _sRGB, const ImGuiFormatOptions& _rFormatOptions, float _fDefaultValue )
			{
				if( _sRGB.empty() )
					return _fDefaultValue;

				size_t iSeparatorIndex = _sRGB.find_first_of( _rFormatOptions.m_sTagOptionParameterSeparator );

				if( iSeparatorIndex != std::string::npos )
				{
					std::string sColor = _sRGB.substr( 0, iSeparatorIndex );
					_sRGB = _sRGB.substr( iSeparatorIndex + 1 );

					return GetColorFromString( sColor );
				}
				
				// Last parameter of the color.
				std::string sColor = _sRGB;
				iSeparatorIndex = _sRGB.find_first_of( _rFormatOptions.m_sTagOptionParameterEnd );

				if( iSeparatorIndex != std::string::npos )
					std::string sColor = _sRGB.substr( 0, iSeparatorIndex );

				_sRGB.clear();
				return GetColorFromString( sColor );
			};

			ImVec4 oColor( -1.f, -1.f, -1.f, -1.f );

			if( _sColor.empty() )
				return oColor;

			std::string sRGB{ _sColor };
			std::string sColor{ "" };
			const ImGuiFormatOptions& rFormatOptions = _pFormatOptions != nullptr ? *_pFormatOptions : WindowManager::s_ImGuiFormatOptions;

			oColor.x = GetNextColor( sRGB, rFormatOptions, -1.f );
			oColor.y = GetNextColor( sRGB, rFormatOptions, -1.f );
			oColor.z = GetNextColor( sRGB, rFormatOptions, -1.f );
			oColor.w = GetNextColor( sRGB, rFormatOptions, 1.f );

			return oColor;
		}

		std::string GetColorTag( const ImVec4& _rColor, const ImGuiFormatOptions* _pFormatOptions /*= nullptr */ )
		{
			if( fzn::Tools::IsColorValid( _rColor ) == false )
				return "";

			const ImGuiFormatOptions& rFormatOptions = _pFormatOptions != nullptr ? *_pFormatOptions : WindowManager::s_ImGuiFormatOptions;

			std::string sColorTag = rFormatOptions.m_sOption_Color;

			sColorTag += rFormatOptions.m_sTagOptionParameterBegin;
			sColorTag += fzn::Tools::Sprintf( "%.2f%s%.2f%s%.2f", _rColor.x, rFormatOptions.m_sTagOptionParameterSeparator.c_str(), _rColor.y, rFormatOptions.m_sTagOptionParameterSeparator.c_str(), _rColor.z );
			sColorTag += rFormatOptions.m_sTagOptionParameterEnd;

			return sColorTag;
		}

		bool IsColorValid( const ImVec4& _rColor )
		{
			if( _rColor.x < 0.f || _rColor.y < 0.f || _rColor.z < 0.f || _rColor.w < 0.f )
				return false;

			return true;
		}

		size_t FindWholeWord( const std::string& _sText, const std::string& _sWord, size_t _Off )
		{
			if( _sText.empty() || _sWord.empty() )
				return std::string::npos;

			auto IsIgnoredCharacter = []( const char& _rChar )
			{
				if( _rChar == '.' || _rChar == ',' || _rChar == '-' )
					return true;

				return false;
			};

			size_t iWord = _sText.find( _sWord, _Off );
			size_t iOffsetedWord = 0;

			bool bBeforeWordValid = false;
			bool bAfterWordValid = false;

			while( iWord != std::string::npos )
			{
				if( iWord == 0 || _sText[ iWord - 1 ] == ' ' || IsIgnoredCharacter( _sText[ iWord - 1 ] ) )
					bBeforeWordValid = true;

				iOffsetedWord = iWord + _sWord.size();
				if( iOffsetedWord >= _sText.size() || _sText[ iOffsetedWord ] == ' ' || IsIgnoredCharacter( _sText[ iOffsetedWord ] ) )
					bAfterWordValid = true;

				if( bBeforeWordValid && bAfterWordValid )
					return iWord;

				bBeforeWordValid = false;
				bAfterWordValid = false;

				iWord += _sWord.size();
				iWord = _sText.find( _sWord, iWord );
			}

			return std::string::npos;
		}

	} //namespace Tools
} //namespace fzn
