//------------------------------------------------------------------------
/// Author : Philippe OFFERMANN
/// Date : 01.05.15
/// Description : Custom music class
//------------------------------------------------------------------------

#include "FZN/Includes.h"
#include "FZN/Audio/AudioObject.h"
#include "FZN/Managers/AudioManager.h"
#include <Fmod/fmod.hpp>
#include "FZN/Audio/Music.h"


namespace fzn
{
	//-------------------------------------------------------------------------------------------------
	/// Default constructor.
	//-------------------------------------------------------------------------------------------------
	Music::Music()
	{
		m_sound = nullptr;
		m_status = Status::Stopped;
		m_channelIndex = -1;
		m_fTime = 0;
		m_length = 0;
	}

	//-------------------------------------------------------------------------------------------------
	/// Copy constructor.
	/// @param	_music	: The music to copy.
	//-------------------------------------------------------------------------------------------------
	Music::Music(const Music& _music) : AudioObject(_music)
	{
		char* test;
		FMOD_RESULT result = _music.m_sound->getUserData((void**)&test);
		LoadFromFile(test);
	}

	//-------------------------------------------------------------------------------------------------
	/// Assignment operator.
	/// @param	_music	: The music to copy.
	/// @return	A copy of the given Music.
	//-------------------------------------------------------------------------------------------------
	Music& Music::operator=(const Music& _music)
	{
		m_status = _music.m_status;
		m_fTime = 0.f;
		char* test;
		FMOD_RESULT result = _music.m_sound->getUserData((void**)&test);
		LoadFromFile(test);
		return *this;
	}

	//-------------------------------------------------------------------------------------------------
	/// Destructor.
	//-------------------------------------------------------------------------------------------------
	Music::~Music()
	{
		if(m_sound != nullptr)
			m_sound->release();
	}


	//-------------------------------------------------------------------------------------------------
	/// Loads the Music from a given file.
	/// @param	_path	: Path to the ogg file.
	//-------------------------------------------------------------------------------------------------
	void Music::LoadFromFile(const std::string& _path)
	{
		g_pFZN_AudioMgr->GetAudioSystem()->createSound(_path.c_str(), FMOD_CREATESTREAM, nullptr, &m_sound);
		m_sound->getLength(&m_length, FMOD_TIMEUNIT_MS);
		const char* test = _path.c_str();
		FMOD_RESULT result = m_sound->setUserData((void*)test);
	}

} //namespace fzn
