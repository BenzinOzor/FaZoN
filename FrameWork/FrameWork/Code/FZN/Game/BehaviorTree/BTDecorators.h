//------------------------------------------------------------------------
/// Author : Philippe OFFERMANN
/// Date : /
/// Description : Behavior tree base decorator classes
//------------------------------------------------------------------------

#ifndef _BTDECORATORS_H_
#define _BTDECORATORS_H_


namespace fzn
{
	class FZN_EXPORT Repeat : public Decorator
	{
	public :
		//-------------------------------------------------------------------------------------------------
		/// Constructor.
		/// @param [in]	_child	: BTElement child of the Repeat.
		//-------------------------------------------------------------------------------------------------
		Repeat(BTElement* _child) : Decorator(_child){}
		//-------------------------------------------------------------------------------------------------
		/// Destructor.
		//-------------------------------------------------------------------------------------------------
		virtual ~Repeat(){}

		//-------------------------------------------------------------------------------------------------
		/// Sets a count.
		/// @param	_count	: Number of.
		//-------------------------------------------------------------------------------------------------
		void SetCount(int _count);

		//-------------------------------------------------------------------------------------------------
		/// Executes the initialize action.
		//-------------------------------------------------------------------------------------------------
		void OnInitialize();
		//-------------------------------------------------------------------------------------------------
		/// Updates this Repeat.
		/// @return	The Status.
		//-------------------------------------------------------------------------------------------------
		State Update();

	protected :
		int m_iMaxCount;
		int m_iCount;
	};

	class FZN_EXPORT Timer : public Decorator
	{
	public :
		//-------------------------------------------------------------------------------------------------
		/// Constructor.
		/// @param [in,out]	_child	If non-null, the child.
		//-------------------------------------------------------------------------------------------------
		Timer(BTElement* _child) : Decorator(_child){}
		//-------------------------------------------------------------------------------------------------
		/// Destructor.
		//-------------------------------------------------------------------------------------------------
		virtual ~Timer(){}

		//-------------------------------------------------------------------------------------------------
		/// Sets time limit.
		/// @param	_seconds	The seconds.
		//-------------------------------------------------------------------------------------------------
		void SetTimeLimit(DWORD _seconds);

		//-------------------------------------------------------------------------------------------------
		/// Executes the initialize action.
		//-------------------------------------------------------------------------------------------------
		void OnInitialize();
		//-------------------------------------------------------------------------------------------------
		/// Updates this Timer.
		/// @return	The Status.
		//-------------------------------------------------------------------------------------------------
		State Update();

	protected :
		DWORD m_initialTime;
		DWORD m_currentTime;
		DWORD m_maxTime;
	};

	class FZN_EXPORT Delayer : public Decorator
	{
	public :
		//-------------------------------------------------------------------------------------------------
		/// Constructor.
		/// @param [in,out]	_child	If non-null, the child.
		//-------------------------------------------------------------------------------------------------
		Delayer(BTElement* _child) : Decorator(_child){}
		//-------------------------------------------------------------------------------------------------
		/// Destructor.
		//-------------------------------------------------------------------------------------------------
		virtual ~Delayer(){}

		//-------------------------------------------------------------------------------------------------
		/// Sets target time.
		/// @param	_seconds	The seconds.
		//-------------------------------------------------------------------------------------------------
		void SetTargetTime(DWORD _seconds);

		//-------------------------------------------------------------------------------------------------
		/// Executes the initialize action.
		//-------------------------------------------------------------------------------------------------
		void OnInitialize();
		//-------------------------------------------------------------------------------------------------
		/// Updates this Delayer.
		/// @return	The Status.
		//-------------------------------------------------------------------------------------------------
		State Update();

	protected:
		DWORD m_initialTime;
		DWORD m_currentTime;
		DWORD m_targetTime;
	};

	class FZN_EXPORT Reverse : public Decorator
	{
	public :
		//-------------------------------------------------------------------------------------------------
		/// Constructor.
		/// @param [in,out]	_child	If non-null, the child.
		//-------------------------------------------------------------------------------------------------
		Reverse(BTElement* _child) : Decorator(_child){}
		//-------------------------------------------------------------------------------------------------
		/// Destructor.
		//-------------------------------------------------------------------------------------------------
		virtual ~Reverse(){}

		//-------------------------------------------------------------------------------------------------
		/// Updates this Reverse.
		/// @return	The Status.
		//-------------------------------------------------------------------------------------------------
		State Update();
	};
} //namespace fzn

#endif //_BTDECORATORS_H_