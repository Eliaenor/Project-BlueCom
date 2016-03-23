#include "stdafx.h"
#include "ClassClient.h"


ClassClient::ClassClient(){	return; }

int ClassClient::init()
{
	struct addrinfo *host = NULL;
	char            HostName[100];
	WSADATA	        wsaData;

	setRecordPlay(0, NULL);
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		cerr << "WSAStartup a echoue " << endl;
		return 1;
	}
	if (gethostname(HostName, 100) == SOCKET_ERROR)
	{
		cerr << "gethostname() a rencontre l'erreur " << WSAGetLastError() << endl;
		return 1;
	}
	if ((getaddrinfo(HostName, NULL, NULL, &host)) != 0)
	{
		cerr << "getaddrinfo() a rencontre l'erreur " << WSAGetLastError() << endl;
		return 1;
	}
	ClientAddr = *(host->ai_addr);
	cout << "Client correctement initialise" << endl;
	return 0;
}

int ClassClient::start(const char* addr, const char* port)
{
	SOCKADDR     			ServeurAddr;
	struct addrinfo         *host = NULL;


	if ((ListeningSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
	{
		cerr << "ne peut creer la socket n1. Erreur n° " << WSAGetLastError() << endl;
		WSACleanup();
		return 1;
	}
	if ((getaddrinfo(addr, port, NULL, &host)) != 0)
	{
		cerr << "getaddrinfo() a rencontre l'erreur " << WSAGetLastError() << endl;
		return 1;
	}
	ServeurAddr = *(host->ai_addr);
	ServeurAddr.sa_family = AF_INET;
	if (connect(ListeningSocket, &ServeurAddr, sizeof(ServeurAddr)) == SOCKET_ERROR)
	{
		cerr << "connect() n1 a rencontre l'erreur " << WSAGetLastError() << endl;
		return 1;
	}

	cout << "Connection OK" << endl;
	return 0;
}

int ClassClient::startVoice(const char* addr, const char* port)
{
	SOCKADDR     			ServeurAddr;
	struct addrinfo         *host = NULL;

	if ((VoiceSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
	{
		cerr << "ne peut creer la socket n2. Erreur n° " << WSAGetLastError() << endl;
		WSACleanup();
		return 1;
	}
	if ((getaddrinfo(addr, port, NULL, &host)) != 0)
	{
		cerr << "getaddrinfo() a rencontre l'erreur " << WSAGetLastError() << endl;
		return 1;
	}
	ServeurAddr = *(host->ai_addr);
	ServeurAddr.sa_family = AF_INET;
	if (connect(VoiceSocket, &ServeurAddr, sizeof(ServeurAddr)) == SOCKET_ERROR)
	{
		cerr << "connect() n2 a rencontre l'erreur " << WSAGetLastError() << endl;
		return 1;
	}
	cout << "Voice OK" << endl;
	return 0;
}

bool ClassClient::DSinit()
{
	HRESULT hr;

	hr = DirectSoundCreate8(NULL, &lpds, NULL);
	if (hr != DS_OK)
	{
		// !!!!!!! Must control that and signal the user !!!!!!!
		cout << "[DSinit] An error as ocurred when 00." << endl;
		return FALSE;
	}
	HWND hWnd = GetForegroundWindow();
	if (hWnd == NULL)
	{
		hWnd = GetDesktopWindow();
	}
	hr = lpds->SetCooperativeLevel(hWnd, DSSCL_NORMAL);
	if (hr != DS_OK)
	{
		cout << "[DSinit] An error as ocurred when 01." << endl;
		return FALSE;
	}
	return TRUE;
}

void ClassClient::setTID()
{
	char buff[11];

	recv(ListeningSocket, buff, 11, NULL);
	cout << buff << endl;
	strcpy_s(TID, buff);
	cout << "TID is : " << TID << endl;
	return;
}

void ClassClient::setVoiceSocket()
{
	string tmp;

	tmp = string("/TID/");
	tmp += string(TID);
	cout << tmp << endl;
	send(VoiceSocket, tmp.c_str(), 16, NULL);
	return;
}

void ClassClient::setUsername()
{
	char user[16];

	cout << "Enter Username :";
	cin.getline(user, 256);
	username = string(user);
	send(ListeningSocket, user, (strlen(user) + 1), NULL);
	return;
}

SOCKET ClassClient::getSocket()
{
	return ListeningSocket;
}

SOCKET ClassClient::getVoiceSocket()
{
	return VoiceSocket;
}

void ClassClient::startRecord()
{
	WAVEFORMATEX wfx;
	DWORD err;
	HRESULT hr;
	DSCBUFFERDESC dscbd;
	LPDIRECTSOUNDCAPTURE8 lplpDSC;
	DSBPOSITIONNOTIFY PositionNotify[cEvents];
	LPDIRECTSOUNDNOTIFY lpDsNotify;

	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = 1;
	wfx.nSamplesPerSec = SAMPLEPERSEC;
	wfx.wBitsPerSample = 16;
	wfx.nAvgBytesPerSec = SAMPLEPERSEC * BLOCKALIGN;
	wfx.nBlockAlign = BLOCKALIGN;
	wfx.cbSize = 0;
	
	hr = DirectSoundCaptureCreate8(NULL, &lplpDSC, NULL);
	if (hr != DS_OK)
	{
		cout << "[startRecord] An error as ocurred when 01." << endl;
		return;
	}
	ZeroMemory(&dscbd, sizeof(dscbd));
	dscbd.dwSize = sizeof(dscbd);
	dscbd.dwBufferBytes = ((wfx.nAvgBytesPerSec / 10) * 3);
	dscbd.lpwfxFormat = &wfx;

	hr = lplpDSC->CreateCaptureBuffer(&dscbd, &pDSCB, NULL);
	if (hr != DS_OK)
	{
		cout << "[startRecord] An error as ocurred when 02." << endl;
		hr = pDSCB->QueryInterface(IID_IDirectSoundCaptureBuffer8, (LPVOID*)&pDSCB);
		pDSCB->Release();
		return;
	}
	hr = pDSCB->QueryInterface(IID_IDirectSoundNotify, (LPVOID *)&lpDsNotify);
	if (hr != DS_OK)
	{
		cout << "[startRecord] An error as ocurred when 03." << endl;
		return;
	}
	// Create the events
	for (int i = 0; i < cEvents; ++i)
	{
		rghEvent[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (NULL == rghEvent[i])
		{
			hr = GetLastError();
			cout << "[startRecord] An error as ocurred when 04. code : "<< hr << endl;
			return;
		}
	}
	// Describe notifications. 

	PositionNotify[0].dwOffset = 0;
	PositionNotify[0].hEventNotify = rghEvent[0];

	PositionNotify[1].dwOffset = cBuffUse;
	PositionNotify[1].hEventNotify = rghEvent[1];

	PositionNotify[2].dwOffset = cBuffUse * 2;
	PositionNotify[2].hEventNotify = rghEvent[2];

	PositionNotify[3].dwOffset = DSBPN_OFFSETSTOP;
	PositionNotify[3].hEventNotify = rghEvent[3];

	hr = lpDsNotify->SetNotificationPositions(cEvents, PositionNotify);
	if (hr != DS_OK)
	{
		cout << "[startRecord] An error as ocurred when 05." << endl;
		return;
	}
	p.pSDCB = pDSCB;
	for (int i = 0; i < cEvents; i++)
	p.rghEvent[i] = rghEvent[i];
	p.EventGo = CreateEvent(NULL, TRUE, FALSE, NULL);
	p.ListeningSocket = VoiceSocket;

	//Create Message thread
	HANDLE hProcessThread = CreateThread(NULL, 0, &EventListener, &p, 0, &err);
	CloseHandle(hProcessThread);

	hr = WaitForSingleObject(p.EventGo, INFINITE);
	if (hr != WAIT_OBJECT_0)
	{
		cout << "[startRecord] An error as ocurred when 05-2-1." << endl;
		return;
	}
	
	hr = pDSCB->Start(DSCBSTART_LOOPING);
	if (hr != DS_OK)
	{
		cout << "[startRecord] An error as ocurred when 06." << endl;
		return;
	}
	return;
}

void ClassClient::stopRecord()
{
	HRESULT hr;

	if (pDSCB == NULL)
		return;
	hr = pDSCB->Stop();
	if (hr != DS_OK)
	{
		cout << "[stopRecord] An error as ocurred when 00." << endl;
		return;
	}
	cout << "Recording succesfully stopped." << endl;
	return;
}

void ClassClient::exit()
{
	stopRecord();
	return;
}

HWAVEIN ClassClient::getHandle()
{
	return 0;
}

string ClassClient::getUsername()
{
	return username;
}

LPDIRECTSOUNDBUFFER* ClassClient::getLPDSB(int flag)
{
	if (flag == 1)
		return &lpDsb;
	if (flag == 2)
		return &lpDsbS;
	else
		return NULL;
}

void ClassClient::destroyPlay(LPDIRECTSOUNDBUFFER *lplpDsb, int i)
{
	(*lplpDsb)->Stop();
	(*lplpDsb)->Release();
	*lplpDsb = NULL;
	if (i == 1)
		setRecordPlay(0, NULL);
}

bool ClassClient::createPlay(LPDIRECTSOUNDBUFFER *lplpDsb, int i)
{
	DSBUFFERDESC dsbdesc;
	WAVEFORMATEX wfx;
	HRESULT hr;

	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = CHANNEL;
	wfx.nSamplesPerSec = SAMPLEPERSEC;
	wfx.wBitsPerSample = BITPERSAMPLE;
	wfx.nAvgBytesPerSec = SAMPLEPERSEC * BLOCKALIGN;
	wfx.nBlockAlign = BLOCKALIGN;
	wfx.cbSize = 0;

	ZeroMemory(&dsbdesc, sizeof(dsbdesc));
	dsbdesc.dwSize = sizeof(DSBUFFERDESC);
	dsbdesc.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLVOLUME;
	dsbdesc.dwBufferBytes = ((wfx.nAvgBytesPerSec / 10) * 3);
	dsbdesc.lpwfxFormat = &wfx;

	hr = lpds->CreateSoundBuffer(&dsbdesc, lplpDsb, NULL);
	if (hr != DS_OK)
	{
		*lplpDsb = NULL;
		return FALSE;
	}
	if (i == 1)
		setRecordPlay(1, lplpDsb);
	if (i == 2)
		setPlayBuff(1, lplpDsb);
	return TRUE;
}

void ClassClient::getChanList()
{
	char buff[1024];
	int res = 1;
	Channel *chan;

	res = recv(ListeningSocket, buff, 1024, NULL);
	while (strncmp("SRV/END/", buff, 8) != 0)
	{
		buff[1023] = '\0';
		chan = new Channel;
		unserial(chan, buff + 8);
		channels.push_back(chan);
		res = recv(ListeningSocket, buff, 1024, NULL);
	}
	cout << "Chan List initialized" << endl;
	return;
};