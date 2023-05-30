#pragma once

#include <string>
#include <vector>

#include "ExternalWrapper/EWDefines.h"


class FZN_EXPORT EWFileEncrypter
{
public:
	enum class CryptionTaskResult
	{
		eSuccess,
		eFileNotFound,
		eRSAKeyMismatch,
		eCount,
	};

	EWFileEncrypter();
	~EWFileEncrypter();

	static void InitKeys( const std::string& _sKey = "" );
	static CryptionTaskResult Decrypt_File( const std::string& _sFile, std::vector< unsigned char >& _oResult, bool _bTextFile = false );
	static void TEST();
};
