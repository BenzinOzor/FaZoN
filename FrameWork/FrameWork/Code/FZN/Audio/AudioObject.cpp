//------------------------------------------------------------------------
/// Author : Philippe OFFERMANN
/// Date : 01.05.15
/// Description : Custom audio object class
//------------------------------------------------------------------------

#include "FZN/Includes.h"
#include "FZN/Managers/WindowManager.h"
#include "FZN/Managers/AudioManager.h"
#include <Fmod/fmod.hpp>
#include "FZN/Audio/AudioObject.h"


namespace fzn
{
	//-------------------------------------------------------------------------------------------------
	/// Default constructor.
	//-------------------------------------------------------------------------------------------------
	AudioObject::AudioObject()
	{
		m_sound = nullptr;
		m_status = Status::Stopped;
		m_channelIndex = -1;
		m_fTime = 0.f;
		m_length = 0;
		m_bLoop = false;
	}

	//-------------------------------------------------------------------------------------------------
	/// Copy constructor.
	/// @param	_audioObject	: The AudioObject to copy.
	//-------------------------------------------------------------------------------------------------
	AudioObject::AudioObject(const AudioObject& _audioObject)
	{
		m_status = _audioObject.m_status;
		m_fTime = 0.f;
		m_bLoop = _audioObject.m_bLoop;
	}

	//-------------------------------------------------------------------------------------------------
	/// Destructor.
	//-------------------------------------------------------------------------------------------------
	AudioObject::~AudioObject()
	{
		/*if(m_sound != nullptr)
			m_sound->release();*/
	}


	//-------------------------------------------------------------------------------------------------
	/// Updates AudioObject timer and status
	//-------------------------------------------------------------------------------------------------
	void AudioObject::Update()
	{
		if(m_sound != nullptr && m_status == Status::Playing)
		{
			m_fTime += FrameTime * 1000.f;
			if( m_fTime > static_cast<float>( m_length ) )
			{
				m_status = Status::Stopped;
				
				if( m_bLoop )
					Play();
			}
		}
	}

	//-------------------------------------------------------------------------------------------------
	/// Plays the Sound.
	//-------------------------------------------------------------------------------------------------
	void AudioObject::Play()
	{
		if(m_status == Status::Playing)
			return;

		g_pFZN_AudioMgr->Sound_Play(*this);
		m_status = Status::Playing;
		m_fTime = 0.f;
	}

	//-------------------------------------------------------------------------------------------------
	/// Pauses the Sound.
	//-------------------------------------------------------------------------------------------------
	void AudioObject::Pause()
	{
		if(m_status == Status::Paused)
			return;

		g_pFZN_AudioMgr->Sound_Pause(*this);
		m_status = Status::Paused;
	}

	//-------------------------------------------------------------------------------------------------
	/// Resumes the Sound.
	//-------------------------------------------------------------------------------------------------
	void AudioObject::Resume()
	{
		if(m_status == Status::Playing)
			return;

		g_pFZN_AudioMgr->Sound_Resume(*this);
		m_status = Status::Playing;
	}

	void AudioObject::Stop()
	{
		if( m_status == Status::Stopped )
			return;

		g_pFZN_AudioMgr->Sound_Stop(*this);
		m_status = Status::Stopped;
	}

	//-------------------------------------------------------------------------------------------------
	/// Gets the Fmod sound component.
	/// @return	The sound component.
	//-------------------------------------------------------------------------------------------------
	FMOD::Sound* AudioObject::GetSoundComponent()
	{
		return m_sound;
	}

	//-------------------------------------------------------------------------------------------------
	/// Gets the current status of the AudioObject (Playing, Paused, Stopped).
	/// @return	The current status.
	//-------------------------------------------------------------------------------------------------
	fzn::AudioObject::Status AudioObject::GetStatus()
	{
		return m_status;
	}

	//-------------------------------------------------------------------------------------------------
	/// Gets the length of the AudioObject.
	/// @return	The length (ms).
	//-------------------------------------------------------------------------------------------------
	unsigned int AudioObject::GetLength()
	{
		if(m_sound != nullptr)
			m_sound->getLength(&m_length, FMOD_TIMEUNIT_MS);
		
		return m_length;
	}

	//-------------------------------------------------------------------------------------------------
	/// Gets the current progression of the AudioObject.
	/// @return	The progression (ms).
	//-------------------------------------------------------------------------------------------------
	float AudioObject::GetProgression()
{
		return m_fTime;
	}

	//-------------------------------------------------------------------------------------------------
	/// Indicates if the AudioObject is playing.
	/// @return	If true, the AudioObject is playing.
	//-------------------------------------------------------------------------------------------------
	bool AudioObject::IsPlaying()
	{
		return m_status == Status::Playing;
	}

	//-------------------------------------------------------------------------------------------------
	/// Indicates if the AudioObject is paused.
	/// @return	If true, the AudioObject is paused.
	//-------------------------------------------------------------------------------------------------
	bool AudioObject::IsPaused()
	{
		return m_status == Status::Paused;
	}

	//-------------------------------------------------------------------------------------------------
	/// Indicates if the AudioObject is stopped.
	/// @return	If true, the AudioObject is stopped.
	//-------------------------------------------------------------------------------------------------
	bool AudioObject::IsStopped()
	{
		return m_status == Status::Stopped;
	}

	void AudioObject::SetLoop( bool _bLoop )
	{
		m_bLoop = _bLoop;
	}

	bool AudioObject::GetLoop() const
	{
		return m_bLoop;
	}

} //namespace fzn
