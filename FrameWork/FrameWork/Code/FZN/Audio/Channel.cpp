//------------------------------------------------------------------------
//Author : Philippe OFFERMANN
//Date : 29.04.15
//Description : Custom channel class
//------------------------------------------------------------------------

#include "FZN/Includes.h"
#include "FZN/Audio/AudioObject.h"
#include <Fmod/fmod.hpp>
#include "FZN/Audio/Channel.h"


namespace fzn
{
	//-------------------------------------------------------------------------------------------------
	/// Default constructor.
	//-------------------------------------------------------------------------------------------------
	Channel::Channel()
	{
		m_channel = nullptr;
		m_index = 0;
		m_isPlaying = false;
		m_sound = nullptr;
	}

	//-------------------------------------------------------------------------------------------------
	/// Destructor.
	//-------------------------------------------------------------------------------------------------
	Channel::~Channel()
	{
		m_sound = nullptr;
	}


	//-------------------------------------------------------------------------------------------------
	/// Updates channel state and sound if there is one.
	//-------------------------------------------------------------------------------------------------
	void Channel::Update()
	{
		if( m_channel != nullptr )
		{
			bool bChannelPlaying = false;
			
			m_channel->isPlaying( &bChannelPlaying );
			m_isPlaying = (INT8)bChannelPlaying;

			if( m_isPlaying && m_sound != nullptr )
			{
				m_sound->Update();
				if( m_sound->IsStopped() )
					m_sound = nullptr;
			}
		}
	}


	//-------------------------------------------------------------------------------------------------
	/// Plays the Channel.
	//-------------------------------------------------------------------------------------------------
	void Channel::Play()
	{
		m_channel->setPaused(false);
	}

	//-------------------------------------------------------------------------------------------------
	/// Pauses the Channel.
	//-------------------------------------------------------------------------------------------------
	void Channel::Pause()
	{
		m_channel->setPaused(true);
	}

	void Channel::Stop()
	{
		m_channel->stop();
	}

	//-------------------------------------------------------------------------------------------------
	/// Indicates whether or not the Channel is available to take a sound to play.
	/// @return	If true, the Channel is available.
	//-------------------------------------------------------------------------------------------------
	bool Channel::IsChannelAvailable()
	{
		bool bChannelPlaying = false;
		m_channel->isPlaying(&bChannelPlaying);
		m_isPlaying = (INT8)bChannelPlaying;
		return (!m_isPlaying && m_sound == nullptr);
	}


	//-------------------------------------------------------------------------------------------------
	/// Sets channel component.
	/// @param [in]	_channel	: The channel to set.
	//-------------------------------------------------------------------------------------------------
	void Channel::SetChannelComponent(FMOD::Channel& _channel)
	{
		m_channel = &_channel;
		m_channel->getIndex(&m_index);
	}

	//-------------------------------------------------------------------------------------------------
	/// Gets the index of the Channel.
	/// @return	The Channel index.
	//-------------------------------------------------------------------------------------------------
	int Channel::GetIndex()
	{
		return m_index;
	}

	//-------------------------------------------------------------------------------------------------
	/// Indicates whether or not the Channel is playing.
	/// @return	If true, th Channel is playing a sound.
	//-------------------------------------------------------------------------------------------------
	bool Channel::IsPlaying()
	{
		bool bIsPlaying = false;
		m_channel->isPlaying(&bIsPlaying);
		m_isPlaying = (INT8)bIsPlaying;
		return m_isPlaying;
	}

	//-------------------------------------------------------------------------------------------------
	/// Sets the AudioObject of the Channel.
	/// @param [in]	_audioObject	: The AudioObject to put in the Channel
	//-------------------------------------------------------------------------------------------------
	void Channel::SetAudioObject(AudioObject* _audioObject)
	{
		m_sound = _audioObject;
	}
} //namespace fzn