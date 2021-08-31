#ifndef __FWW_SOUND_H__
#define __FWW_SOUND_H__

#pragma make_public(fzn::Sound)

namespace fzn { class Sound; }
//#include <Audio/Sound/Sound.h>

namespace wzn
{
	public ref class FWWSound
	{
	public:
		enum class Status
		{
			Playing,
			Paused,
			Stopped
		};

		FWWSound();
		FWWSound( const FWWSound^ _pSound );
		FWWSound( fzn::Sound* _pUnmanagedSound );
		FWWSound^ operator=( const FWWSound^ _pSound );
		~FWWSound();
		
		void LoadFromFile( System::String^ _sPath );
		void Play();
		void Pause();
		void Resume();
		void Stop();

		//-------------------------------------------------------------------------------------------------
		/// Properties
		//-------------------------------------------------------------------------------------------------
		property Status			m_eStatus			{ Status		get();								}
		property UINT32			m_uLength			{ UINT32		get();								}
		property int			m_iProgression		{ int			get();								}
		property bool			m_bIsPlaying		{ bool			get();								}
		property bool			m_bIsPaused			{ bool			get();								}
		property bool			m_bIsStopped		{ bool			get();								}
		property fzn::Sound*	m_pUnmanagedSound	{ fzn::Sound*	get();	void set( fzn::Sound* );	}

	private:
		fzn::Sound* m_pSound;
	};
}

#endif //__FWW_SOUND_H__
