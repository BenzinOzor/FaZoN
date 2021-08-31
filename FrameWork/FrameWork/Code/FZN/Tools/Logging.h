//------------------------------------------------------------------------
//Author : Philippe OFFERMANN
//Date : 02.03.16
//Description : Logging functions
//------------------------------------------------------------------------

#ifndef _ENGINELOGGING_H_
#define _ENGINELOGGING_H_

#include <string>

#include "FZN/Defines.h"

#define FZN_LOG( ... )					fzn::LogMessage( __FILE__, __LINE__, __VA_ARGS__ )
#define FZN_COLOR_LOG( oColor, ... )	fzn::LogMessage( __FILE__, __LINE__, oColor, __VA_ARGS__ )
#define FZN_DBLOG( ... )				fzn::LogMessage( __FILE__, __LINE__, fzn::DBG_MSG_COLORS::DBG_MSG_COL_WHITE, __VA_ARGS__ )

namespace fzn
{
	enum DBG_MSG_COLORS
	{
		DBG_MSG_COL_BLACK,
		DBG_MSG_COL_DARKBLUE,
		DBG_MSG_COL_DARKGREEN,
		DBG_MSG_COL_DARKCYAN,
		DBG_MSG_COL_DARKRED,
		DBG_MSG_COL_PURPLE,
		DBG_MSG_COL_DARKYELLOW,
		DBG_MSG_COL_LIGHTGRAY,
		DBG_MSG_COL_GRAY,
		DBG_MSG_COL_BLUE,
		DBG_MSG_COL_GREEN,
		DBG_MSG_COL_CYAN,
		DBG_MSG_COL_RED,
		DBG_MSG_COL_PINK,
		DBG_MSG_COL_YELLOW,
		DBG_MSG_COL_WHITE,
		DBG_MSG_COL_COUNT
	};

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Write a message in the ouput window
	//Parameter 1 : Message to display
	//Additionnal parameters : Message's arguments
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	FZN_EXPORT void LogMessage( const char* _sFile, int _iLine, const char* _message, ... );

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Write a message in the ouput window
	//Parameter 1 : Display the message in both console and output window (true) or not
	//Parameter 2 : Message color in the console
	//Parameter 3 : Message to display
	//Additionnal parameters : Message's arguments
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	FZN_EXPORT void LogMessage( const std::string& _sFile, int _iLine, DBG_MSG_COLORS _color, const char* _message, ... );
} //namespace fzn

#endif //_ENGINELOGGING_H_
