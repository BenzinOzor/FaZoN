/*
AStarSearch class
Copyright 2001 Justin Heyes-Jones
*/

//#ifndef _STLASTARDEFINITIONS_H_
//#define _STLASTARDEFINITIONS_H_
#include "FZN/Game/PathFinding/STLAstar.h"


namespace fzn
{
	/////////////////CONSTRUCTOR / DESTRUCTOR/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Constructor
	//Parameter : Maximum number of nodes
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class UserState>
	AStarSearch<UserState>::AStarSearch( int _maxNodes /*=1000*/ ) :
		m_AllocateNodeCount( 0 ), m_FreeNodeCount( 0 ), m_FixedSizeAllocator( _maxNodes ),
		m_State( SEARCH_STATE_NOT_INITIALISED ), m_CurrentSolutionNode( nullptr ), m_CancelRequest( FALSE ), m_bReopenClosed( FALSE ), m_nodesNumber( 0 )
	{
	}


	/////////////////OTHER FUNCTIONS/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Call at any time to cancel the search and free up all the memory
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class UserState>
	void SetReopenClosed( INT8 _reopen )
	{
		m_bReopenClosed = _reopen;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Call at any time to cancel the search and free up all the memory
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class UserState>
	void AStarSearch<UserState>::CancelSearch()
	{
		m_CancelRequest = true;
		m_nodesNumber = 0;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Setting of the start and goal states
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class UserState>
	void AStarSearch<UserState>::SetStartAndGoalStates( UserState& _start, UserState& _goal )
	{
		m_CancelRequest = false;

		m_Start = AllocateNode();
		m_Goal = AllocateNode();		// CB: to do: goal never deallocated when search failed or canceled...

		m_Start->m_UserState = _start;
		m_Goal->m_UserState = _goal;

		m_State = SEARCH_STATE_SEARCHING;

		// Initialise the AStar specific parts of the Start Node
		// The user only needs fill out the state information

		m_Start->g = 0;
		m_Start->h = m_Start->m_UserState.GoalDistanceEstimate( m_Goal->m_UserState );
		m_Start->f = m_Start->g + m_Start->h;
		m_Start->parent = 0;

		// Push the start node on the Open list

		m_OpenList.push_back( m_Start ); // heap now unsorted

		// Sort back element into heap
		push_heap( m_OpenList.begin(), m_OpenList.end(), HeapCompare_f() );

		// Initialise counter for search steps
		m_Steps = 0;
		m_nodesNumber = 0;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accessor on the start and goal states
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class UserState>
	void GetStartAndGoalStates( UserState& _start, UserState& _goal )
	{
		_start = m_Start->m_UserState;
		_goal = m_Goal->m_UserState;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Advances search one step
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class UserState>
	UINT AStarSearch<UserState>::SearchStep()
	{
		// Firstly break if the user has not initialised the search
		//assert((m_State > SEARCH_STATE_NOT_INITIALISED) && (m_State < SEARCH_STATE_INVALID));

		// Next I want it to be safe to do a searchstep once the search has succeeded...
		if( ( m_State == SEARCH_STATE_SUCCEEDED ) || ( m_State == SEARCH_STATE_FAILED ) )
			return m_State;

		// Failure is defined as emptying the open list as there is nothing left to search...
		// New: Allow user abort
		if( m_OpenList.empty() || m_CancelRequest )
		{
			FreeAllNodes();
			m_State = SEARCH_STATE_FAILED;
			return m_State;
		}

		// Incremement step count
		m_Steps++;

		// Pop the best node (the one with the lowest f) 
		Node* n = m_OpenList.front(); // get pointer to the node
		pop_heap( m_OpenList.begin(), m_OpenList.end(), HeapCompare_f() );
		m_OpenList.pop_back();

		// Check for the goal, once we pop that we're done
		if( n->m_UserState.IsGoal( m_Goal->m_UserState ) )
		{
			// The user is going to use the Goal Node he passed in, so copy the parent pointer of n 
			m_Goal->parent = n->parent;

			// A special case is that the goal was passed in as the start state so handle that here
			if( n != m_Start )
			{
				//delete n;
				FreeNode( n );

				// set the child pointers in each node (except Goal which has no child)
				Node* nodeChild = m_Goal;
				Node* nodeParent = m_Goal->parent;

				do
				{
					nodeParent->child = nodeChild;
					nodeChild = nodeParent;
					nodeParent = nodeParent->parent;
					m_nodesNumber++;
				} while( nodeChild != m_Start ); // Start is always the first node by definition

			}

			// delete nodes that aren't needed for the solution
			FreeUnusedNodes();
			m_State = SEARCH_STATE_SUCCEEDED;

			return m_State;
		}
		else // not goal
		{
			// We now need to generate the successors of this node
			// The user helps us to do this, and we keep the new nodes in m_Successors ...
			m_Successors.clear(); // empty vector of successor nodes to n

			// User provides this functions and uses AddSuccessor to add each successor of node 'n' to m_Successors
			INT8 ret = n->m_UserState.GetSuccessors( this, n->parent ? &n->parent->m_UserState : false );

			if( !ret )
			{

				for( int i = m_Successors.size() - 1 ; i >= 0 ; --i )
					FreeNode( m_Successors[i] );

				m_Successors.clear(); // empty vector of successor nodes to n

				// free up everything else we allocated
				FreeAllNodes();

				m_State = SEARCH_STATE_OUT_OF_MEMORY;
				return m_State;
			}

			// Now handle each successor to the current node ...
			for( UINT iSuccessor = 0 ; iSuccessor < m_Successors.size() ; ++iSuccessor )
			{
				// 	The g value for this successor ...
				//float newg = n->g + n->m_UserState.GetCost((*successor)->m_UserState);
				float newg = n->g + n->m_UserState.GetCost( m_Successors[iSuccessor]->m_UserState );

				// Now we need to find whether the node is on the open or closed lists
				// If it is but the node that is already on them is better (lower g)
				// then we can forget about this successor

				// First linear search of open list to find node

				UINT iOpenListResult = 0;
				for( ; iOpenListResult < m_OpenList.size() ; ++iOpenListResult )
				{
					if( m_OpenList[iOpenListResult]->m_UserState.IsSameState( m_Successors[iSuccessor]->m_UserState ) )
						break;
				}

				if( iOpenListResult < m_OpenList.size() )
				{
					// we found this state on open
					if( m_OpenList[iOpenListResult]->g <= newg )
					{
						FreeNode( m_Successors[iSuccessor] );
						// the one on Open is cheaper than this one
						continue;
					}
				}

				UINT iClosedListResult = 0;
				for( ; iClosedListResult < m_ClosedList.size() ; ++iClosedListResult )
				{
					if( m_ClosedList[iClosedListResult]->m_UserState.IsSameState( m_Successors[iSuccessor]->m_UserState ) )
						break;
				}

				if( iClosedListResult < m_ClosedList.size() )
				{
					// we found this state on closed
					if( !m_bReopenClosed || m_ClosedList[iClosedListResult]->g <= newg )
					{
						// the one on Closed is cheaper than this one
						FreeNode( m_Successors[iSuccessor] );
						continue;
					}
				}

				// This node is the best node so far with this particular state
				// so lets keep it and set up its AStar specific data ...
				m_Successors[iSuccessor]->parent = n;
				m_Successors[iSuccessor]->g = newg;
				m_Successors[iSuccessor]->h = m_Successors[iSuccessor]->m_UserState.GoalDistanceEstimate( m_Goal->m_UserState );
				m_Successors[iSuccessor]->f = m_Successors[iSuccessor]->g + m_Successors[iSuccessor]->h;

				// Remove successor from closed if it was on it
				if( iClosedListResult < m_ClosedList.size() )
				{
					// remove it from Closed
					FreeNode( m_ClosedList[iClosedListResult] );
					m_ClosedList.erase( std::find( m_ClosedList.begin(), m_ClosedList.end(), m_ClosedList[iClosedListResult] ) );
				}

				// Update old version of this node
				if( iOpenListResult < m_OpenList.size() )
				{
					FreeNode( m_OpenList[iOpenListResult] );
					m_OpenList.erase( std::find( m_OpenList.begin(), m_OpenList.end(), m_OpenList[iOpenListResult] ) );

					// re-make the heap 
					make_heap( m_OpenList.begin(), m_OpenList.end(), HeapCompare_f() );

					// make_heap rather than sort_heap is an essential bug fix
					// thanks to Mike Ryynanen for pointing this out and then explaining
					// it in detail. sort_heap called on an invalid heap does not work
					//					sort_heap( m_OpenList.begin(), m_OpenList.end(), HeapCompare_f() );
					//					assert( is_heap( m_OpenList.begin(), m_OpenList.end(), HeapCompare_f() ) );
				}

				// heap now unsorted
				m_OpenList.push_back( m_Successors[iSuccessor] );

				// sort back element into heap
				push_heap( m_OpenList.begin(), m_OpenList.end(), HeapCompare_f() );
			}

			// push n onto Closed, as we have expanded it now
			m_ClosedList.push_back( n );

		} // end else (not goal so expand)
		return m_State; // Succeeded bool is false at this point. 
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// User calls this to add a successor to a list of successors when expanding the search frontier
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class UserState>
	INT8 AStarSearch<UserState>::AddSuccessor( UserState& _state )
	{
		Node* node = AllocateNode();

		if( node )
		{
			node->m_UserState = _state;
			m_Successors.push_back( node );
			return true;
		}

		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Free the solution nodes
	// This is done to clean up all used Node memory when you are done with the search
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class UserState>
	void AStarSearch<UserState>::FreeSolutionNodes()
	{
		Node* n = m_Start;

		if( m_Start->child )
		{
			do
			{
				Node* del = n;
				n = n->child;
				FreeNode( del );
				del = nullptr;
			} while( n != m_Goal );

			FreeNode( n ); // Delete the goal
		}
		else
		{
			// if the start node is the solution we need to just delete the start and goal nodes
			FreeNode( m_Start );
			FreeNode( m_Goal );
		}
	}


	/////////////////SOLUTION TRAVERSING FUNCTIONS/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Accesor on the distance to the goal
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class UserState>
	float GetSolutionDistance()
	{
		if( m_Goal->parent )
			return m_Goal->parent->f;
		return -1.0f;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Get start node
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class UserState>
	UserState* AStarSearch<UserState>::GetSolutionStart()
	{
		m_CurrentSolutionNode = m_Start;

		return ( m_Start ? &m_Start->m_UserState : nullptr );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Get end node
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class UserState>
	UserState* AStarSearch<UserState>::GetSolutionEnd()
	{
		m_CurrentSolutionNode = m_Goal;

		return ( m_Goal ? &m_Goal->m_UserState : nullptr );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Get next node
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class UserState>
	UserState* AStarSearch<UserState>::GetSolutionNext()
	{
		if( m_CurrentSolutionNode )
		{
			if( m_CurrentSolutionNode->child )
			{
				Node* child = m_CurrentSolutionNode->child;

				m_CurrentSolutionNode = m_CurrentSolutionNode->child;

				return &child->m_UserState;
			}
		}

		return nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Step solution iterator backwards
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class UserState>
	UserState* AStarSearch<UserState>::GetSolutionPrev()
	{
		if( m_CurrentSolutionNode )
		{
			if( m_CurrentSolutionNode->parent )
			{
				Node* parent = m_CurrentSolutionNode->parent;

				m_CurrentSolutionNode = m_CurrentSolutionNode->parent;

				return &parent->m_UserState;
			}
		}

		return nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//For educational use and debugging it is useful to be able to view
	//the open and closed list at each step, here are two functions to allow that.
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Get start node in the open list
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class UserState>
	UserState* AStarSearch<UserState>::GetOpenListStart()
	{
		float f, g, h;
		return GetOpenListStart( f, g, h );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Get start node in the open list
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class UserState>
	UserState* AStarSearch<UserState>::GetOpenListStart( float& f, float& g, float& h )
	{
		iterDbgOpen = m_OpenList.begin();

		if( iterDbgOpen != m_OpenList.end() )
		{
			f = ( *iterDbgOpen )->f;
			g = ( *iterDbgOpen )->g;
			h = ( *iterDbgOpen )->h;
			return &( *iterDbgOpen )->m_UserState;
		}

		return nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Get next node in the open list
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class UserState>
	UserState* AStarSearch<UserState>::GetOpenListNext()
	{
		float f, g, h;
		return GetOpenListNext( f, g, h );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Get next node in the open list
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class UserState>
	UserState* AStarSearch<UserState>::GetOpenListNext( float& f, float& g, float& h )
	{
		iterDbgOpen++;
		if( iterDbgOpen != m_OpenList.end() )
		{
			f = ( *iterDbgOpen )->f;
			g = ( *iterDbgOpen )->g;
			h = ( *iterDbgOpen )->h;
			return &( *iterDbgOpen )->m_UserState;
		}

		return nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Get start node in the closed list
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class UserState>
	UserState* AStarSearch<UserState>::GetClosedListStart()
	{
		float f, g, h;
		return GetClosedListStart( f, g, h );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Get start node in the closed list
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class UserState>
	UserState* AStarSearch<UserState>::GetClosedListStart( float& f, float& g, float& h )
	{
		iterDbgClosed = m_ClosedList.begin();
		if( iterDbgClosed != m_ClosedList.end() )
		{
			f = ( *iterDbgClosed )->f;
			g = ( *iterDbgClosed )->g;
			h = ( *iterDbgClosed )->h;
			return &( *iterDbgClosed )->m_UserState;
		}

		return nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Get next node in the closed list
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class UserState>
	UserState* AStarSearch<UserState>::GetClosedListNext()
	{
		float f, g, h;
		return GetClosedListNext( f, g, h );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Get next node in the closed list
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class UserState>
	UserState* AStarSearch<UserState>::GetClosedListNext( float& f, float& g, float& h )
	{
		iterDbgClosed++;
		if( iterDbgClosed != m_ClosedList.end() )
		{
			f = ( *iterDbgClosed )->f;
			g = ( *iterDbgClosed )->g;
			h = ( *iterDbgClosed )->h;
			return &( *iterDbgClosed )->m_UserState;
		}

		return nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Get the number of steps
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class UserState>
	int AStarSearch<UserState>::GetStepCount()
	{
		return m_Steps;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Get the number of nodes in the solution
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class UserState>
	int AStarSearch<UserState>::GetNumberOfNodes()
	{
		return m_nodesNumber;
	}


	//=========================================================
	//==========================PRIVATE=========================
	//=========================================================

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//This is called when a search fails or is cancelled to free all used memory 
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class UserState>
	void AStarSearch<UserState>::FreeAllNodes()
	{
		UINT iOpen = 0;
		UINT iOpenListSize = m_OpenList.size();
		UINT iClosed = 0;
		UINT iClosedListSize = m_ClosedList.size();

		while( iOpen < iOpenListSize )
		{
			FreeNode( m_OpenList[iOpen] );
			iOpen++;
		}

		m_OpenList.clear();


		while( iClosed < iClosedListSize )
		{
			FreeNode( m_ClosedList[iClosed] );
			iClosed++;
		}

		m_ClosedList.clear();
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//This call is made by the search class when the search ends. A lot of nodes may be
	//created that are still present when the search ends. They will be deleted by this routine once the search ends
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class UserState>
	void AStarSearch<UserState>::FreeUnusedNodes()
	{
		UINT iOpen = 0;
		UINT iOpenListSize = m_OpenList.size();
		UINT iClosed = 0;
		UINT iClosedListSize = m_ClosedList.size();

		while( iOpen < iOpenListSize )
		{
			if( !m_OpenList[iOpen]->child )
			{
				FreeNode( m_OpenList[iOpen] );
				m_OpenList[iOpen] = nullptr;
			}

			iOpen++;
		}

		while( iClosed < iClosedListSize )
		{
			if( !m_ClosedList[iClosed]->child )
			{
				FreeNode( m_ClosedList[iClosed] );
				m_ClosedList[iClosed] = nullptr;
			}

			iClosed++;
		}

		m_OpenList.clear();
		m_ClosedList.clear();
	}


	/////////////////NODE MEMORY MANAGEMENT/////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Free a node
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class UserState>
	void AStarSearch<UserState>::FreeNode( Node *node )
	{
		m_FreeNodeCount++;

#if !USE_FSA
		delete node;
#else
		m_FixedSizeAllocator.Free( node );
#endif
	}
} //namespace fzn

//#endif //_STLASTARDEFINITIONS_H_
