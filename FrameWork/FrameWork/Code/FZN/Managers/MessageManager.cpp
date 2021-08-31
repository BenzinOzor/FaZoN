//------------------------------------------------------------------------
//Author : Philippe OFFERMANN
//Date : 18.10.14
//Description : Message delivering management (definitions)
//Portions Copyright (C) Steve Rabin, 2001
//------------------------------------------------------------------------

#include "FZN/Includes.h"
#include "FZN/Game/Message/Message.h"
#include "FZN/Game/GameObjectAI/GameObjectAI.h"
#include "FZN/Game/StateMachine/StateMachine.h"
#include "FZN/Managers/AIManager.h"
#include "FZN/DataStructure/Vector.h"
#include "FZN/Managers/MessageManager.h"


FZN_EXPORT fzn::MessageManager* g_pFZN_MessageMgr = nullptr;

namespace fzn
{
	/////////////////CONSTRUCTOR / DESTRUCTOR/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Default constructor
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	MessageManager::MessageManager()
	{
		m_deliveryTimer.restart();
		m_iNbMessages = 0;

		g_pFZN_MessageMgr = this;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Default constructor
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	MessageManager::~MessageManager()
	{
		for( int i = 0 ; i < m_iNbMessages ; ++i )
		{
			if( m_delayedMessages[i] != nullptr )
			{
				delete m_delayedMessages[i];
				m_delayedMessages[i] = nullptr;
			}
		}

		m_delayedMessages.Clear();

		g_pFZN_MessageMgr = nullptr;
	}


	//=========================================================
	//==========================PUBLIC==========================
	//=========================================================

	/////////////////OTHER FUNCTIONS/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Manages messages contained in the delayed list
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void MessageManager::Update()
	{
		DeliverDelayedMessage();
	}


	/////////////////MESSAGE MANAGEMENT/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Deliver messages with delay
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void MessageManager::DeliverDelayedMessage()
	{
		int index = 0;

		while( index < m_iNbMessages )
		{
			if( m_delayedMessages[index]->m_fDeliveryTime <= m_deliveryTimer.getElapsedTime().asSeconds() )
			{

				RouteMsg( m_delayedMessages[index] );
				delete m_delayedMessages[index];
				m_delayedMessages.Remove( index );
				m_iNbMessages--;
			}
			else ++index;
		}
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Sends a message 
	//Parameter 1 : Time of the delivery (0 if instant)
	//Parameter 2 : Message type
	//Parameter 3 and 4 : Sender and receiver ID
	//Parameter 5 : Data to send
	//Parameter 6 : State in which the receiver is allowed to get the message (-1 for any)
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void MessageManager::Send( float _delay, int _name, int _sender, int _receiver, void* _data, int _state )
	{
		//Imediate delivery
		if( _delay <= 0.f )
		{
			Message msg( m_deliveryTimer.getElapsedTime().asSeconds(), _name, _sender, _receiver, _data, _state );
			RouteMsg( &msg );
		}
		else
		{
			//Check for duplicates - then store
			for( int i = 0 ; i < m_iNbMessages ; ++i )
			{
				if( m_delayedMessages[i]->m_bDelivered == false && m_delayedMessages[i]->m_type == _name && m_delayedMessages[i]->m_sender == _sender && m_delayedMessages[i]->m_receiver == _receiver && m_delayedMessages[i]->m_iState == _state )
					return;				//Already in list, don't add
			}

			float fDeliveryTime = _delay + m_deliveryTimer.getElapsedTime().asSeconds();
			m_delayedMessages.PushBack( new Message( fDeliveryTime, _name, _sender, _receiver, _data, _state ) );
			m_iNbMessages++;
		}
	}


	//=========================================================
	//==========================PRIVATE=========================
	//=========================================================

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Message delivering
	//Parameter : Message to deliver
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void MessageManager::RouteMsg( Message* _msg )
	{
		GameObjectAI* object = g_pFZN_AIMgr->GetGameObject( _msg->m_receiver );

		if( object != nullptr )
		{
			_msg->m_bDelivered = true;
			object->Process( StateMachine::EVENT_Message, _msg );
		}
	}
}
