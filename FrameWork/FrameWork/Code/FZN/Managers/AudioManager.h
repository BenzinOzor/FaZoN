//------------------------------------------------------------------------
//Author : Philippe OFFERMANN
//Date : 28.04.15
//Description : Audio system manager
//------------------------------------------------------------------------

#ifndef _AUDIOMANAGER_H_
#define _AUDIOMANAGER_H_

#include <unordered_map>
#include <vector>

#include <Fmod/fmod.hpp>
#include "FZN/Audio/AudioObject.h"
#include "FZN/Audio/Channel.h"

#define ChannelsNumber 32

namespace sf
{
	class Sound;
};
/*namespace FMOD {
	class ChannelGroup;
	class System;
}*/

namespace fzn
{
	class FZN_EXPORT AudioManager
	{
	public:
		struct SoundInfo
		{
			sf::Sound* m_pSound = nullptr;
			std::string m_sName = "";
			bool		m_bLoop = false;
			int			m_iNbInstances = 0;
		};

		/////////////////CONSTRUCTOR / DESTRUCTOR/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Default constructor
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		AudioManager( bool _bUseFMOD = false );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Default destructor
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		~AudioManager();


		/////////////////MANAGEMENT FUNCTIONS/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Updates the channels
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void			Update();


		/////////////////SOUND FUNCTIONS/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Plays an AudioObject on the right channel
		//Parameter 1 : AudioObject to play
		//Parameter 2 : Channel group
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void			Sound_Play( AudioObject& _audioObject, FMOD::ChannelGroup* _channelGroup = FMOD_DEFAULT );
		void			Sound_Play( const std::string& _sSound, bool _bOnlyOne = false, bool _bLoop = false );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Pauses an AudioObject on the right channel
		//Parameter : AudioObject to pause
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void			Sound_Pause( AudioObject& _audioObject );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Resumes an AudioObject on the right channel
		//Parameter : AudioObject to resume
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void			Sound_Resume( AudioObject& _audioObject );
		void			Sound_Stop( AudioObject& _audioObject );
		void			Sound_Stop( const std::string& _sSound );

		void			Sound_PauseAll();
		void			Sound_ResumeAll();
		void			Sound_StopAll();

		/////////////////ACCESSOR / MUTATOR/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on the number of the drivers of the system
		//Return value : Number of drivers
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		int				GetDriversNumber();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on the audio system of the manager
		//Return value : Audio system
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		FMOD::System*	GetAudioSystem();
		bool			IsUsingFMOD() const;
		bool			IsSoundValid( const std::string& _sSound ) const;
		void			SetSoundsVolume( float _fVolume );
		float			GetSoundsVolume() const;
		void			SetMusicVolume( float _fVolume );
		float			GetMusicVolume() const;

	protected:
		typedef std::unordered_map< std::string, std::vector< std::string > >	Sounds;
		typedef std::vector< SoundInfo > SoundInstances;

		void			_LoadSoundPoolFromXML();

		void			_CheckWaitingList();
		void			_PlaySound( const SoundInfo& _oSound );

		bool			m_bUseFMOD;

		float			m_fSoundsVolume;
		float			m_fMusicVolume;

		Sounds			m_oSoundPool;
		SoundInstances	m_oAvailableSounds;
		SoundInstances	m_oSounds;
		SoundInstances	m_oWaitingList;

		FMOD::Channel*	m_channel;
		FMOD::System*	m_system;
		Channel			m_channels[ ChannelsNumber ];
	};
} //namespace fzn

extern FZN_EXPORT fzn::AudioManager* g_pFZN_AudioMgr;

#endif //_AUDIOMANAGER_H_
