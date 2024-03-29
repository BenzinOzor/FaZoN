//------------------------------------------------------------------------
/// Author : Philippe OFFERMANN
/// Date : 01.05.15
/// Description : Custom audio object class
//------------------------------------------------------------------------

#ifndef _AUDIOOBJECT_H_
#define _AUDIOOBJECT_H_

#include "FZN/Defines.h"


namespace FMOD { class Sound; }
namespace fzn
{
	class FZN_EXPORT AudioObject
	{
	public:
		//-------------------------------------------------------------------------------------------------
		/// Possible statuses of an AudioObject.
		//-------------------------------------------------------------------------------------------------
		enum Status
		{
			Playing,
			Paused,
			Stopped
		};

		//-------------------------------------------------------------------------------------------------
		/// Default constructor.
		//-------------------------------------------------------------------------------------------------
		AudioObject();
		//-------------------------------------------------------------------------------------------------
		/// Copy constructor.
		/// @param	_audioObject	: The AudioObject to copy.
		//-------------------------------------------------------------------------------------------------
		AudioObject(const AudioObject& _audioObject);
		//-------------------------------------------------------------------------------------------------
		/// Destructor.
		//-------------------------------------------------------------------------------------------------
		virtual ~AudioObject();

		//-------------------------------------------------------------------------------------------------
		/// Updates AudioObject timer and status
		//-------------------------------------------------------------------------------------------------
		void Update();

		//-------------------------------------------------------------------------------------------------
		/// Plays the Sound.
		//-------------------------------------------------------------------------------------------------
		void Play();
		//-------------------------------------------------------------------------------------------------
		/// Pauses the Sound.
		//-------------------------------------------------------------------------------------------------
		void Pause();
		//-------------------------------------------------------------------------------------------------
		/// Resumes the Sound.
		//-------------------------------------------------------------------------------------------------
		void Resume();
		//-------------------------------------------------------------------------------------------------
		/// Stops the Music.
		//-------------------------------------------------------------------------------------------------
		void Stop();

		//-------------------------------------------------------------------------------------------------
		/// Gets the Fmod sound component.
		/// @return	The sound component.
		//-------------------------------------------------------------------------------------------------
		FMOD::Sound* GetSoundComponent();
		//-------------------------------------------------------------------------------------------------
		/// Gets the current state of the AudioObject (Playing, Paused, Stopped).
		/// @return	The current state.
		//-------------------------------------------------------------------------------------------------
		Status GetStatus();
		//-------------------------------------------------------------------------------------------------
		/// Gets the length of the AudioObject.
		/// @return	The length (ms).
		//-------------------------------------------------------------------------------------------------
		unsigned int GetLength();
		//-------------------------------------------------------------------------------------------------
		/// Gets the current progression of the AudioObject.
		/// @return	The progression (ms).
		//-------------------------------------------------------------------------------------------------
		float GetProgression();
		//-------------------------------------------------------------------------------------------------
		/// Indicates if the AudioObject is playing.
		/// @return	If true, the AudioObject is playing.
		//-------------------------------------------------------------------------------------------------
		bool IsPlaying();
		//-------------------------------------------------------------------------------------------------
		/// Indicates if the AudioObject is paused.
		/// @return	If true, the AudioObject is paused.
		//-------------------------------------------------------------------------------------------------
		bool IsPaused();
		//-------------------------------------------------------------------------------------------------
		/// Indicates if the AudioObject is stopped.
		/// @return	If true, the AudioObject is stopped.
		//-------------------------------------------------------------------------------------------------
		bool IsStopped();

		void SetLoop( bool _bLoop );
		bool GetLoop() const;


		/////////////////MEMBER VARIABLES/////////////////

		int					m_channelIndex;

	protected:
		FMOD::Sound*		m_sound;				//FMOD sound component
		Status				m_status;				//Current status of the music (Playing, Paused, or Stopped)
		float				m_fTime;				//Timer of the music (ms)
		unsigned int		m_length;				//Length of the music (ms)
		bool				m_bLoop;
	};
} //namespace fzn

#endif //_AUDIOOBJECT_H_