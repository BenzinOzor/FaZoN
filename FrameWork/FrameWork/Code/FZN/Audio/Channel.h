//------------------------------------------------------------------------
//Author : Philippe OFFERMANN
//Date : 29.04.15
//Description : Custom channel class
//------------------------------------------------------------------------

#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include "FZN/Defines.h"


namespace FMOD { class Channel; }

namespace fzn
{

	class FZN_EXPORT Channel
	{
	public:
		//-------------------------------------------------------------------------------------------------
		/// Default constructor.
		//-------------------------------------------------------------------------------------------------
		Channel();
		//-------------------------------------------------------------------------------------------------
		/// Destructor.
		//-------------------------------------------------------------------------------------------------
		~Channel();

		//-------------------------------------------------------------------------------------------------
		/// Updates Channel state and sound if there is one.
		//-------------------------------------------------------------------------------------------------
		void Update();

		//-------------------------------------------------------------------------------------------------
		/// Plays the Channel.
		//-------------------------------------------------------------------------------------------------
		void Play();
		//-------------------------------------------------------------------------------------------------
		/// Pauses the Channel.
		//-------------------------------------------------------------------------------------------------
		void Pause();
		void Stop();
		//-------------------------------------------------------------------------------------------------
		/// Indicates whether or not the Channel is available to take a sound to play.
		/// @return	If true, the Channel is available.
		//-------------------------------------------------------------------------------------------------
		bool IsChannelAvailable();
		//-------------------------------------------------------------------------------------------------
		/// Sets channel component.
		/// @param [in]	_channel	: The channel to set.
		//-------------------------------------------------------------------------------------------------
		void SetChannelComponent(FMOD::Channel& _channel);
		//-------------------------------------------------------------------------------------------------
		/// Gets the index of the Channel.
		/// @return	The Channel index.
		//-------------------------------------------------------------------------------------------------
		int GetIndex();
		//-------------------------------------------------------------------------------------------------
		/// Indicates whether or not the Channel is playing.
		/// @return	If true, th Channel is playing a sound.
		//-------------------------------------------------------------------------------------------------
		bool IsPlaying();
		//-------------------------------------------------------------------------------------------------
		/// Sets the AudioObject of the Channel.
		/// @param [in]	_audioObject	: The AudioObject to put in the Channel
		//-------------------------------------------------------------------------------------------------
		void SetAudioObject(AudioObject* _audioObject);

	private:
		FMOD::Channel* m_channel;			//FMOD Channel component
		int m_index;							//Channel index in the system
		bool m_isPlaying;						//Indicates if the channel is currently playing a sound (true) or not
		AudioObject* m_sound;				//Audio object currently playing in the channel
	};
} //namespace fzn

#endif //_CHANNEL_H_