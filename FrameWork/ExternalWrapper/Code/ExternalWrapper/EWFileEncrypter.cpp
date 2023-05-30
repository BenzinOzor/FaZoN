#include <msclr/marshal_cppstd.h>
#include <fstream>

#include "ExternalWrapper/EWFileEncrypter.h"

using namespace FileEncrypter::Code;
using namespace System::IO;
using System::Runtime::InteropServices::Marshal;


EWFileEncrypter::EWFileEncrypter()
{
}

EWFileEncrypter::~EWFileEncrypter()
{
}

void EWFileEncrypter::InitKeys( const std::string& _sKey /*= "" */ )
{
	Encrypter::InitKeys( msclr::interop::marshal_as< System::String^ >( _sKey ) );
}

EWFileEncrypter::CryptionTaskResult EWFileEncrypter::Decrypt_File( const std::string& _sFile, std::vector< unsigned char >& _oResult, bool _bTextFile /*= false*/ )
{
	if( _sFile.empty() )
		return CryptionTaskResult::eFileNotFound;

	array< System::Byte >^ oDecryptRet = nullptr;
	Encrypter::CryptionTaskResult eResult = Encrypter::Decrypt_File( msclr::interop::marshal_as< System::String^ >( _sFile ), oDecryptRet, _bTextFile );

	_oResult.clear();
	_oResult.reserve( oDecryptRet->Length );
	
	for( int iChar = 0; iChar < oDecryptRet->Length; ++iChar )
	{
		_oResult.push_back( (unsigned char) oDecryptRet[ iChar ] );
	}

	return (EWFileEncrypter::CryptionTaskResult)eResult;
}

void EWFileEncrypter::TEST()
{
	printf( "OUI." );
}
