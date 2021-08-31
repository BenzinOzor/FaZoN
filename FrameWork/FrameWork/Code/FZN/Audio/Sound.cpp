//------------------------------------------------------------------------
/// Author : Philippe OFFERMANN
/// Date : 28.04.15
/// Description : Custom sound class
//------------------------------------------------------------------------

#include "FZN/Includes.h"
#include "FZN/Audio/AudioObject.h"
#include "FZN/Managers/AudioManager.h"
#include <Fmod/fmod.hpp>
#include "FZN/Audio/Sound.h"


namespace fzn
{
	//-------------------------------------------------------------------------------------------------
	/// Default constructor.
	//-------------------------------------------------------------------------------------------------
	Sound::Sound() : AudioObject()
	{
	}

	//-------------------------------------------------------------------------------------------------
	/// Copy constructor.
	/// @param	_sound	: The sound to copy.
	//-------------------------------------------------------------------------------------------------
	Sound::Sound(const Sound& _sound) : AudioObject(_sound)
	{
		char* test;
		FMOD_RESULT result = _sound.m_sound->getUserData((void**)&test);
		LoadFromFile(test);
	}

	//-------------------------------------------------------------------------------------------------
	/// Assignment operator.
	/// @param	_sound	: The sound to copy.
	/// @return	A copy of the given Sound.
	//-------------------------------------------------------------------------------------------------
	Sound& Sound::operator=(const Sound& _sound)
	{
		m_status = _sound.m_status;
		m_fTime = 0.f;
		char* test;
		FMOD_RESULT result = _sound.m_sound->getUserData((void**)&test);
		LoadFromFile(test);
		return *this;
	}

	//-------------------------------------------------------------------------------------------------
	/// Destructor.
	//-------------------------------------------------------------------------------------------------
	Sound::~Sound()
	{
		if(m_sound != nullptr)
			m_sound->release();
	}


	//-------------------------------------------------------------------------------------------------
	/// Loads the sound from a given file.
	/// @param	_path	: Path to the ogg file.
	//-------------------------------------------------------------------------------------------------
	void Sound::LoadFromFile(const std::string& _path)
	{
		g_pFZN_AudioMgr->GetAudioSystem()->createSound(_path.c_str(), FMOD_DEFAULT, nullptr, &m_sound);
		m_sound->getLength(&m_length, FMOD_TIMEUNIT_MS);
		const char* test = _path.c_str();
		FMOD_RESULT result = m_sound->setUserData((void*)test);
	}
} //namespace fzn
