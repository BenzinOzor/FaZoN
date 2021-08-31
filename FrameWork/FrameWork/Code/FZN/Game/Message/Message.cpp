//------------------------------------------------------------------------
//Author : Philippe OFFERMANN
//Date : 18.10.14
//Description : Messages management (definitions)
//Portions Copyright (C) Steve Rabin, 2001
//------------------------------------------------------------------------

#include "FZN/Includes.h"
#include "FZN/Game/GameObjectAI/GameObjectAI.h"
#include "FZN/Game/Message/Message.h"


namespace fzn
{
	/////////////////CONSTRUCTOR / DESTRUCTOR/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Default constructor
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	Message::Message()
	{
		m_type = -1;
		m_sender = INVALID_OBJECT_ID;
		m_receiver = INVALID_OBJECT_ID;
		m_iState = -1;
		m_fDeliveryTime = 0.f;
		m_bDelivered = false;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Parametered constructor
	//Parameter 1 : Time of the delivery (0 if instant)
	//Parameter 2 : Message type
	//Parameter 3 and 4 : Sender and receiver ID
	//Parameter 5 : Data to send
	//Parameter 6 : State in which the receiver is allowed to get the message (-1 for any)
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	Message::Message(float _fDeliveryTime, int _type, int _sender, int _receiver, void* _data, int _state)
	{
		m_fDeliveryTime = _fDeliveryTime;
		m_type = _type;
		m_sender = _sender;
		m_receiver = _receiver;
		m_data = _data;
		m_iState = _state;
		m_bDelivered = false;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Default destructor
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	Message::~Message()
	{
	}

}
