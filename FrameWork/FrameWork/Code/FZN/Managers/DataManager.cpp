//------------------------------------------------------------------------
//Author : Philippe OFFERMANN
//Date : 20.12.13
//Description : Data manager (textures, animations, sounds, musics and fonts)
//------------------------------------------------------------------------

#include <filesystem>

#include "FZN/Includes.h"
#include "FZN/Display/Animation.h"
#include "FZN/Display/Anm2.h"
#include "FZN/Audio/Music.h"
#include "FZN/Audio/Sound.h"
#include "FZN/Display/BitmapFont.h"
#include <tinyXML2/tinyxml2.h>
#include "FZN/Managers/DataManager.h"
#include "FZN/Tools/Shaders.h"


FZN_EXPORT fzn::DataManager* g_pFZN_DataMgr = nullptr;

namespace fzn
{
	/////////////////CONSTRUCTOR / DESTRUCTOR/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Default constructor
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	DataManager::DataManager()
	{
		m_bResourceFileExists = false;
		m_bResourceFileLoaded = false;

		m_bSmoothTextures = true;

		g_pFZN_DataMgr = this;

		_LoadShaders();
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Destructor
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	DataManager::~DataManager()
	{
		MapTextures::iterator texIt = m_mapTextures.begin();
		MapTextures::iterator texItEnd = m_mapTextures.end();
		for( ; texIt != texItEnd ; texIt++ )
		{
			delete texIt->second;
			texIt->second = nullptr;
		}

		MapMusics::iterator musIt = m_mapMusics.begin();
		MapMusics::iterator musItEnd = m_mapMusics.end();
		for( ; musIt != musItEnd ; musIt++ )
		{
			delete musIt->second;
			musIt->second = nullptr;
		}

		MapSound::iterator soundIt = m_mapSound.begin();
		MapSound::iterator soundItEnd = m_mapSound.end();
		for( ; soundIt != soundItEnd ; soundIt++ )
		{
			delete soundIt->second;
			soundIt->second = nullptr;
		}

		MapSfMusics::iterator sfMusIt = m_mapSfMusics.begin();
		MapSfMusics::iterator sfMusItEnd = m_mapSfMusics.end();
		for( ; sfMusIt != sfMusItEnd ; sfMusIt++ )
		{
			delete sfMusIt->second;
			sfMusIt->second = nullptr;
		}

		MapSoundBuffers::iterator soundBuffersIt = m_mapSoundBuffers.begin();
		MapSoundBuffers::iterator soundBuffersItEnd = m_mapSoundBuffers.end();
		for( ; soundBuffersIt != soundBuffersItEnd ; soundBuffersIt++ )
		{
			delete soundBuffersIt->second;
			soundBuffersIt->second = nullptr;
		}

		MapFonts::iterator fontIt = m_mapFonts.begin();
		MapFonts::iterator fontItEnd = m_mapFonts.end();
		for( ; fontIt != fontItEnd ; fontIt++ )
		{
			delete fontIt->second;
			fontIt->second = nullptr;
		}

		MapBitmapFonts::iterator bitmapFontIt = m_mapBitmapFonts.begin();
		MapBitmapFonts::iterator bitmapFontItEnd = m_mapBitmapFonts.end();
		for( ; fontIt != fontItEnd ; fontIt++ )
		{
			delete fontIt->second;
			fontIt->second = nullptr;
		}

		MapAnimations::iterator anmIt = m_mapAnimations.begin();
		MapAnimations::iterator anmItEnd = m_mapAnimations.end();
		for( ; anmIt != anmItEnd ; anmIt++ )
		{
			delete anmIt->second;
			anmIt->second = nullptr;
		}
		
		MapAnimatedObjects::iterator oAnmObjectsIt = m_mapAnimatedObjets.begin();
		MapAnimatedObjects::iterator oAnmObjectsItEnd = m_mapAnimatedObjets.end();

		for( ; oAnmObjectsIt != oAnmObjectsItEnd; ++oAnmObjectsIt )
		{
			MapAnm2s::iterator anm2It = oAnmObjectsIt->second.begin();
			MapAnm2s::iterator anm2ItEnd = oAnmObjectsIt->second.end();

			for( ; anm2It != anm2ItEnd; anm2It++ )
			{
				delete anm2It->second;
				anm2It->second = nullptr;
			}

			oAnmObjectsIt->second.clear();
		}

		MapResourceGroups::iterator resIt = m_mapResourceGroups.begin();
		MapResourceGroups::iterator resItEnd = m_mapResourceGroups.end();
		for( ; resIt != resItEnd ; resIt++ )
		{
			delete resIt->second;
			resIt->second = nullptr;
		}

		m_mapTextures.clear();
		//m_mapMusics.clear();
		m_mapSound.clear();
		//m_mapSfMusics.clear();
		//m_mapSoundBuffers.clear();
		m_mapFonts.clear();
		m_mapAnimations.clear();
		m_mapAnimatedObjets.clear();
		m_mapResourceGroups.clear();

		g_pFZN_DataMgr = nullptr;
	}


	//=========================================================
	//==========================PUBLIC==========================
	//=========================================================

	/////////////////MAPS MANAGEMENT/////////////////
	/////////////////TEXTURES MAP/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Load of a texture in the map
	//Parameter 1 : Texture name
	//Parameter 2 : Path to the texture
	//Return value : Loaded (or allready existing) texture
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	sf::Texture* DataManager::LoadTexture( const std::string& _szName, const std::string& _path )
	{
		MapTextures::iterator it = m_mapTextures.find( _szName );

		if( it == m_mapTextures.end() )
		{
			sf::Texture* tmpTexture = new sf::Texture;
			tmpTexture->loadFromFile( _path );
			tmpTexture->setSmooth( m_bSmoothTextures );

			FZN_LOG( "Loading texture \"%s\" at \"%s\".", _szName.c_str(), _path.c_str() );

			return m_mapTextures[_szName] = tmpTexture;
		}

		return it->second;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on a texture in the map
	//Parameter : Texture name
	//Return value : Concerned texture
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	sf::Texture* DataManager::GetTexture( const std::string& _szName, bool _bHandleError /*= true*/ )
	{
		MapTextures::iterator it = m_mapTextures.find( _szName );

		if( it != m_mapTextures.end() )
			return it->second;

		if( _bHandleError )
			FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "\"%s\" not found.", _szName.c_str() );
		return nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Texture unloading from the map
	//Parameter : Texture name
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void DataManager::UnloadTexture( const std::string& _szName )
	{
		MapTextures::iterator it = m_mapTextures.find( _szName );

		if( it != m_mapTextures.end() )
		{
			delete it->second;
			it->second = nullptr;
			m_mapTextures.erase( it );

			FZN_LOG( "Unloading texture \"%s\".", _szName.c_str() );
		}
		else
			FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "\"%s\" not found.", _szName.c_str() );
	}

	/////////////////ANIMATIONS MAP/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Load of an animation in the map
	//Parameter 1 : Animation name
	//Parameter 2 : Path to the animation
	//Return value : Loaded (or allready existing) animation
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	Animation* DataManager::LoadAnimation( const std::string& _szName, const std::string& _path )
	{
		MapAnimations::iterator it = m_mapAnimations.find( _szName );
		if( it == m_mapAnimations.end() )
		{
			FZN_LOG( "Loading animation \"%s\" at \"%s\".", _szName.c_str(), _path.c_str() );

			return m_mapAnimations[_szName] = new Animation( _szName, _path );
		}

		return it->second;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on an animation in the map
	//Parameter : Animation name
	//Return value : Concerned animation
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	Animation* DataManager::GetAnimation( const std::string& _szName, bool _bHandleError /*= true*/ )
	{
		MapAnimations::iterator it = m_mapAnimations.find( _szName );

		if( it != m_mapAnimations.end() )
			return it->second;

		if( _bHandleError )
			FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "\"%s\" not found.", _szName.c_str() );
		return nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Animation unloading from the map
	//Parameter : Animation name
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void DataManager::UnloadAnimation( const std::string& _szName )
	{
		MapAnimations::iterator it = m_mapAnimations.find( _szName );

		if( it != m_mapAnimations.end() )
		{
			delete it->second;
			it->second = nullptr;
			m_mapAnimations.erase( it );

			FZN_LOG( "Unloading animation \"%s\".", _szName.c_str() );
		}
		else
			FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "\"%s\" not found.", _szName.c_str() );
	}

	void DataManager::LoadAnm2s( const std::string& _sAnimatedObject, const std::string& _sFile )
	{
		FZN_LOG( "Loading Anm2 file \"%s\"", _sFile.c_str() );

		tinyxml2::XMLDocument oAnmFile;

		if( _sFile.size() == 0 || oAnmFile.LoadFile( _sFile.c_str() ) )
		{
			FZN_COLOR_LOG( DBG_MSG_COL_RED, "Failure : %s", oAnmFile.ErrorName() );
			return;
		}

		tinyxml2::XMLElement* pRoot = oAnmFile.FirstChildElement( "AnimatedActor" );

		if( pRoot == nullptr )
			return;

		tinyxml2::XMLElement* pContent = pRoot->FirstChildElement( "Content" );

		if( pContent == nullptr )
			 return;

		tinyxml2::XMLElement* pAnimations = pRoot->FirstChildElement( "Animations" );

		if( pAnimations == nullptr )
			return;

		tinyxml2::XMLElement* pCurrentAnimation = pAnimations->FirstChildElement( "Animation" );
		std::string szAnimName = "";

		while( pCurrentAnimation != nullptr )
		{
			szAnimName = Tools::XMLStringAttribute( pCurrentAnimation, "Name" );
			m_mapAnimatedObjets[ _sAnimatedObject ][ szAnimName ] = new Anm2( pContent, pCurrentAnimation, _sFile );
			szAnimName.clear();

			pCurrentAnimation = pCurrentAnimation->NextSiblingElement();
		}
	}

	Anm2* DataManager::GetAnm2( const std::string& _sAnimatedObject, const std::string& _sName, bool _bHandleError /*= true*/ )
	{
		MapAnimatedObjects::iterator itAnimObjects = m_mapAnimatedObjets.find( _sAnimatedObject );

		if( itAnimObjects != m_mapAnimatedObjets.end() )
		{
			MapAnm2s::iterator it = itAnimObjects->second.find( _sName );

			if( it != itAnimObjects->second.end() )
				return it->second;

			if( _bHandleError )
				FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "\"%s\" in \"%s\" not found.", _sName.c_str(), _sAnimatedObject.c_str() );
			return nullptr;
		}
		
		if( _bHandleError )
			FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "\"%s\" not found.", _sAnimatedObject.c_str() );
		return nullptr;
	}

	void DataManager::UnloadAnm2( const std::string& _sAnimatedObject, const std::string& _sName /*= ""*/ )
	{
		MapAnimatedObjects::iterator itAnimObjects = m_mapAnimatedObjets.find( _sAnimatedObject );

		if( itAnimObjects != m_mapAnimatedObjets.end() )
		{
			if( _sName.empty() == false )
			{
				MapAnm2s::iterator it = itAnimObjects->second.find( _sName );

				if( it != itAnimObjects->second.end() )
				{
					delete it->second;
					it->second = nullptr;
					itAnimObjects->second.erase( it );

					FZN_LOG( "Unloading anm2 \"%s\".", _sName.c_str() );
				}
				else
					FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "\"%s\" in \"%s\" not found.", _sName.c_str(), _sAnimatedObject.c_str() );
			}
			else
			{
				MapAnm2s::iterator it = itAnimObjects->second.begin();
				for( ; it != itAnimObjects->second.end(); ++it )
				{
					FZN_LOG( "Unloading anm2 \"%s\".", it->second->GetName().c_str() );
					delete it->second;
					it->second = nullptr;
				}

				itAnimObjects->second.clear();
				m_mapAnimatedObjets.erase( itAnimObjects );
			}
		}
		else
			FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "\"%s\" not found.", _sAnimatedObject.c_str() );
	}

	/////////////////MUSICS MAP/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Load of a music in the map
	//Parameter 1 : Music name
	//Parameter 2 : Path to the music
	//Return value : Loaded (or allready existing) music
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	Music* DataManager::LoadMusic( const std::string& _szName, const std::string& _path )
	{
		MapMusics::iterator it = m_mapMusics.find( _szName );
		if( it == m_mapMusics.end() )
		{
			Music* tmpMusic = new Music;
			tmpMusic->LoadFromFile( _path );

			FZN_LOG( "Loading music \"%s\" at \"%s\".", _szName.c_str(), _path.c_str() );

			return m_mapMusics[_szName] = tmpMusic;
		}
		return m_mapMusics[_szName];
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on a music in the map
	//Parameter : Music name
	//Return value : Concerned music
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	Music* DataManager::GetMusic( const std::string& _szName, bool _bHandleError /*= true*/ )
	{
		MapMusics::iterator it = m_mapMusics.find( _szName );

		if( it != m_mapMusics.end() )
			return it->second;

		if( _bHandleError )
			FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "\"%s\" not found.", _szName.c_str() );
		return nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Music unloading from the map
	//Parameter : Music name
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void DataManager::UnloadMusic( const std::string& _szName )
	{
		MapMusics::iterator it = m_mapMusics.find( _szName );

		if( it != m_mapMusics.end() )
		{
			delete it->second;
			it->second = nullptr;
			m_mapMusics.erase( it );

			FZN_LOG( "Unloading music \"%s\".", _szName.c_str() );
		}
		else
			FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "\"%s\" not found.", _szName.c_str() );
	}

	/////////////////SOUNDS MAP/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Load of a sound in the map
	//Parameter 1 : Sound name
	//Parameter 2 : Path to the sound
	//Return value : Loaded (or allready existing) sound
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	Sound* DataManager::LoadSound( const std::string& _szName, const std::string& _path )
	{
		MapSound::iterator it = m_mapSound.find( _szName );

		if( it == m_mapSound.end() )
		{
			Sound* tmpSound = new Sound;
			tmpSound->LoadFromFile( _path );

			FZN_LOG( "Loading sound \"%s\" at \"%s\".", _szName.c_str(), _path.c_str() );

			return m_mapSound[_szName] = tmpSound;
		}

		return it->second;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on a sound in the map
	//Parameter : Sound name
	//Return value : Concerned sound
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	Sound* DataManager::GetSound( const std::string& _szName, bool _bHandleError /*= true*/ )
	{
		MapSound::iterator it = m_mapSound.find( _szName );

		if( it != m_mapSound.end() )
			return it->second;

		if( _bHandleError )
			FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "\"%s\" not found.", _szName.c_str() );
		return nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Sound unloading from the map
	//Parameter : Sound name
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void DataManager::UnloadSound( const std::string& _szName )
	{
		MapSound::iterator it = m_mapSound.find( _szName );

		if( it != m_mapSound.end() )
		{
			delete it->second;
			it->second = nullptr;
			m_mapSound.erase( it );

			FZN_LOG( "Unloading sound \"%s\".", _szName.c_str() );
		}
		else
			FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "\"%s\" not found.", _szName.c_str() );
	}

	sf::Music* DataManager::LoadSfMusic( const std::string& _name, const std::string& _path )
	{
		MapSfMusics::iterator it = m_mapSfMusics.find( _name );

		if( it == m_mapSfMusics.end() )
		{
			m_mapSfMusics[ _name ] = new sf::Music;
			m_mapSfMusics[ _name ]->openFromFile( _path );

			FZN_LOG( "Loading sf music \"%s\" at \"%s\".", _name.c_str(), _path.c_str() );
			return m_mapSfMusics[ _name ];
		}

		return it->second;
	}

	sf::Music* DataManager::GetSfMusic( const std::string& _name, bool _bHandleError /*= true*/ )
	{
		MapSfMusics::iterator it = m_mapSfMusics.find( _name );

		if( it != m_mapSfMusics.end() )
			return it->second;

		if( _bHandleError )
			FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "%s - \"%s\" not found.", __FUNCTION__, _name.c_str() );
		return nullptr;
	}

	void DataManager::UnloadSfMusic( const std::string& _name )
	{
		MapSfMusics::iterator it = m_mapSfMusics.find( _name );

		if( it != m_mapSfMusics.end() )
		{
			delete it->second;
			it->second = nullptr;
			m_mapSfMusics.erase( it );

			FZN_LOG( "Unloading sf music \"%s\".", _name.c_str() );
		}
		else
			FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "\"%s\" not found.", _name.c_str() );
	}

	sf::SoundBuffer* DataManager::LoadSoundBuffer( const std::string& _name, const std::string& _path )
	{
		MapSoundBuffers::iterator it = m_mapSoundBuffers.find( _name );

		if( it == m_mapSoundBuffers.end() )
		{
			m_mapSoundBuffers[ _name ] = new sf::SoundBuffer;
			m_mapSoundBuffers[ _name ]->loadFromFile( _path );

			FZN_LOG( "Loading sound buffer \"%s\" at \"%s\".", _name.c_str(), _path.c_str() );
			return m_mapSoundBuffers[ _name ];
		}

		return it->second;
	}

	sf::SoundBuffer* DataManager::GetSoundBuffer( const std::string& _name, bool _bHandleError /*= true*/ )
	{
		MapSoundBuffers::iterator it = m_mapSoundBuffers.find( _name );

		if( it != m_mapSoundBuffers.end() )
			return it->second;

		if( _bHandleError )
			FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "\"%s\" not found.", _name.c_str() );
		return nullptr;
	}

	void DataManager::UnloadSoundBuffer( const std::string& _name )
	{
		MapSoundBuffers::iterator it = m_mapSoundBuffers.find( _name );

		if( it != m_mapSoundBuffers.end() )
		{
			delete it->second;
			it->second = nullptr;
			m_mapSoundBuffers.erase( it );

			FZN_LOG( "Unloading sound buffer \"%s\".", _name.c_str() );
		}
		else
			FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "\"%s\" not found.", _name.c_str() );
	}

	/////////////////FONTS MAP/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Load of a font in the map
	//Parameter 1 : Font name
	//Parameter 2 : Path to the font
	//Return value : Loaded (or allready existing) font
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	sf::Font* DataManager::LoadFont( const std::string& _szName, const std::string& _path )
	{
		MapFonts::iterator it = m_mapFonts.find( _szName );

		if( it == m_mapFonts.end() )
		{
			sf::Font* tmpFont = new sf::Font;
			tmpFont->loadFromFile( _path );

			FZN_LOG( "Loading font \"%s\" at \"%s\".", _szName.c_str(), _path.c_str() );

			return m_mapFonts[_szName] = tmpFont;
		}

		return it->second;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on a font in the map
	//Parameter : Font name
	//Return value : Concerned font
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	sf::Font* DataManager::GetFont( const std::string& _szName, bool _bHandleError /*= true*/ )
	{
		MapFonts::iterator it = m_mapFonts.find( _szName );

		if( it != m_mapFonts.end() )
			return it->second;

		if( _bHandleError )
			FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "\"%s\" not found.", _szName.c_str() );
		return nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Font unloading from the map
	//Parameter : Font name
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void DataManager::UnloadFont( const std::string& _szName )
	{
		MapFonts::iterator it = m_mapFonts.find( _szName );

		if( it != m_mapFonts.end() )
		{
			delete it->second;
			it->second = nullptr;
			m_mapFonts.erase( it );

			FZN_LOG( "Unloading font \"%s\".", _szName.c_str() );
		}
		else
			FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "\"%s\" not found.", _szName.c_str() );
	}

	BitmapFont* DataManager::LoadBitmapFont( const std::string& _name, const std::string& _path )
	{
		MapBitmapFonts::iterator it = m_mapBitmapFonts.find( _name );

		if( it == m_mapBitmapFonts.end() )
		{
			BitmapFont* tmpFont = new BitmapFont;
			tmpFont->LoadFromFile( _path );

			FZN_LOG( "Loading bitmap font \"%s\" at \"%s\".", _name.c_str(), _path.c_str() );

			return m_mapBitmapFonts[_name] = tmpFont;
		}

		return it->second;
	}

	BitmapFont* DataManager::GetBitmapFont( const std::string& _name, bool _bHandleError /*= true*/ )
	{
		MapBitmapFonts::iterator it = m_mapBitmapFonts.find( _name );

		if( it != m_mapBitmapFonts.end() )
			return it->second;

		if( _bHandleError )
			FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "\"%s\" not found.", _name.c_str() );
		return nullptr;
	}

	void DataManager::UnloadBitmapFont( const std::string& _name )
	{
		MapBitmapFonts::iterator it = m_mapBitmapFonts.find( _name );

		if( it != m_mapBitmapFonts.end() )
		{
			delete it->second;
			it->second = nullptr;
			m_mapBitmapFonts.erase( it );

			FZN_LOG( "Unloading bitmap font \"%s\".", _name.c_str() );
		}
		else
			FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "\"%s\" not found.", _name.c_str() );
	}

	/////////////////OTHER FUNCTIONS/////////////////

	CustomBitmapGlyph* DataManager::GetBitmapGlyph( const std::string& _name, bool _bHandleError /*= true*/ )
	{
		MapBitmapGylphs::iterator it = m_mapBitmapGlyphs.find( _name );

		if( it != m_mapBitmapGlyphs.end() )
			return &it->second;

		if( _bHandleError )
			FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "\"%s\" not found.", _name.c_str() );
		return nullptr;
	}


	sf::Shader* DataManager::LoadShader( const std::string& _sName, const std::string& _sPath )
	{
		MapShaders::iterator it = m_mapShaders.find( _sName );

		if( it == m_mapShaders.end() )
		{
			const std::string sVert = _sPath + ".vert";
			const std::string sFrag = _sPath + ".frag";

			FZN_LOG( "Loading shader \"%s\" at \"%s\" and \"%s\".", _sName.c_str(), sVert.c_str(), sFrag.c_str() );

			if( m_mapShaders[ _sName ].loadFromFile( sVert, sFrag ) )
			{
				return &m_mapShaders[ _sName ];
			}

			FZN_LOG( "Couldn't load shader \"%s\".", _sName.c_str() );
			m_mapShaders.erase( _sName );
			return nullptr;
		}

		return &it->second;
	}

	sf::Shader* DataManager::GetShader( const std::string& _sName, bool _bHandleError /*= true */ )
	{
		MapShaders::iterator it = m_mapShaders.find( _sName );

		if( it != m_mapShaders.end() )
			return &it->second;

		if( _bHandleError )
			FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "\"%s\" not found.", _sName.c_str() );
		return nullptr;
	}

	void DataManager::UnloadShader( const std::string& _sName )
	{
		MapShaders::iterator it = m_mapShaders.find( _sName );

		if( it != m_mapShaders.end() )
		{
			m_mapShaders.erase( it );

			FZN_LOG( "Unloading shader \"%s\".", _sName.c_str() );
		}
		else
			FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "\"%s\" not found.", _sName.c_str() );
	}


	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Loads the resource file
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void DataManager::LoadResourceFile( const char* _resourceFile /*= DATAPATH( "XMLFiles/Resources" )*/, bool _bLocalResourceFile /*= true*/ )
	{
		tinyxml2::XMLDocument resFile;
		std::string groupName( "" );
		std::string resourcePath( "" );
		std::string sCategoryPathBackup( "" ), sTypePathBackup( "" ), sResourcePathBackUp( "" );
		std::string sTmpPath = "";
		Resource tmpResource;
		ResourceGroup* tmpResGrp;

		MapResourceGroups::iterator itResGrp;

		if( _resourceFile == nullptr || resFile.LoadFile( _resourceFile ) )
		{
			FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "Failure : %s.", resFile.ErrorName() );
			m_bResourceFileExists = false;
			return;
		}

		m_bResourceFileExists = true;

		tinyxml2::XMLElement* allResources = resFile.FirstChildElement( "resources" );
		tinyxml2::XMLElement* category = allResources->FirstChildElement( "display" );
		tinyxml2::XMLElement* type = category->FirstChildElement( "pictures" );
		tinyxml2::XMLElement* resource = type->FirstChildElement( "picture" );


		sTmpPath = Tools::XMLStringAttribute( allResources, "path" );

		if( sTmpPath.empty() == false )
			resourcePath += sTmpPath;

		if( _bLocalResourceFile == false )
		{
			std::filesystem::path oResourceFilePath = _resourceFile;

			size_t iPos = std::string( oResourceFilePath.string() ).find( "Data/" );

			if( iPos != std::string::npos )
			{
				std::string sDataFolderPath = oResourceFilePath.string().substr( 0, iPos + 5 );
				oResourceFilePath = sDataFolderPath;

				if( oResourceFilePath.is_absolute() == false )
				{
					resourcePath = std::filesystem::absolute( oResourceFilePath ).string();
				}
				else
					resourcePath = oResourceFilePath.string();

				Tools::ConvertSlashesInWindowsPath( resourcePath );
			}
		}

		tinyxml2::XMLElement* pBitmapGlyphFiles = allResources->FirstChildElement( "BitmapGlyphFiles" );

		if( pBitmapGlyphFiles != nullptr )
		{
			tinyxml2::XMLElement* pBitmapGlyphFile = pBitmapGlyphFiles->FirstChildElement( "BitmapGlyphFile" );

			while( pBitmapGlyphFile != nullptr )
			{
				const std::string sFilePath = Tools::XMLStringAttribute( pBitmapGlyphFile, "path" );

				if( sFilePath.empty() )
				{
					FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "Bitmap glyph file has no path !" );
					pBitmapGlyphFile = pBitmapGlyphFile->NextSiblingElement();
					continue;
				}
				else
					_LoadBitmapGlyphFile( resourcePath + sFilePath );

				pBitmapGlyphFile = pBitmapGlyphFile->NextSiblingElement();
			}
		}


		sCategoryPathBackup = resourcePath;

		while( category != nullptr )
		{
			sTmpPath = Tools::XMLStringAttribute( category, "path" );

			if( sTmpPath.empty() == false )
				resourcePath += sTmpPath;

			sTypePathBackup = resourcePath;

			type = category->FirstChildElement();

			while( type != nullptr )
			{
				sTmpPath = Tools::XMLStringAttribute( type, "path" );

				if( sTmpPath.empty() == false )
					resourcePath += sTmpPath;

				sResourcePathBackUp = resourcePath;

				resource = type->FirstChildElement();
				
				while( resource != nullptr )
				{
					sTmpPath = Tools::XMLStringAttribute( resource, "path" );

					if( sTmpPath.empty() == false )
						resourcePath += sTmpPath;

					const std::string sGroupNameExists = Tools::XMLStringAttribute( resource, "groupName" );

					if( sGroupNameExists.empty() == false )
					{
						groupName = sGroupNameExists;

						const std::string sName = Tools::XMLStringAttribute( resource, "name" );

						if( sName.empty() == false )
							tmpResource.name = sName;

						tmpResource.path = resourcePath;
						tmpResource.type = resource->Name();

						itResGrp = m_mapResourceGroups.find( groupName );
						if( itResGrp == m_mapResourceGroups.end() )
						{
							tmpResGrp = new ResourceGroup;
							tmpResGrp->m_sName = groupName;
							tmpResGrp->m_bLoaded = false;
							tmpResGrp->m_oResources.push_back( Resource( tmpResource ) );
							m_mapResourceGroups[groupName] = tmpResGrp;
						}
						else 
							itResGrp->second->m_oResources.push_back( Resource( tmpResource ) );

						tmpResource.Reset();
					}
					else
						LoadResourceFromXML( resource, resourcePath );

					resource = resource->NextSiblingElement();
					resourcePath = sResourcePathBackUp;
				}
				type = type->NextSiblingElement();

				resourcePath = sTypePathBackup;
				//resourcePath.resize( resourcePath.size() - 1 );
				//resourcePath = resourcePath.substr( 0, resourcePath.find_last_of( "/" ) + 1 );
			}
			category = category->NextSiblingElement();

			resourcePath = sCategoryPathBackup;
			//resourcePath.resize( resourcePath.size() - 1 );
			//resourcePath = resourcePath.substr( 0, resourcePath.find_last_of( "/" ) + 1 );
		}

		m_bResourceFileLoaded = true;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Loads a group of resources
	//Parameter : Group to load
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void DataManager::LoadResourceGroup( const char* _group )
	{
		//Loading of the resource file if it hasn't been done allready
		if( !m_bResourceFileLoaded )
			LoadResourceFile();

		if( m_bResourceFileExists )
		{
			MapResourceGroups::iterator it = m_mapResourceGroups.find( _group );

			if( it != m_mapResourceGroups.end() )
			{
				FZN_LOG( "Loading resource group \"%s\".", _group );

				for( const Resource& oResource : it->second->m_oResources )
					LoadResourceFromXML( oResource );

				it->second->m_bLoaded = true;
			}
			else
				FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "Group \"%s\" not found.", _group );
		}
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Unloads a group of resources
	//Parameter : Group to unload
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void DataManager::UnloadResourceGroup( const char* _group )
	{
		if( m_bResourceFileLoaded )
		{
			MapResourceGroups::iterator it = m_mapResourceGroups.find( _group );

			if( it != m_mapResourceGroups.end() && it->second->m_bLoaded )
			{
				for( const Resource& oResource : it->second->m_oResources )
					UnloadResource( oResource );

				it->second->m_bLoaded = false;
			}
			else
				FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "Group \"%s\" not found or not loaded.", _group );
		}
		else
			FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "Group \"%s\" can't be unloaded, resource file hasn't been loaded.", _group );
	}


	void DataManager::SetSmoothTextures( bool _bSmooth )
	{
		m_bSmoothTextures = _bSmooth;
	}

	bool DataManager::GetSmoothTextures() const
	{
		return m_bSmoothTextures;
	}

	bool DataManager::ResourceExists( const ResourceType& _eType, const std::string& _sResourceName, const std::string& _sAdditionalName /*= "" */ )
	{
		switch( _eType )
		{
		case ResourceType::eTexture:
		{
			if( GetTexture( _sResourceName, false ) != nullptr )
				return true;

			break;
		}
		case ResourceType::eMusic:
		{
			if( g_pFZN_Core->IsUsingFMOD() )
			{
				if( GetMusic( _sResourceName, false ) != nullptr )
					return true;
			}
			else if( GetSfMusic( _sResourceName, false ) != nullptr )
				return true;

			break;
		}
		case ResourceType::eSound:
		{
			if( g_pFZN_Core->IsUsingFMOD() )
			{
				if( GetSound( _sResourceName, false ) != nullptr )
					return true;
			}
			else if( GetSoundBuffer( _sResourceName, false ) != nullptr )
				return true;

			break;
		}
		case ResourceType::eFont:
		{
			if( GetFont( _sResourceName, false ) != nullptr )
				return true;

			break;
		}
		case ResourceType::eBitmapFont:
		{
			if( GetBitmapFont( _sResourceName, false ) != nullptr )
				return true;

			break;
		}
		case ResourceType::eAnimation:
		{
			if( GetAnimation( _sResourceName, false ) != nullptr )
				return true;

			break;
		}
		case ResourceType::eAnm2:
		{
			if( GetAnm2( _sResourceName, _sAdditionalName, false ) != nullptr )
				return true;

			break;
		}
		case ResourceType::eBitmapGlyph:
		{
			if( GetBitmapGlyph( _sResourceName, false ) != nullptr )
				return true;

			break;
		}
		}

		for( MapResourceGroups::const_iterator itResourceGroup = m_mapResourceGroups.cbegin(); itResourceGroup != m_mapResourceGroups.cend(); ++itResourceGroup )
		{
			for( const Resource& oResource : itResourceGroup->second->m_oResources )
			{
				if( oResource.name == _sResourceName )
					return true;
			}
		}

		FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "Resource %s (%d) doesn't exist !", _sResourceName.c_str(), _eType );
		return false;
	}

	//=========================================================
	//==========================PRIVATE=========================
	//=========================================================


	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Loads a resource in the map from the xml resource file
	//Parameter : Node containing the informations on the resource to load
	//Parameter 2 : Path to the resource
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void DataManager::LoadResourceFromXML( tinyxml2::XMLElement* _node, const std::string& _path )
	{
		if( strcmp( _node->Name(), "picture" ) == 0 )
		{
			LoadTexture( Tools::XMLStringAttribute( _node, "name" ), _path );
			return;
		}
		else if( strcmp( _node->Name(), "animation" ) == 0 )
		{
			LoadAnimation( Tools::XMLStringAttribute( _node, "name" ), _path );
			return;
		}
		else if( strcmp( _node->Name(), "anm2" ) == 0 )
		{
			LoadAnm2s( Tools::XMLStringAttribute( _node, "name" ), _path );
			return;
		}
		else if( strcmp( _node->Name(), "font" ) == 0 )
		{
			LoadFont( Tools::XMLStringAttribute( _node, "name" ), _path );
			return;
		}
		else if( strcmp( _node->Name(), "bmpFont" ) == 0 )
		{
			LoadBitmapFont( Tools::XMLStringAttribute( _node, "name" ), _path );
			return;
		}
		else if( strcmp( _node->Name(), "Shader" ) == 0 )
		{
			LoadShader( Tools::XMLStringAttribute( _node, "name" ), _path );
			return;
		}
		else if( strcmp( _node->Name(), "sound" ) == 0 )
		{
			if( g_pFZN_Core->IsUsingFMOD() )
				LoadSound( Tools::XMLStringAttribute( _node, "name" ), _path );
			else
				LoadSoundBuffer( Tools::XMLStringAttribute( _node, "name" ), _path );
			return;
		}
		else if( strcmp( _node->Name(), "music" ) == 0 )
		{
			if( g_pFZN_Core->IsUsingFMOD() )
				LoadMusic( Tools::XMLStringAttribute( _node, "name" ), _path );
			else
				LoadSfMusic( Tools::XMLStringAttribute( _node, "name" ), _path );
			return;
		}
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Loads a resource in the map from the xml resource file
	//Parameter 1 : Name of the resource
	//Parameter 2 : Path to the resource
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void DataManager::LoadResourceFromXML( const Resource& _oResource )
	{
		if( _oResource.type == "picture" )
		{
			LoadTexture( _oResource.name, _oResource.path );
			return;
		}
		else if( _oResource.type == "animation" )
		{
			LoadAnimation( _oResource.name, _oResource.path );
			return;
		}
		else if( _oResource.type == "anm2" )
		{
			LoadAnm2s( _oResource.name, _oResource.path );
			return;
		}
		else if( _oResource.type == "font" )
		{
			LoadFont( _oResource.name, _oResource.path );
			return;
		}
		else if( _oResource.type == "bmpFont" )
		{
			LoadBitmapFont( _oResource.name, _oResource.path );
			return;
		}
		else if( _oResource.type == "Shader" )
		{
			LoadShader( _oResource.name, _oResource.path );
			return;
		}
		else if( _oResource.type == "sound" )
		{
			if( g_pFZN_Core->IsUsingFMOD() )
				LoadSound( _oResource.name, _oResource.path );
			else
				LoadSoundBuffer( _oResource.name, _oResource.path );
			return;
		}
		else if( _oResource.type == "music" )
		{
			if( g_pFZN_Core->IsUsingFMOD() )
				LoadMusic( _oResource.name, _oResource.path );
			else
				LoadSfMusic( _oResource.name, _oResource.path );
			return;
		}
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Unloads a resource in the map from the xml resource file
	//Parameter : Resource to unload
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void DataManager::UnloadResource( const Resource& _oResource )
	{
		if( _oResource.type == "picture" )
		{
			UnloadTexture( _oResource.name );
			return;
		}
		else if( _oResource.type == "animation" )
		{
			UnloadAnimation( _oResource.name );
			return;
		}
		else if( _oResource.type == "anm2" )
		{
			UnloadAnm2( _oResource.name );
			return;
		}
		else if( _oResource.type == "font" )
		{
			UnloadFont( _oResource.name );
			return;
		}
		else if( _oResource.type == "bmpFont" )
		{
			UnloadBitmapFont( _oResource.name );
			return;
		}
		else if( _oResource.type == "sound" )
		{
			if( g_pFZN_Core->IsUsingFMOD() )
				UnloadSound( _oResource.name );
			else
				UnloadSoundBuffer( _oResource.name );
			return;
		}
		else if( _oResource.type == "music" )
		{
			if( g_pFZN_Core->IsUsingFMOD() )
				UnloadMusic( _oResource.name );
			else
				UnloadSfMusic( _oResource.name );
			return;
		}
	}

	void DataManager::_LoadBitmapGlyphFile( const std::string& _sFile )
	{
		tinyxml2::XMLDocument resFile;

		if( resFile.LoadFile( _sFile.c_str() ) )
		{
			FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "Failure : %s.", resFile.ErrorName() );
			return;
		}

		tinyxml2::XMLElement* pGlyphs = resFile.FirstChildElement( "BitmapGlyphs" );

		if( pGlyphs == nullptr )
		{
			FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "Failure : \"BitmapGlyphs\" tag not found." );
			return;
		}

		tinyxml2::XMLElement* pGlyph = pGlyphs->FirstChildElement( "BitmapGlyph" );

		while( pGlyph != nullptr )
		{
			std::string sName = pGlyph->Attribute( "Name" );

			CustomBitmapGlyph oGlyph;

			std::string sTexturePath = Tools::XMLStringAttribute( pGlyph, "TexturePath" );
			std::string sTextureFile = Tools::XMLStringAttribute( pGlyph, "TextureName" );

			oGlyph.second = g_pFZN_DataMgr->LoadTexture( sTextureFile, sTexturePath );

			oGlyph.first.m_fTop		= pGlyph->FloatAttribute( "Top" );
			oGlyph.first.m_fLeft	= pGlyph->FloatAttribute( "Left" );
			oGlyph.first.m_fWidth	= pGlyph->FloatAttribute( "Width" );
			oGlyph.first.m_fHeight	= pGlyph->FloatAttribute( "Height" );

			m_mapBitmapGlyphs[ sName ] = oGlyph;

			pGlyph = pGlyph->NextSiblingElement();
		}
	}

	void DataManager::_LoadShaders()
	{
		m_mapShaders[ "ColorOverlay" ].loadFromMemory( Shaders::ColorOverlay_Vert, Shaders::ColorOverlay_Frag );
	}

} //namespace fzn
