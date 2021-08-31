//------------------------------------------------------------------------
//Author : Philippe OFFERMANN
//Date : 20.12.13
//Description : Data manager (textures, animations, sounds, musics and fonts)
//------------------------------------------------------------------------

#ifndef _DATAMANAGER_H_
#define _DATAMANAGER_H_

#include <string>
#include <vector>
#include <map>

#include <SFML/System/NonCopyable.hpp>

#include "FZN/Defines.h"
#include "FZN/Managers/FazonCore.h"

namespace tinyxml2 { class XMLElement; }

namespace sf
{
	class Music;
	class SoundBuffer;
	class Texture;
	class Font;
	class Shader;
}

namespace fzn
{
	class Animation;
	class Anm2;
	class Music;
	class Sound;
	class BitmapFont;

	//=========================================================
	//========================Resource===========================
	//=========================================================

	struct Resource
	{
		Resource()
		{
			Reset();
		}

		void Reset()
		{
			name = "";
			path = "";
			type = "";
		}

		std::string name;			//Name of the resource
		std::string path;			//Path to the resource
		std::string type;			//Type of resource
	};

	//=========================================================
	//=====================ResourceGroup==========================
	//=========================================================

	struct ResourceGroup
	{
		std::string				m_sName = "";		//Name of the resource group
		std::vector< Resource > m_oResources;		//Container of all the paths to the resources
		bool					m_bLoaded = false;	//Indicates if the group has been loaded (true) or not
	};

	//=========================================================
	//======================DataManager==========================
	//=========================================================



	class FZN_EXPORT DataManager : public sf::NonCopyable
	{
	public:
		enum ResourceType
		{
			eTexture,
			eMusic,
			eSound,
			eFont,
			eBitmapFont,
			eAnimation,
			eAnm2,
			eBitmapGlyph,
			eNbResourceTypes,
		};

		/////////////////CONSTRUCTOR / DESTRUCTOR/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Default constructor
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		DataManager();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Destructor
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		~DataManager();


		/////////////////MAPS MANAGEMENT/////////////////
		/////////////////TEXTURES MAP/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Load of a texture in the map
		//Parameter 1 : Texture name
		//Parameter 2 : Path to the texture
		//Return value : Loaded (or allready existing) texture
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		sf::Texture* LoadTexture(const std::string& _name, const std::string& _path);
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on a texture in the map
		//Parameter : Texture name
		//Return value : Concerned texture
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		sf::Texture* GetTexture( const std::string& _name, bool _bHandleError = true );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Texture unloading from the map
		//Parameter : Texture name
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void UnloadTexture(const std::string& _name);

		/////////////////ANIMATIONS MAP/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Load of an animation in the map
		//Parameter 1 : Animation name
		//Parameter 2 : Path to the animation
		//Return value : Loaded (or allready existing) animation
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		Animation* LoadAnimation(const std::string& _name, const std::string& _path);
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on an animation in the map
		//Parameter : Animation name
		//Return value : Concerned animation
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		Animation* GetAnimation( const std::string& _name, bool _bHandleError = true );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Animation unloading from the map
		//Parameter : Animation name
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void UnloadAnimation(const std::string& _name);

		void LoadAnm2s( const std::string& _sAnimatedObject, const std::string& _sFile );
		Anm2* GetAnm2( const std::string& _sAnimatedObject, const std::string& _sName, bool _bHandleError = true );
		void UnloadAnm2( const std::string& _sAnimatedObject, const std::string& _sName = "" );

		/////////////////MUSICS MAP/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Load of a music in the map
		//Parameter 1 : Music name
		//Parameter 2 : Path to the music
		//Return value : Loaded (or allready existing) music
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		Music* LoadMusic(const std::string& _name, const std::string& _path);
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on a music in the map
		//Parameter : Music name
		//Return value : Concerned music
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		Music* GetMusic( const std::string& _name, bool _bHandleError = true );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Music unloading from the map
		//Parameter : Music name
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void UnloadMusic(const std::string& _name);

		/////////////////SOUNDS MAP/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Load of a sound in the map
		//Parameter 1 : Sound name
		//Parameter 2 : Path to the sound
		//Return value : Loaded (or allready existing) sound
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		Sound* LoadSound(const std::string& _name, const std::string& _path);
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on a sound in the map
		//Parameter : Sound name
		//Return value : Concerned sound
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		Sound* GetSound( const std::string& _name, bool _bHandleError = true );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Sound unloading from the map
		//Parameter : Sound name
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void UnloadSound(const std::string& _name);

		sf::Music* LoadSfMusic(const std::string& _name, const std::string& _path);
		sf::Music* GetSfMusic( const std::string& _name, bool _bHandleError = true );
		void UnloadSfMusic(const std::string& _name);

		sf::SoundBuffer* LoadSoundBuffer(const std::string& _name, const std::string& _path);
		sf::SoundBuffer* GetSoundBuffer( const std::string& _name, bool _bHandleError = true );
		void UnloadSoundBuffer(const std::string& _name);

