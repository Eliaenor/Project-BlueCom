// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <cstdlib>
#include <string>
#include <list>
#include <vector>
#include <algorithm> 
#include <sstream>
#define _WINSOCKAPI_    // stops windows.h including winsock.h
#include <windows.h>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <Dsound.h>


#pragma comment(lib, "Ws2_32.lib")


// TODO: reference additional headers your program requires here

using namespace std;

#define SAMPLEPERSEC 44100
#define BITPERSAMPLE 16
#define CHANNEL 1
#define BLOCKALIGN (CHANNEL * BITPERSAMPLE) / 8
#define AVGBYTESPERSEC SAMPLEPERSEC * BLOCKALIGN
#define cBuffUse AVGBYTESPERSEC / 10
#define cBuffSise (cBuffUse * 3)

#include "ClassServeur.h"
#include "../common/Utilities.h"

int ft_cmd(string cmd);
void setRecordPlay(int var, LPDIRECTSOUNDBUFFER* var2);
DWORD WINAPI VoiceDeploy(void *p);

