#include "FWW/FWWIncludes.h"

#include "FWW/Tools/FWWTools.h"


namespace wzn
{
	const char* FWWTools::ToCharPtr( String^ _sString )
	{
		char* cWord = (char*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( _sString ).ToPointer();
		return std::string( cWord ).c_str();
	}
}