		/////////////////FONTS MAP/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Load of a font in the map
		//Parameter 1 : Font name
		//Parameter 2 : Path to the font
		//Return value : Loaded (or allready existing) font
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		sf::Font* LoadFont(const std::string& _name, const std::string& _path);
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on a font in the map
		//Parameter : Font name
		//Return value : Concerned font
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		sf::Font* GetFont( const std::string& _name, bool _bHandleError = true );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Font unloading from the map
		//Parameter : Font name
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void UnloadFont(const std::string& _name);

		BitmapFont* LoadBitmapFont(const std::string& _name, const std::string& _path);
		BitmapFont* GetBitmapFont( const std::string& _name, bool _bHandleError = true );
		void UnloadBitmapFont(const std::string& _name);

		CustomBitmapGlyph* GetBitmapGlyph( const std::string& _name, bool _bHandleError = true );

		sf::Shader* LoadShader( const std::string& _sName, const std::string& _sPath );
		sf::Shader* GetShader( const std::string& _sName, bool _bHandleError = true );
		void		UnloadShader( const std::string& _sName );


		/////////////////RESOURCE FILE/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Loads the resource file
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void LoadResourceFile( const char* _resourceFile = DATAPATH( "XMLFiles/Resources" ), bool _bLocalResourceFile = true );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Loads a group of resources
		//Parameter : Group to load
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void LoadResourceGroup(const char* _group);
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Unloads a group of resources
		//Parameter : Group to unload
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void UnloadResourceGroup(const char* _group);

		void SetSmoothTextures( bool _bSmooth );
		bool GetSmoothTextures() const;

		bool ResourceExists( const ResourceType& _eType, const std::string& _sResourceName, const std::string& _sAdditionalName = "" );

	private:
		/////////////////TYPEDEFS/////////////////

		typedef std::map< std::string, Animation* >			MapAnimations;		//Map containing Animations ordered by strings
		typedef std::map< std::string, Anm2* >				MapAnm2s;			//Map containing Anm2 animations ordered by strings
		typedef std::map< std::string, MapAnm2s >			MapAnimatedObjects;
		typedef std::map< std::string, sf::Texture* >		MapTextures;		//Map containing Textures ordered by strings
		typedef std::map< std::string, Music* >				MapMusics;			//Map containing Musics ordered by strings
		typedef std::map< std::string, Sound* >				MapSound;			//Map containing SoundBuffers ordered by strings
		typedef std::map< std::string, sf::Music* >			MapSfMusics;
		typedef std::map< std::string, sf::SoundBuffer* >	MapSoundBuffers;
		typedef std::map< std::string, sf::Font* >			MapFonts;			//Map containing Fonts ordered by strings
		typedef std::map< std::string, BitmapFont* >		MapBitmapFonts;
		typedef std::map< std::string, CustomBitmapGlyph >	MapBitmapGylphs;
		typedef std::map< std::string, sf::Shader >			MapShaders;			//Map containing all the resources groups
		typedef std::map< std::string, ResourceGroup* >		MapResourceGroups;	//Map containing all the resources groups


		/////////////////OTHER FUNCTIONS/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Loads a resource in the map from the xml resource file
		//Parameter 1 : Node containing the informations on the resource to load
		//Parameter 2 : Path to the resource
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void LoadResourceFromXML(tinyxml2::XMLElement* _node, const std::string& _szPath);
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Loads a resource in the map from the xml resource file
		//Parameter : Resource to load
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void LoadResourceFromXML( const Resource& _oResource );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Unloads a resource in the map from the xml resource file
		//Parameter : Resource to unload
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void UnloadResource( const Resource& _oResource );

		void _LoadBitmapGlyphFile( const std::string& _sFile );

		void _LoadShaders();

		/////////////////MEMBER VARIABLES/////////////////

		/////////////////MAPS/////////////////

		MapTextures			m_mapTextures;
		MapMusics			m_mapMusics;
		MapSound			m_mapSound;
		MapSfMusics			m_mapSfMusics;
		MapSoundBuffers		m_mapSoundBuffers;
		MapFonts			m_mapFonts;
		MapBitmapFonts		m_mapBitmapFonts;
		MapAnimations		m_mapAnimations;
		MapAnimatedObjects	m_mapAnimatedObjets;
		MapBitmapGylphs		m_mapBitmapGlyphs;
		MapShaders			m_mapShaders;
		MapResourceGroups	m_mapResourceGroups;

		/////////////////RESOURCE FILE/////////////////
		bool m_bResourceFileExists;			//Does the resource file exists
		bool m_bResourceFileLoaded;			//Has the resource file been loaded

		bool m_bSmoothTextures;
	};
} //namspace fzn

extern FZN_EXPORT fzn::DataManager* g_pFZN_DataMgr;

#endif //_DATAMANAGER_H_
