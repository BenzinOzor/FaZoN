//------------------------------------------------------------------------
//Author : Philippe OFFERMANN
//Date : 20.12.13
//Description : Data manager (textures, animations, sounds, musics and fonts)
//------------------------------------------------------------------------

#include <filesystem>

#include <ExternalWrapper/EWFileEncrypter.h>

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

#define ResourceLoadFctLambda( Fct ) [&]( const std::string& a, const std::string& b ) -> std::any { return Fct( a, b ); }
#define ResourceLoadFctLambdaFmod( Fct1, Fct2 ) [&]( const std::string& a, const std::string& b ) -> std::any { if( g_pFZN_Core->IsUsingFMOD() ) return Fct1( a, b ); return Fct2( a, b, USINGCRYPTEDFILES ); }
#define ResourceUnloadFctLambda( Fct ) [&]( const std::string& a ) { Fct( a ); }
#define ResourceUnloadFctLambdaFmod( Fct1, Fct2 ) [&]( const std::string& a ) { if( g_pFZN_Core->IsUsingFMOD() ) Fct1( a ); Fct2( a ); }


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

		m_mapResourceLoadFcts.insert( { "Picture", [&]( const std::string& a, const std::string& b ) -> std::any { return DataManager::LoadTexture( a,b, USINGCRYPTEDFILES ); } } );
		m_mapResourceLoadFcts.insert( { "Animation", ResourceLoadFctLambda( DataManager::LoadAnimation ) } );
		m_mapResourceLoadFcts.insert( { "Anm2", [&]( const std::string& a, const std::string& b ) -> std::any { DataManager::LoadAnm2s( a,b ); return 0;} } );
		m_mapResourceLoadFcts.insert( { "Font", ResourceLoadFctLambda( DataManager::LoadFont ) } );
		m_mapResourceLoadFcts.insert( { "BmpFont", ResourceLoadFctLambda( DataManager::LoadBitmapFont ) } );
		m_mapResourceLoadFcts.insert( { "Shader", ResourceLoadFctLambda( DataManager::LoadShader ) } );
		m_mapResourceLoadFcts.insert( { "Sound", ResourceLoadFctLambdaFmod( DataManager::LoadSound, DataManager::LoadSoundBuffer ) } );
		m_mapResourceLoadFcts.insert( { "Music", ResourceLoadFctLambdaFmod( DataManager::LoadMusic, DataManager::LoadSfMusic ) } );

		m_mapResourceUnloadFcts.insert( { "Picture", ResourceUnloadFctLambda( DataManager::UnloadTexture ) } );
		m_mapResourceUnloadFcts.insert( { "Animation", ResourceUnloadFctLambda( DataManager::UnloadAnimation ) } );
		m_mapResourceUnloadFcts.insert( { "Anm2", ResourceUnloadFctLambda( DataManager::UnloadAnm2 ) } );
		m_mapResourceUnloadFcts.insert( { "Font", ResourceUnloadFctLambda( DataManager::UnloadFont ) } );
		m_mapResourceUnloadFcts.insert( { "BmpFont", ResourceUnloadFctLambda( DataManager::UnloadBitmapFont ) } );
		m_mapResourceUnloadFcts.insert( { "Sound", ResourceUnloadFctLambdaFmod( DataManager::UnloadSound, DataManager::UnloadSoundBuffer ) } );
		m_mapResourceUnloadFcts.insert( { "Music", ResourceUnloadFctLambdaFmod( DataManager::UnloadMusic, DataManager::UnloadSfMusic ) } );
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
			delete sfMusIt->second.m_pMusic;
			sfMusIt->second.m_pMusic = nullptr;
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
	sf::Texture* DataManager::LoadTexture( const std::string& _szName, const std::string& _path, bool _bCryptedFile /*= USINGCRYPTEDFILES*/ )
	{
		MapTextures::iterator it = m_mapTextures.find( _szName );

		if( it == m_mapTextures.end() )
		{
			if( _bCryptedFile )
				return _LoadCryptedTexture( _szName, _path );

			sf::Texture* tmpTexture = new sf::Texture;
			tmpTexture->loadFromFile( _path );
			tmpTexture->setSmooth( m_bSmoothTextures );

			FZN_LOG( "Loading texture \"%s\" at \"%s\".", _szName.c_str(), _path.c_str() );

			_SendFileLoadedEvent();
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

		if( g_pFZN_DataMgr->LoadXMLFile( oAnmFile, _sFile ) )
		{
			FZN_COLOR_LOG( DBG_MSG_COL_RED, "Failure : %s", oAnmFile.ErrorStr() );
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

	sf::Music* DataManager::LoadSfMusic( const std::string& _name, const std::string& _path, bool _bCryptedFile /*= USINGCRYPTEDFILES*/ )
	{
		MapSfMusics::iterator it = m_mapSfMusics.find( _name );

		if( it == m_mapSfMusics.end() )
		{
			if( _bCryptedFile )
				return _LoadCryptedSfMusic( _name, _path );

			m_mapSfMusics[ _name ].m_pMusic = new sf::Music;
			m_mapSfMusics[ _name ].m_pMusic->openFromFile( _path );

			FZN_LOG( "Loading sf music \"%s\" at \"%s\".", _name.c_str(), _path.c_str() );

			_SendFileLoadedEvent();
			return m_mapSfMusics[ _name ].m_pMusic;
		}

		return it->second.m_pMusic;
	}

	sf::Music* DataManager::GetSfMusic( const std::string& _name, bool _bHandleError /*= true*/ )
	{
		MapSfMusics::iterator it = m_mapSfMusics.find( _name );

		if( it != m_mapSfMusics.end() )
			return it->second.m_pMusic;

		if( _bHandleError )
			FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "%s - \"%s\" not found.", __FUNCTION__, _name.c_str() );
		return nullptr;
	}

	void DataManager::UnloadSfMusic( const std::string& _name )
	{
		MapSfMusics::iterator it = m_mapSfMusics.find( _name );

		if( it != m_mapSfMusics.end() )
		{
			delete it->second.m_pMusic;
			it->second.m_pMusic = nullptr;
			m_mapSfMusics.erase( it );

			FZN_LOG( "Unloading sf music \"%s\".", _name.c_str() );
		}
		else
			FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "\"%s\" not found.", _name.c_str() );
	}

	sf::SoundBuffer* DataManager::LoadSoundBuffer( const std::string& _name, const std::string& _path, bool _bCryptedFile /*= USINGCRYPTEDFILES*/ )
	{
		MapSoundBuffers::iterator it = m_mapSoundBuffers.find( _name );

		if( it == m_mapSoundBuffers.end() )
		{
			if( _bCryptedFile )
				return _LoadCryptedSoundBuffer( _name, _path );

			m_mapSoundBuffers[ _name ] = new sf::SoundBuffer;
			m_mapSoundBuffers[ _name ]->loadFromFile( _path );

			FZN_LOG( "Loading sound buffer \"%s\" at \"%s\".", _name.c_str(), _path.c_str() );

			_SendFileLoadedEvent();
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

	CustomBitmapGlyph* DataManager::GetBitmapGlyph( const std::string& _sName, const std::string& _sTag, bool _bHandleError /*= true*/ )
	{
		BitmapGylphs::iterator it = std::find_if( m_oBitmapGlyphs.begin(), m_oBitmapGlyphs.end(), [ _sName, _sTag ]( const CustomBitmapGlyph& oGlyph )
		{
			return oGlyph.m_sName == _sName &&  oGlyph.m_sTag == _sTag;
		} );

		if( it != m_oBitmapGlyphs.end() )
			return &(*it);

		if( _bHandleError )
			FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "Bitmap glyph \"%s\"%s not found.", _sName.c_str(), _sTag.empty() ? "" : Tools::Sprintf( " (%s)", _sTag.c_str() ).c_str() );
		return nullptr;
	}


	sf::Shader* DataManager::LoadShader( const std::string& _sName, const std::string& _sPath, bool _bCryptedFile /*= USINGCRYPTEDFILES*/ )
	{
		MapShaders::iterator it = m_mapShaders.find( _sName );

		if( it == m_mapShaders.end() )
		{
			if( _bCryptedFile )
				return _LoadCryptedShader( _sName, _sPath );

			const std::string sVert = _sPath + ".vert";
			const std::string sFrag = _sPath + ".frag";

			FZN_LOG( "Loading shader \"%s\" at \"%s\" and \"%s\".", _sName.c_str(), sVert.c_str(), sFrag.c_str() );

			if( m_mapShaders[ _sName ].loadFromFile( sVert, sFrag ) )
			{
				_SendFileLoadedEvent();
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


	std::string DataManager::LoadTextFile( const std::string& _sPath, bool _bCryptedFile /*= USINGCRYPTEDFILES*/ )
	{
		if( _bCryptedFile )
		{
			std::vector< unsigned char > oDecryptedData = _DecryptFile( _sPath, true );
			std::string sContent = ( (char*)&( oDecryptedData[ 0 ] ) );

			return sContent;
		}
		
		auto file_stream = std::ifstream{ _sPath };

		return { ( std::istreambuf_iterator<char>( file_stream ) ), std::istreambuf_iterator<char>() };
	}

	tinyxml2::XMLError DataManager::LoadXMLFile( tinyxml2::XMLDocument& _oFile, const std::string& _sPath, bool _bCryptedFile /*= USINGCRYPTEDFILES*/ )
	{
		if( _bCryptedFile == false )
			return _oFile.LoadFile( _sPath.c_str() );

		if( _sPath.empty() )
			return tinyxml2::XMLError::XML_ERROR_FILE_NOT_FOUND;

		std::string sXML = LoadTextFile( _sPath, _bCryptedFile );

		return _oFile.Parse( sXML.c_str() );
	}

	bool DataManager::LoadSfImage( sf::Image& _oImage, const std::string& _sPath, bool _bCryptedFile /*= USINGCRYPTEDFILES */ )
	{
		if( _bCryptedFile == false )
			return _oImage.loadFromFile( _sPath );

		std::vector< unsigned char >	oDecryptedData	= _DecryptFile( _sPath );
		unsigned char*					pFirstByte		= (unsigned char*)( &oDecryptedData[ 0 ] );

		return _oImage.loadFromMemory( pFirstByte, oDecryptedData.size() * sizeof( unsigned char ) );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Loads the resource file
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void DataManager::LoadResourceFile( const char* _resourceFile /*= DATAPATH( "XMLFiles/Resources" )*/, bool _bLocalResourceFile /*= true*/ )
	{
		tinyxml2::XMLDocument resFile;

		if( _resourceFile == nullptr || g_pFZN_DataMgr->LoadXMLFile( resFile, _resourceFile ) )
		{
			FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "Failure : %s.", resFile.ErrorStr() );
			m_bResourceFileExists = false;
			return;
		}

		m_bResourceFileExists = true;

		tinyxml2::XMLElement* pResources = resFile.FirstChildElement( "Resources" );

		if( pResources == nullptr )
		{
			FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "Failure : \"Resources\" tag not found." );
			return;
		}

		_LookForResources( pResources, Tools::XMLStringAttribute( pResources, "Path" ) );

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

				Event oEvent( Event::eBeginFileLoading );

				oEvent.m_oFileLoading.m_iNbFilesToLoad = it->second->m_oResources.size();

				g_pFZN_Core->PushEvent( oEvent );

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
		/*case ResourceType::eBitmapGlyph:
		{
			if( GetBitmapGlyph( _sResourceName, false ) != nullptr )
				return true;

			break;
		}*/
		}

		for( MapResourceGroups::const_iterator itResourceGroup = m_mapResourceGroups.cbegin(); itResourceGroup != m_mapResourceGroups.cend(); ++itResourceGroup )
		{
			for( const Resource& oResource : itResourceGroup->second->m_oResources )
			{
				if( oResource.m_sName == _sResourceName )
					return true;
			}
		}

		FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "Resource %s (%d) doesn't exist !", _sResourceName.c_str(), _eType );
		return false;
	}

	//=========================================================
	//==========================PRIVATE=========================
	//=========================================================


	/////////////////CRYPTED DATA LOADING/////////////////

	sf::Texture* DataManager::_LoadCryptedTexture( const std::string& _sName, const std::string& _sPath )
	{
		FZN_LOG( "Loading texture \"%s\" at \"%s\".", _sName.c_str(), _sPath.c_str() );

		std::vector< unsigned char >	oDecryptedData	= _DecryptFile( _sPath );
		unsigned char*					pFirstByte		= (unsigned char*)( &oDecryptedData[ 0 ] );

		sf::Texture* pTexture = new sf::Texture;

		if( pTexture->loadFromMemory( pFirstByte, oDecryptedData.size() * sizeof( unsigned char ) ) )
		{
			pTexture->setSmooth( m_bSmoothTextures );
			_SendFileLoadedEvent();
			return m_mapTextures[ _sName ] = pTexture;
		}
		
		delete pTexture;
		return nullptr;
	}

	sf::Music* DataManager::_LoadCryptedSfMusic( const std::string& _sName, const std::string& _sPath )
	{
		FZN_LOG( "Loading sf music \"%s\" at \"%s\".", _sName.c_str(), _sPath.c_str() );


		m_mapSfMusics[ _sName ].m_oData	= _DecryptFile( _sPath );
		unsigned char* pFirstByte		= (unsigned char*)( &m_mapSfMusics[ _sName ].m_oData[ 0 ] );

		m_mapSfMusics[ _sName ].m_pMusic = new sf::Music;

		if( m_mapSfMusics[ _sName ].m_pMusic->openFromMemory( pFirstByte, m_mapSfMusics[ _sName ].m_oData.size() * sizeof( unsigned char ) ) )
		{
			_SendFileLoadedEvent();
			return m_mapSfMusics[ _sName ].m_pMusic;
		}

		delete m_mapSfMusics[ _sName ].m_pMusic;
		m_mapSfMusics.erase( _sName );
		return nullptr;
	}

	sf::SoundBuffer* DataManager::_LoadCryptedSoundBuffer( const std::string& _sName, const std::string& _sPath )
	{
		FZN_LOG( "Loading sound buffer \"%s\" at \"%s\".", _sName.c_str(), _sPath.c_str() );

		std::vector< unsigned char >	oDecryptedData	= _DecryptFile( _sPath );
		unsigned char*					pFirstByte		= (unsigned char*)( &oDecryptedData[ 0 ] );

		sf::SoundBuffer* pSoundBuffer = new sf::SoundBuffer;

		if( pSoundBuffer->loadFromMemory( pFirstByte, oDecryptedData.size() * sizeof( unsigned char ) ) )
		{
			_SendFileLoadedEvent();
			return m_mapSoundBuffers[ _sName ] = pSoundBuffer;
		}
		
		delete pSoundBuffer;
		return nullptr;
	}

	sf::Shader* DataManager::_LoadCryptedShader( const std::string& _sName, const std::string& _sPath )
	{
		const std::string sVert = _sPath + ".sdrv";
		const std::string sFrag = _sPath + ".sdrf";

		FZN_LOG( "Loading shader \"%s\" at \"%s\" and \"%s\".", _sName.c_str(), sVert.c_str(), sFrag.c_str() );
		
		const std::string sVertContent = LoadTextFile( sVert, true );
		const std::string sFragContent = LoadTextFile( sFrag, true );

		if( m_mapShaders[ _sName ].loadFromMemory( sVertContent, sFragContent ) )
		{
			_SendFileLoadedEvent();
			return &m_mapShaders[ _sName ];
		}
		
		FZN_LOG( "Couldn't load shader \"%s\".", _sName.c_str() );

		return nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Loads a resource in the map from the xml resource file
	//Parameter : Node containing the informations on the resource to load
	//Parameter 2 : Path to the resource
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void DataManager::LoadResourceFromXML( tinyxml2::XMLElement* _node, const std::string& _path )
	{
		_FindAndLoadResource( _node->Name(), Tools::XMLStringAttribute( _node, "Name" ), _path );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Loads a resource in the map from the xml resource file
	//Parameter 1 : Name of the resource
	//Parameter 2 : Path to the resource
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void DataManager::LoadResourceFromXML( const Resource& _oResource )
	{
		_FindAndLoadResource( _oResource.m_sType, _oResource.m_sName, _oResource.m_sPath );
	}

	void DataManager::_FindAndLoadResource( const std::string& _sType, const std::string& _sName, const std::string& _sPath )
	{
		MapResourceLoadFcts::iterator it = m_mapResourceLoadFcts.find( _sType );

		if( it != m_mapResourceLoadFcts.end() )
			it->second( _sName, _sPath );
		else
			FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "Resource type \"%s\" doesn't exist ! Resource \"%s\" couldn't be loaded.", _sType.c_str(), _sName.c_str() );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Unloads a resource in the map from the xml resource file
	//Parameter : Resource to unload
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void DataManager::UnloadResource( const Resource& _oResource )
	{
		MapResourceUnloadFcts::iterator it = m_mapResourceUnloadFcts.find( _oResource.m_sType );

		if( it != m_mapResourceUnloadFcts.end() )
			it->second( _oResource.m_sName );
		else
			FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "Resource type \"%s\" doesn't exist ! Resource \"%s\" couldn't be unloaded.", _oResource.m_sType.c_str(), _oResource.m_sName.c_str() );
	}

	void DataManager::_LoadBitmapGlyphFile( const std::string& _sFile )
	{
		tinyxml2::XMLDocument resFile;

		if( g_pFZN_DataMgr->LoadXMLFile( resFile, _sFile ) )
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
			CustomBitmapGlyph oGlyph;
			oGlyph.m_sName	= Tools::XMLStringAttribute( pGlyph, "Name" );
			oGlyph.m_sTag	= Tools::XMLStringAttribute( pGlyph, "Tag" );

			oGlyph.m_oGlyph.m_fTop		= pGlyph->FloatAttribute( "Top" );
			oGlyph.m_oGlyph.m_fLeft		= pGlyph->FloatAttribute( "Left" );
			oGlyph.m_oGlyph.m_fWidth	= pGlyph->FloatAttribute( "Width" );
			oGlyph.m_oGlyph.m_fHeight	= pGlyph->FloatAttribute( "Height" );

			std::string sTexturePath = Tools::XMLStringAttribute( pGlyph, "TexturePath" );
			std::string sTextureFile = Tools::XMLStringAttribute( pGlyph, "TextureName" );

			oGlyph.m_pTexture = g_pFZN_DataMgr->LoadTexture( sTextureFile, sTexturePath );

			m_oBitmapGlyphs.push_back( oGlyph );

			pGlyph = pGlyph->NextSiblingElement();
		}
	}

	void DataManager::_LoadShaders()
	{
		m_mapShaders[ "ColorOverlay" ].loadFromMemory( Shaders::ColorOverlay_Vert, Shaders::ColorOverlay_Frag );
	}

	void DataManager::_SendFileLoadedEvent()
	{
		g_pFZN_Core->PushEvent( Event::eFileLoaded );
	}

	std::vector< unsigned char > DataManager::_DecryptFile( const std::string& _sPath, bool _bTextFile /*= false*/ )
	{
		std::vector< unsigned char > oRet;
		EWFileEncrypter::CryptionTaskResult eResult = EWFileEncrypter::Decrypt_File( _sPath, oRet, _bTextFile );

		if( eResult == EWFileEncrypter::CryptionTaskResult::eFileNotFound )
			FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "Failure : File not found. (%s)", _sPath.c_str() );
		else if( eResult == EWFileEncrypter::CryptionTaskResult::eRSAKeyMismatch )
			FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "Failure : Decrypting key mismatch. (%s)", _sPath.c_str() );

		return oRet;
	}

	void DataManager::_LookForResources( tinyxml2::XMLNode* _pNode, const std::string& _sCurrenntPath )
	{
		if( _pNode == nullptr )
			return;

		tinyxml2::XMLNode* pNode = _pNode->FirstChild();

		while( pNode != nullptr )
		{
			tinyxml2::XMLElement* pElement = pNode->ToElement();

			if( pElement == nullptr )
			{
				pNode = pNode->NextSibling();
				continue;
			}

			std::string sPath = _sCurrenntPath + Tools::XMLStringAttribute( pElement, "Path" );

			if( strcmp( pElement->Name(), "Folder" ) == 0 )
				_LookForResources( pNode, sPath );
			else
				_AddResource( pElement, sPath );

			pNode = pNode->NextSibling();
		}
	}

	void DataManager::_AddResource( tinyxml2::XMLElement* _pElement, const std::string& _sCurrenntPath )
	{
		if( _pElement == nullptr )
			return;

		const std::string sResourceGroup = Tools::XMLStringAttribute( _pElement, "Group" );

		if( sResourceGroup.empty() )
			LoadResourceFromXML( _pElement, _sCurrenntPath );
		else
		{
			MapResourceGroups::iterator itResGrp = m_mapResourceGroups.find( sResourceGroup );

			std::vector< Resource >* pResourcesVector = nullptr;
			if( itResGrp != m_mapResourceGroups.end() && itResGrp->second != nullptr )
				pResourcesVector = &itResGrp->second->m_oResources;
			else
			{
				ResourceGroup* pResGrp = new ResourceGroup();

				pResGrp->m_sName = sResourceGroup;
				pResGrp->m_bLoaded = false;
				pResourcesVector = &pResGrp->m_oResources;
				m_mapResourceGroups[ sResourceGroup ] = pResGrp;
			}

			pResourcesVector->push_back( { Tools::XMLStringAttribute( _pElement, "Name" ), _sCurrenntPath, _pElement->Name() } );
		}
	}
} //namespace fzn
