//------------------------------------------------------------------------
//Author : Philippe OFFERMANN
//Date : 17.10.14
//Description : Messages management (declarations)
//Portions Copyright (C) Steve Rabin, 2001
//------------------------------------------------------------------------

#ifndef _MESSAGE_H_
#define _MESSAGE_H_


namespace fzn
{
	class FZN_EXPORT Message
	{
	public:
		/////////////////CONSTRUCTOR / DESTRUCTOR/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Default constructor
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		Message();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Parametered constructor
		//Parameter 1 : Time of the delivery (0 if instant)
		//Parameter 2 : Message type
		//Parameter 3 and 4 : Sender and receiver ID
		//Parameter 5 : Data to send
		//Parameter 6 : State in which the receiver is allowed to get the message (-1 for any)
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		Message( float _delay, int _type, int _Sender, int _Receiver, void* _data = nullptr, int state = -1 );
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Default destructor
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		~Message();

		/////////////////MEMBER VARIABLES/////////////////

		int m_type;							//Message name
		int m_sender;						//Object that sent the message
		int m_receiver;						//Object that will get the message
		int m_iState;						//State in which the receiver is allowed get the message (-1 means any state)
		INT8 m_bDelivered;					//Whether the message has been delivered
		float m_fDeliveryTime;				//Time at which to send the message
		void* m_data;						//Datas contained in the message
	};
} //namespace fzn

#endif //_MESSAGE_H_