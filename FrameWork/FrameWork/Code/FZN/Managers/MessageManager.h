//------------------------------------------------------------------------
//Author : Philippe OFFERMANN
//Date : 18.10.14
//Description : Message delivering management (declarations)
//Portions Copyright (C) Steve Rabin, 2001
//------------------------------------------------------------------------

#ifndef _MSGROUTE_H_
#define _MSGROUTE_H_

#define NoSender -1						//Sender of the message is nobody

#include "FZN/DataStructure/Vector.h"

namespace fzn
{
	class Message;

	class FZN_EXPORT MessageManager
	{
	public:
		enum MSG_Scope
		{
			NO_MSG_SCOPING,
			SCOPE_TO_THIS_STATE
		} ;

		/////////////////CONSTRUCTOR / DESTRUCTOR/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Default constructor
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		MessageManager();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Default destructor
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		~MessageManager();


		/////////////////OTHER FUNCTIONS/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Manages messages contained in the delayed list
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void Update();


		/////////////////MESSAGE MANAGEMENT/////////////////

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Deliver messages with delay
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void DeliverDelayedMessage();
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Sends a message 
		//Parameter 1 : Time of the delivery (0 if instant)
		//Parameter 2 : Message type
		//Parameter 3 and 4 : Sender and receiver ID
		//Parameter 5 : Data to send
		//Parameter 6 : State in which the receiver is allowed to get the message (-1 for any)
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void Send( float _delay, int _type, int _sender, int _receiver, void* _data = nullptr, int _state = -1 );

	private:
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Message delivering
		//Parameter : Message to deliver
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void RouteMsg( Message* _message );


		/////////////////MEMBER VARIABLES/////////////////

		Vector<Message*> m_delayedMessages;					//Delayed messages storage
		int m_iNbMessages	;									//Number of messages in the delivery list
		sf::Clock m_deliveryTimer;								//Message delivering timer
	};
} //namespace fzn

extern FZN_EXPORT fzn::MessageManager* g_pFZN_MessageMgr;

#endif //_MSGROUTE_H_