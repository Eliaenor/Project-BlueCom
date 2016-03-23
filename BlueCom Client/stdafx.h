// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define INITGUID

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
//#include <DSound.h>

using namespace std;
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "Dsound.lib")
//#pragma comment (lib, "Dsound3d.dll")

// TODO: reference additional headers your program requires 

#define cEvents 4
#define SAMPLEPERSEC 44100
#define BITPERSAMPLE 16
#define CHANNEL 1
#define BLOCKALIGN (CHANNEL * BITPERSAMPLE) / 8
#define AVGBYTESPERSEC SAMPLEPERSEC * BLOCKALIGN
#define cBuffUse AVGBYTESPERSEC / 10
#define cBuffSise (cBuffUse * 3)

#include "ClassClient.h"
#include "../common/Utilities.h"
#include "Entry Manager.h"

void f_exit();
void setHandle(HWAVEIN var);
void setHandleOUT(HWAVEOUT var);
void setRecordPlay(int var, LPDIRECTSOUNDBUFFER* var2);
void setPlayBuff(int var, LPDIRECTSOUNDBUFFER* var2);
int getRecordPlay();
void setGSocket(SOCKET var);
BOOL WriteToBuffer(LPDIRECTSOUNDBUFFER lpDsb, CHAR* lpbSoundData, DWORD dwOffset, INT FLAG);
DWORD WINAPI TextLauncher(void *p);
DWORD WINAPI VoiceLauncher(void *p);
//DWORD WINAPI messageListener(void *p);
DWORD WINAPI EventListener(void *p);