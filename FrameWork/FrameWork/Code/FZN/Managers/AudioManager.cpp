//------------------------------------------------------------------------
//Author : Philippe OFFERMANN
//Date : 28.04.15
//Description : Audio system manager
//------------------------------------------------------------------------

#include "FZN/Includes.h"
#include <Fmod/fmod.hpp>
#include "FZN/Managers/AudioManager.h"
#include "FZN/Managers/DataManager.h"


FZN_EXPORT fzn::AudioManager* g_pFZN_AudioMgr = nullptr;

const char* SOUNDS_FILE_NAME = "XMLFiles/Sounds.xml";

namespace fzn
{
	/////////////////CONSTRUCTOR / DESTRUCTOR/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Default constructor
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	AudioManager::AudioManager( bool _bUseFMOD /*= false*/ )
		: m_bUseFMOD( _bUseFMOD )
	{
		if( m_bUseFMOD )
		{
			if( FMOD::System_Create( &m_system ) != FMOD_RESULT::FMOD_OK )
				FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "FMOD system creation error" );

			if( m_system->init( ChannelsNumber, FMOD_INIT_NORMAL, nullptr ) != FMOD_RESULT::FMOD_OK )
				FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "FMOD system initialisation error" );
		}

		_LoadSoundPoolFromXML();

		g_pFZN_AudioMgr = this;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Default destructor
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	AudioManager::~AudioManager()
	{
		if( m_system != nullptr )
			m_system->release();

		SoundInstances::iterator itSound = m_oSounds.begin();
		while( itSound != m_oSounds.end() )
		{
			CheckNullptrDelete( itSound->m_pSound );

			++itSound;
		}
		m_oSounds.clear();

		itSound = m_oAvailableSounds.begin();
		while( itSound != m_oAvailableSounds.end() )
		{
			CheckNullptrDelete( itSound->m_pSound );

			++itSound;
		}
		m_oAvailableSounds.clear();

		g_pFZN_AudioMgr = nullptr;
	}


	/////////////////MANAGEMENT FUNCTIONS/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Updates the channels
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void AudioManager::Update()
	{
		if( m_bUseFMOD )
		{
			for( int i = 0; i < ChannelsNumber; ++i )
				m_channels[ i ].Update();
		}

		_CheckWaitingList();

		SoundInstances::iterator itSound = m_oSounds.begin();
		while( itSound != m_oSounds.end() )
		{
			bool bRemove = false;

			if( itSound->m_pSound != nullptr )
			{
				if( itSound->m_pSound->getStatus() == sf::Music::Stopped )
					bRemove = true;
			}
			else
				bRemove = true;

			if( bRemove )
			{
				SoundInfo oSoundInfo = *itSound;

				m_oAvailableSounds.push_back( oSoundInfo );

				itSound = m_oSounds.erase( itSound );
			}
			else
				++itSound;
		}
	}


	/////////////////SOUND FUNCTIONS/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Plays an AudioObject on the right channel
	//Parameter 1 : AudioObject to play
	//Parameter 2 : Channel group
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void AudioManager::Sound_Play(AudioObject& _audioObject, FMOD::ChannelGroup* _channelGroup)
	{
		int channel;
		m_system->playSound( _audioObject.GetSoundComponent(), _channelGroup, false, &m_channel );
		m_channel->getIndex( &channel );
		_audioObject.m_channelIndex = channel;

		m_channels[ channel ].SetChannelComponent( *m_channel );
		m_channels[ channel ].SetAudioObject( &_audioObject );
	}

	void AudioManager::Sound_Play( const std::string& _sSound, bool _bOnlyOne /*= false*/, bool _bLoop /*= false */ )
	{
		if( _sSound.empty() )
			return;

		if( _bOnlyOne )
		{
			SoundInstances::iterator itSound = _FindSound( m_oSounds, _sSound );

			if( itSound != m_oSounds.end() )
			{
				if( _bLoop )
					++itSound->m_iNbInstances;

				return;
			}
		}

		SoundInstances::iterator itSound = _FindSound( m_oWaitingList, _sSound );

		if( itSound != m_oWaitingList.end() )
		{
			if( _bOnlyOne && _bLoop )
				++itSound->m_iNbInstances;

			return;
		}

		SoundInfo oSound;
		oSound.m_sName = _sSound;
		oSound.m_bLoop = _bLoop;

		if( _bOnlyOne && _bLoop )
			++oSound.m_iNbInstances;

		m_oWaitingList.push_back( oSound );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Pauses an AudioObject on the right channel
	//Parameter : AudioObject to pause
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void AudioManager::Sound_Pause(AudioObject& _audioObject)
	{
		m_channels[_audioObject.m_channelIndex].Pause();
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Resumes an AudioObject on the right channel
	//Parameter : AudioObject to resume
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void AudioManager::Sound_Resume(AudioObject& _audioObject)
	{
		m_channels[ _audioObject.m_channelIndex ].Play();
	}


	void AudioManager::Sound_Stop( AudioObject& _audioObject )
	{
		if( m_bUseFMOD )
			m_channels[ _audioObject.m_channelIndex ].Stop();
	}

	void AudioManager::Sound_Stop( const std::string& _sSound )
	{
		if( _sSound.empty() )
			return;

		SoundInstances::iterator itSound = _FindSound( m_oSounds, _sSound );

		if( itSound != m_oSounds.end() )
		{
			if( itSound->m_iNbInstances > 0 )
				--itSound->m_iNbInstances;

			if( itSound->m_iNbInstances <= 0 )
				itSound->m_pSound->stop();

			return;
		}

		itSound = _FindSound( m_oWaitingList, _sSound );

		if( itSound != m_oWaitingList.end() )
		{
			if( itSound->m_iNbInstances > 0 )
				--itSound->m_iNbInstances;

			if( itSound->m_iNbInstances <= 0 )
				m_oWaitingList.erase( itSound );
		}
	}

	void AudioManager::Sound_PauseAll()
	{
		for( SoundInfo& oSound : m_oSounds )
		{
			if( oSound.m_pSound != nullptr && oSound.m_pSound->getStatus() == sf::Sound::Playing )
			{
				oSound.m_pSound->pause();
			}
		}
	}

	void AudioManager::Sound_ResumeAll()
	{
		for( SoundInfo& oSound : m_oSounds )
		{
			if( oSound.m_pSound != nullptr && oSound.m_pSound->getStatus() == sf::Sound::Paused )
			{
				oSound.m_pSound->play();
			}
		}
	}

	void AudioManager::Sound_StopAll()
	{
		for( SoundInfo& oSound : m_oSounds )
		{
			if( oSound.m_pSound != nullptr )
			{
				oSound.m_pSound->stop();
			}
		}
	}

	void AudioManager::Music_Play( const std::string& _sMusic, bool _bLoop, Audio::MusicCallbackPtr _pCallback, bool _bDeleteCallback )
	{
		if( m_oMusic.m_sName == _sMusic && m_oMusic.m_pMusic != nullptr )
		{
			m_oMusic.m_pMusic->play();
			return;
		}

		if( m_oMusic.m_pMusic != nullptr && m_oMusic.m_pMusic->getStatus() == sf::Music::Playing )
			m_oMusic.m_pMusic->stop();
		
		m_oMusic.m_pMusic			= nullptr;
		m_oMusic.m_sName			= _sMusic;
		m_oMusic.m_pCallback		= _pCallback;
		m_oMusic.m_bDeleteCallback	= _bDeleteCallback;

		m_oMusic.m_pMusic = g_pFZN_DataMgr->GetSfMusic( _sMusic );

		if( m_oMusic.m_pMusic != nullptr )
		{
			m_oMusic.m_pMusic->setLoop( _bLoop );
			m_oMusic.m_pMusic->setVolume( m_fMusicVolume );

			m_oMusic.m_pMusic->stop();
			m_oMusic.m_pMusic->play();
		}
	}

	void AudioManager::Music_Pause()
	{
		if( m_oMusic.m_pMusic == nullptr )
			return;

		m_oMusic.m_pMusic->pause();
	}

	void AudioManager::Music_Resume()
	{
		if( m_oMusic.m_pMusic == nullptr || m_oMusic.m_pMusic->getStatus() == sf::Music::Playing )
			return;

		m_oMusic.m_pMusic->play();
	}

	void AudioManager::Music_Stop()
	{
		if( m_oMusic.m_pMusic == nullptr )
			return;

		m_oMusic.m_pMusic->stop();
	}

	/////////////////ACCESSOR / MUTATOR/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on the number of the drivers of the system
	//Return value : Number of drivers
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	int AudioManager::GetDriversNumber()
	{
		int iDriverCount = 0;

		m_system->getNumDrivers(&iDriverCount);

		return iDriverCount;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on the audio system of the manager
	//Return value : Audio system
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	FMOD::System* AudioManager::GetAudioSystem()
	{
		return m_system;
	}

	bool AudioManager::IsUsingFMOD() const
	{
		return m_bUseFMOD;
	}

	bool AudioManager::IsSoundValid( const std::string& _sSound ) const
	{
		if( _sSound.empty() )
			return false;

		Sounds::const_iterator itSound = m_oSoundPool.find( _sSound );

		if( itSound != m_oSoundPool.end() && itSound->second.empty() == false )
		{
			for( const std::string& sSound : itSound->second )
			{
				if( g_pFZN_DataMgr->ResourceExists( fzn::DataManager::ResourceType::eSound, sSound ) == false )
					return false;
			}

			return true;
		}

		return g_pFZN_DataMgr->ResourceExists( fzn::DataManager::ResourceType::eSound, _sSound );
	}

	void AudioManager::SetSoundsVolume( float _fVolume )
	{
		m_fSoundsVolume = fzn::Math::Clamp( _fVolume, 0.f, 100.f );
	}

	float AudioManager::GetSoundsVolume() const
	{
		return m_fSoundsVolume;
	}

	void AudioManager::SetMusicVolume( float _fVolume )
	{
		m_fMusicVolume = fzn::Math::Clamp( _fVolume, 0.f, 100.f );
	}

	float AudioManager::GetMusicVolume() const
	{
		return m_fMusicVolume;
	}

	void AudioManager::_LoadSoundPoolFromXML()
	{
		tinyxml2::XMLDocument resFile;

		if( g_pFZN_DataMgr->LoadXMLFile( resFile, DATAPATH( SOUNDS_FILE_NAME ) ) )
		{
			FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "Failure : %s.", resFile.ErrorStr() );
			return;
		}

		tinyxml2::XMLElement* pSoundList = resFile.FirstChildElement( "Sounds" );

		if( pSoundList == nullptr )
		{
			FZN_COLOR_LOG( fzn::DBG_MSG_COL_RED, "Failure : \"Sounds\" tag not found." );
			return;
		}

		tinyxml2::XMLElement* pSound = pSoundList->FirstChildElement( "Sound" );

		while( pSound != nullptr )
		{
			std::string sSoundName = fzn::Tools::XMLStringAttribute( pSound, "Name" );

			tinyxml2::XMLElement* pSoundBuffer = pSound->FirstChildElement( "SoundBuffer" );

			while( pSoundBuffer != nullptr )
			{
				m_oSoundPool[ sSoundName ].push_back( fzn::Tools::XMLStringAttribute( pSoundBuffer, "Name" ) );
				pSoundBuffer = pSoundBuffer->NextSiblingElement();
			}

			pSound = pSound->NextSiblingElement();
		}
	}

	void AudioManager::_CheckWaitingList()
	{
		for( const SoundInfo& oSound : m_oWaitingList )
		{
			_PlaySound( oSound );
		}

		m_oWaitingList.clear();
	}

	void AudioManager::_PlaySound( const SoundInfo& _oSound )
	{
		std::string sSound = "";
		Sounds::iterator itSound = m_oSoundPool.find( _oSound.m_sName );

		if( itSound != m_oSoundPool.end() )
		{
			if( itSound->second.empty() )
				return;

			const int iRandomSoundBuffer = Rand( 0, itSound->second.size() );
			sSound = itSound->second[ iRandomSoundBuffer ];
		}
		else
			sSound = _oSound.m_sName;

		SoundInfo* pSound = nullptr;

		if( m_oAvailableSounds.empty() == false )
		{
			m_oSounds.push_back( m_oAvailableSounds.back() );
			pSound = &m_oSounds.back();
			m_oAvailableSounds.pop_back();
		}
		else if( m_oSounds.size() < 100 )
		{
			m_oSounds.push_back( SoundInfo() );
			pSound = &m_oSounds.back();
			pSound->m_pSound = new sf::Sound();
		}

		if( pSound != nullptr )
		{
			pSound->m_sName = sSound;

			sf::SoundBuffer* pSoundBuffer = g_pFZN_DataMgr->GetSoundBuffer( pSound->m_sName );

			if( pSoundBuffer == nullptr )
			{
				pSound->m_pSound->stop();	// Stopping the sound for good measure and leaving it for the update to put it in the available sounds vector.
				return;
			}

			pSound->m_pSound->setBuffer( *pSoundBuffer );
			pSound->m_pSound->setVolume( m_fSoundsVolume );
			pSound->m_pSound->setLoop( _oSound.m_bLoop );
			pSound->m_pSound->play();

			if( _oSound.m_bLoop )
				pSound->m_iNbInstances += _oSound.m_iNbInstances;
		}
	}

	fzn::AudioManager::SoundInstances::iterator AudioManager::_FindSound( SoundInstances& _oVector, const std::string& _sSound ) const
	{
		return std::find_if( _oVector.begin(), _oVector.end(), [ _sSound ]( const SoundInfo& oSound )
			{
				return oSound.m_sName == _sSound;
			} );
	}

}
