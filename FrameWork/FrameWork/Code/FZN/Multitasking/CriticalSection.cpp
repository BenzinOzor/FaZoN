//------------------------------------------------------------------------
//Author : Philippe OFFERMANN
//Date : 18.01.15
//Description : Custom critical section class
//------------------------------------------------------------------------

#include "FZN/Includes.h"
#include "FZN/Multitasking/CriticalSection.h"


namespace fzn
{
	/////////////////CONSTRUCTORS / DESTRUCTORS/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Default constructor
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	CriticalSection::CriticalSection()
	{
		InitializeCriticalSection( &m_criticalSection );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Destructor
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	CriticalSection::~CriticalSection()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Enters the critical section
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void CriticalSection::Enter()
	{
		EnterCriticalSection( &m_criticalSection );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Leaves the critical section
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void CriticalSection::Leave()
	{
		LeaveCriticalSection( &m_criticalSection );
	}
} //namespace fzn
