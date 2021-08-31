#include "FWW/FWWIncludes.h"

#include <FZN/Audio/Sound.h>

#include "FWW/Tools/FWWTools.h"
#include "FWW/Audio/FWWSound.h"


namespace wzn
{
	FWWSound::FWWSound()
	{
		m_pSound = new fzn::Sound();
	}

	FWWSound::FWWSound( const FWWSound^ _pSound )
	{
		CheckNullptr( _pSound );

		if( m_pSound != nullptr )
			delete m_pSound;

		m_pSound = new fzn::Sound( *_pSound->m_pSound );
	}

	FWWSound::FWWSound( fzn::Sound* _pUnmanagedSound )
	{
		m_pSound = _pUnmanagedSound;
	}

	FWWSound^ FWWSound::operator=( const FWWSound^ _pSound )
	{
		CheckNullptrReturn( _pSound, nullptr );

		if( m_pSound != nullptr )
			delete m_pSound;

		m_pSound = new fzn::Sound( *_pSound->m_pSound );

		return this;
	}

	FWWSound::~FWWSound()
	{
		if( m_pSound != nullptr )
		{
			delete m_pSound;
			m_pSound = nullptr;
		}
	}


	void FWWSound::LoadFromFile( System::String^ _sPath )
	{
		CheckNullptr( m_pSound );
		std::string sStdPath = FWWTools::ToCharPtr( _sPath );
		m_pSound->LoadFromFile( sStdPath );
	}

	void FWWSound::Play()
	{
		CheckNullptr( m_pSound );
		m_pSound->Play();
	}

	void FWWSound::Pause()
	{
		CheckNullptr( m_pSound );
		m_pSound->Pause();
	}

	void FWWSound::Resume()
	{
		CheckNullptr( m_pSound );
		m_pSound->Resume();
	}

	void FWWSound::Stop()
	{
		CheckNullptr( m_pSound );
		//m_pSound->Stopped();
	}

	//-------------------------------------------------------------------------------------------------
	/// Properties
	//-------------------------------------------------------------------------------------------------
	FWWSound::Status FWWSound::m_eStatus::get()
	{
		CheckNullptrReturn( m_pSound, FWWSound::Status::Stopped );
		return (FWWSound::Status)m_pSound->GetStatus();
	}

	UINT32 FWWSound::m_uLength::get()
	{
		CheckNullptrReturn( m_pSound, 0 );
		return m_pSound->GetLength();
	}

	int FWWSound::m_iProgression::get()
	{
		CheckNullptrReturn( m_pSound, 0 );
		return m_pSound->GetProgression();
	}
	
	bool FWWSound::m_bIsPlaying::get()
	{
		CheckNullptrReturn( m_pSound, false );
		return m_pSound->IsPlaying();
	}

	bool FWWSound::m_bIsPaused::get()
	{
		CheckNullptrReturn( m_pSound, false );
		return m_pSound->IsPaused();
	}

	bool FWWSound::m_bIsStopped::get()
	{
		CheckNullptrReturn( m_pSound, false );
		return m_pSound->IsStopped();
	}

	fzn::Sound* FWWSound::m_pUnmanagedSound::get()
	{
		return m_pSound;
	}

	void FWWSound::m_pUnmanagedSound::set( fzn::Sound* _pSound )
	{
		if( m_pSound != nullptr )
			delete m_pSound;

		m_pSound = _pSound;
	}
}
