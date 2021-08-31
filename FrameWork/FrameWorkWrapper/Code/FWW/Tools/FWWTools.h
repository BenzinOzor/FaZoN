#ifndef __FWW_TOOLS_H__
#define __FWW_TOOLS_H__

//#pragma make_public(std::string)

using namespace System;

namespace wzn
{
	public ref class FWWTools
	{
	public :
		static const char* ToCharPtr( String^ _sString );
	};
}

#endif //__FWW_TOOLS_H__
