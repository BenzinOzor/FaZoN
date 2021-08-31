//------------------------------------------------------------------------
//Auteur : Philippe OFFERMANN
//Date : 25.03.14
//Description : Main include file for the engine
//------------------------------------------------------------------------

#ifndef _ENGINEINCLUDES_H_
#define _ENGINEINCLUDES_H_

/////////////////SFML INCLUSIONS/////////////////
#include <SFML\Graphics.hpp>
#include <SFML\System.hpp>
#include <SFML\Window.hpp>
#include <SFML\Network.hpp>
#include <SFML\Audio.hpp>

/////////////////MISCELLANEOUS INCLUDES/////////////////
#include <Windows.h>

/////////////////ENGINE SPECIFICS INCLUDES/////////////////
#include "Defines.h"
#include "Tools/Logging.h"
#include "Tools/Math.h"
#include "Tools/Tools.h"

#include "Managers/FazonCore.h"

#pragma comment(lib, "winmm.lib")

#endif //_ENGINEINCLUDES_H_
