#include <FZN/Includes.h>
#include <stdio.h>
#include <windows.h>

int main( int argc, char* argv[] )
{
	for( int i = 0; i < argc; ++i )
	{
		FZN_LOG( "%d - %s", i, argv[ i ] );
	}

	HINSTANCE Dll = LoadLibrary( argv[ 1 ] );
	if( Dll == NULL)
	{
		return 1;
	}

	FARPROC pMain = GetProcAddress( Dll, "main" );
	int iRes = pMain();

	FreeLibrary( Dll );
	return iRes;
}
