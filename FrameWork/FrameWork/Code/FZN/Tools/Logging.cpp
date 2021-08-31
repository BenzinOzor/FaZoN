//------------------------------------------------------------------------
//Author : Philippe OFFERMANN
//Date : 02.03.16
//Description : Logging functions
//------------------------------------------------------------------------

#include "FZN/Includes.h"
#include <stdarg.h>

#define FZN_LOG_STRLEN 2048
#define FZN_ENABLE_LOGS 1

namespace fzn
{


	FZN_EXPORT void LogMessage( const char* _sFile, int _iLine, const char* _message, ... )
	{
#if FZN_ENABLE_LOGS
		char sMessage[FZN_LOG_STRLEN];
		va_list args;
		va_start( args, _message );

		vsprintf_s( sMessage, FZN_LOG_STRLEN - 1, _message, args );
		
		OutputDebugStringA( Tools::Sprintf( "%s (%i) : %s\n", _sFile, _iLine, sMessage ).c_str() );
#endif
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Write a message in the ouput window
	//Parameter 1 : Display the message in both console and output window (true) or not
	//Parameter 2 : Message color in the console
	//Parameter 3 : Message to display
	//Additionnal parameters : Message's arguments
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void LogMessage( const std::string& _sFile, int _iLine, DBG_MSG_COLORS _eColor, const char* _message, ... )
	{
#if FZN_ENABLE_LOGS
		char sMessage[FZN_LOG_STRLEN];
		DBG_MSG_COLORS eColor = DBG_MSG_COL_WHITE;

		if( _eColor >= 0 && _eColor < DBG_MSG_COL_COUNT )
			eColor = _eColor;

		va_list args;
		va_start( args, _message );

		vsprintf_s( sMessage, FZN_LOG_STRLEN - 1, _message, args );

		std::string sTruncatedPath = _sFile.substr( _sFile.find_last_of( '\\' ) + 1 );

		OutputDebugStringA( Tools::Sprintf( "%s (%i) : %s\n", _sFile.c_str(), _iLine, sMessage ).c_str() );
		
		SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), eColor );
		printf( "%s (%i) : %s\n", sTruncatedPath.c_str(), _iLine, sMessage );
		SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), DBG_MSG_COL_WHITE );
#endif
	}
} //namespace fzn