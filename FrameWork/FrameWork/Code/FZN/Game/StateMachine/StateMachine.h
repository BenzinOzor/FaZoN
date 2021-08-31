//------------------------------------------------------------------------
//Author : Philippe OFFERMANN
//Date : 17.10.14
//Description : State machine management (declarations)
//Portions Copyright (C) Steve Rabin, 2001
//------------------------------------------------------------------------

#ifndef _STATEMACHINE_H_
#define _STATEMACHINE_H_

//State Machine Language Macros (put these keywords in the file USERTYPE.DAT in the same directory as MSDEV.EXE)
/*#define BeginStateMachine		if( _state < 0 ) { if(0) {
#define EndStateMachine			return( true ); } } else { return( false ); } return( false );
#define State(a)				return( true ); } } else if( a == _state ) { if(0) { 
#define OnMsg(a)				return( true ); } else if( fzn::EVENT_Message == _event && _msg && a == _msg->m_type ) { 
#define OnStateEvent(a)				return( true ); } else if( a == _event ) { 
#define OnUpdate				OnStateEvent( fzn::EVENT_Update )
#define OnEnter					OnStateEvent( fzn::EVENT_Enter )
#define OnExit					OnStateEvent( fzn::EVENT_Exit )*/

namespace fzn
{
	class Message;

	class FZN_EXPORT StateMachine
	{
	public:
		//Possible events for a state machine to process
		enum StateMachineEvent
		{
			EVENT_INVALID,
			EVENT_Update,
			EVENT_Message,
			EVENT_Enter,
			EVENT_Exit
		};

		/////////////////CONSTRUCTOR / DESTRUCTOR/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Default constructor
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		StateMachine();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Parametered constructor
		//Parameter : GameObject owner of the state machine
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		StateMachine( GameObject* _owner );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Default destructor
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		~StateMachine();


		/////////////////OTHER FUNCTIONS/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//State machine initialization
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void Initialize();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//State machine update
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void Update();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//State machine update
		//Parameter 1 : State to process
		//Parameter 2 : Message to process
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void Process( StateMachineEvent _state, Message* _msg = nullptr );


		/////////////////ACCESSORS / MUTATORS/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Mutator on the owner of the state machine
		//Parameter : New owner
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void SetOwner( GameObject* _newOwner );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Mutator on the current state of the machine
		//Parameter : New state
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void SetState( unsigned int _newState );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Accessor on the current state of the machine
		//Return value : Current state
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		int GetState();

	protected:
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//FSM Processing
		//Parameter 1 : State to process
		//Parameter 2 : Message to process
		//Parameter 3 : State to process
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		virtual INT8 States( StateMachineEvent _event, Message* _message, int _state ) = 0;

		/////////////////MEMBER VARIABLES/////////////////

		GameObject* m_owner;							//Owner of the state machine
		unsigned int m_iCurrentState;						//Current state of the state machine
		unsigned int m_iNextState;							//Next state of the state machine
		INT8 m_bStateChange;								//Indicate the need to change the current state

	};
} //namespace fzn

//------------------------------------------------------------------------------------------------------------------------------------------------------------------
//States definition template using the defines at the start of the file
//------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*INT8 StateMachine::States(fzn::StateMachineEvent _event, fzn::Message* _msg, int _state)
{
	BeginStateMachine

		/////////////////STATE 1/////////////////
		State(State1)
			OnEnter

			OnUpdate

			OnExit

	EndStateMachine
}*/

#endif //_STATEMACHINE_H_
