#include "stdafx.h"

HWAVEIN HandleIN;
HWAVEOUT HandleOUT;
SOCKET GSocket;
int RecordPlay;
LPDIRECTSOUNDBUFFER* RecordPlayBuff = NULL;
LPDIRECTSOUNDBUFFER* PlayBuff = NULL;
ClassClient *MyClient = NULL;

void setMyClient(ClassClient *var)
{
	MyClient = var;
}

void setRecordPlay(int var, LPDIRECTSOUNDBUFFER* var2)
{
	RecordPlay = var;
	if (var == 1)
	RecordPlayBuff = var2;
}

void setPlayBuff(int var, LPDIRECTSOUNDBUFFER* var2)
{
	if (var == 1)
		PlayBuff = var2;
}

int getRecordPlay()
{
	return RecordPlay;
}

void setHandle(HWAVEIN var)
{
	HandleIN = var;
}

void setHandleOUT(HWAVEOUT var)
{
	HandleOUT = var;
}

void setGSocket(SOCKET var)
{
	GSocket = var;
}

void f_exit()
{
	string quit;

	cout << "Please enter 'exit' to quit." << endl;
	getline(cin, quit);
	while (strcmp(quit.c_str(), "exit") != 0)
	{
		cin >> quit;
	}
	return;
}

int ft_findvoice(char *buff)
{
	int i = 0;
	int y = 0;

	while (buff[i] != '\0')
	{
		if (buff[i] == '/')
		{
			if (y == 1)
				return i + 1;
			y++;
		}
		i++;
	}
	return 0;
}

void get_ServerMessage(char *buff)
{

	if (strncmp("001", buff, 3) == 0)
	{
		// User Join
	}
	else if (strncmp("002", buff, 3) == 0)
	{
		// User Quit
	}
	else if (strncmp("003", buff, 3) == 0)
	{
		cout << "Server Info :" << buff + 4 << endl;
		// Info
	}
	else if (strncmp("009", buff, 3) == 0)
	{
		(*PlayBuff)->Stop();
		cout << "Stoping audio stream from " << buff + 4 << endl;
	}
	else
		return ;
}

DWORD WINAPI TextLauncher(void *p){
	SOCKET *Obj = reinterpret_cast<SOCKET*>(p);
	int res = 1;
	char buff[1039];
	int pos = 0;

	while (res > 0)
	{
		res = recv(*Obj, buff, 1039, NULL);
		if (res > 0)
		{
			if (strncmp("SRV/", buff, 4) == 0)
			{
				cout << "Server Message" << endl;
				get_ServerMessage(buff + 4);
			}
			else
			{
				//cerr << "receve " << res << "octets." << endl;
				cout << "message from Serveur receved : " << buff << endl;
			}
		}
		else if (res == 0)
			cout << "connection closed." << endl;
		else
			cout << "error : recv failed" << endl;
	}
	return 0;
}

DWORD WINAPI VoiceLauncher(void *p){
	SOCKET *Obj = reinterpret_cast<SOCKET*>(p);
	int res = 1;
	char buff[cBuffUse + 1];
	int pos = 0;

	while (res > 0)
	{
		res = recv(*Obj, buff, cBuffUse + 1, NULL);
		if (res > 0)
		{
			//cout << "Sound from " << (int)buff[0] << " receved" << endl;
			WriteToBuffer(*PlayBuff, buff + 1, 0, DSBLOCK_FROMWRITECURSOR);
		}
		else if (res == 0)
			cout << "connection closed." << endl;
		else
			cout << "error : recv failed" << endl;
	}
	return 0;
}


BOOL WriteToBuffer(LPDIRECTSOUNDBUFFER lpDsb, CHAR* lpbSoundData, DWORD dwOffset, INT FLAG)
{
	LPVOID lpvPtr1;
	DWORD dwBytes1;
	LPVOID lpvPtr2;
	DWORD dwBytes2;
	HRESULT hr;

	// Obtain memory address of write block. This will be in two parts
	// if the block wraps around.
	hr = lpDsb->Lock(dwOffset, cBuffUse, &lpvPtr1,
		&dwBytes1, &lpvPtr2, &dwBytes2, FLAG);

	// If DSERR_BUFFERLOST is returned, restore and retry lock. 
	if (DSERR_BUFFERLOST == hr)
	{
		hr = lpDsb->Restore();
		if (DS_OK != hr)
		{
			// Process the failed call to Restore.
		}
		hr = lpDsb->Lock(dwOffset, cBuffUse, &lpvPtr1,
			&dwBytes1, &lpvPtr2, &dwBytes2, FLAG);
	}
	if SUCCEEDED(hr)
	{
		// Write to pointers. 
		CopyMemory(lpvPtr1, lpbSoundData, dwBytes1);
		if (NULL != lpvPtr2)
		{
			CopyMemory(lpvPtr2, lpbSoundData+dwBytes1, dwBytes2);
		}
		// Release the data back to DirectSound. 
		hr = lpDsb->Unlock(lpvPtr1, dwBytes1, lpvPtr2,dwBytes2);
		if SUCCEEDED(hr)
		{
			lpDsb->Play(0, 0, DSBPLAY_LOOPING);
			return TRUE;
		}
	}
	// Lock, Unlock, or Restore failed. 
	return FALSE;
}

DWORD WINAPI EventListener(void *p)
{
	DWORD dwWaitResult;
	DWORD hr;
	DWORD pos;
	int exit = 0;
	int i = 0;
	VOID *ptrCapture = NULL;
	DWORD sizeCapture;
	char buff[cBuffUse];

	events_param* param = reinterpret_cast<events_param*>(p);
	if (!SetEvent(param->EventGo))
	{
		printf("SetEvent failed (%d)\n", GetLastError());
		return 0;
	}
	cout << "Waiting for Events" << endl;
	while (exit == 0)
	{
		//cout << "-";
		ZeroMemory(buff, cBuffUse);
		dwWaitResult = WaitForMultipleObjects(
			cEvents,   // number of handles in array
			param->rghEvent,  // array of Events handles
			FALSE,     // wait until one is signaled
			INFINITE);
		switch (dwWaitResult)
		{
		case WAIT_OBJECT_0:
			ResetEvent(param->rghEvent[0]);
			if (i == 0)
				break;
			hr = param->pSDCB->GetCurrentPosition(NULL, &pos);
			/*cout << "3 - " << cBuffUse << endl;*/
			param->pSDCB->Lock(cBuffUse*2, cBuffUse, &ptrCapture, &sizeCapture, NULL, NULL, NULL);
			if (hr != DS_OK)
			{
				cout << "[EventListener] An error as ocurred when 00." << endl;
				return 0;
			}
			memcpy_s(buff, cBuffUse, ptrCapture, sizeCapture);
			if (RecordPlay == 1)
				WriteToBuffer(*RecordPlayBuff, buff, cBuffUse*2, 0);
			param->pSDCB->Unlock(ptrCapture, sizeCapture, NULL, NULL);
			if (hr != DS_OK)
			{
				cout << "[EventListener] An error as ocurred when 01." << endl;
				return 0;
			}
			break;
		case WAIT_OBJECT_0 + 1:
			ResetEvent(param->rghEvent[1]);
			i = 1;
			hr = param->pSDCB->GetCurrentPosition(NULL, &pos);
			//	cout << "1 - " << cBuffUse << endl;
			param->pSDCB->Lock(0, cBuffUse, &ptrCapture, &sizeCapture, NULL, NULL, NULL);
			if (hr != DS_OK)
			{
				cout << "[EventListener] An error as ocurred when 00." << endl;
				return 0;
			}
			memcpy_s(buff, cBuffUse, ptrCapture, sizeCapture);
			if (RecordPlay == 1)
				WriteToBuffer(*RecordPlayBuff, buff, 0, 0);
			param->pSDCB->Unlock(ptrCapture, sizeCapture, NULL, NULL);
			if (hr != DS_OK)
			{
				cout << "[EventListener] An error as ocurred when 01." << endl;
				return 0;
			}
			break;
		case WAIT_OBJECT_0 + 2:
			ResetEvent(param->rghEvent[2]);
			hr = param->pSDCB->GetCurrentPosition(NULL, &pos);
			//	cout << "2 - " << cBuffUse << endl;
			param->pSDCB->Lock(cBuffUse, cBuffUse, &ptrCapture, &sizeCapture, NULL, NULL, NULL);
			if (hr != DS_OK)
			{
				cout << "[EventListener] An error as ocurred when 00." << endl;
				return 0;
			}
			memcpy_s(buff, cBuffUse, ptrCapture, sizeCapture);
			if (RecordPlay == 1)
				WriteToBuffer(*RecordPlayBuff, buff, cBuffUse, 0);
			param->pSDCB->Unlock(ptrCapture, sizeCapture, NULL, NULL);
			if (hr != DS_OK)
			{
				cout << "[EventListener] An error as ocurred when 01." << endl;
				return 0;
			}
			break;
		case WAIT_OBJECT_0 + 3:
			// Stopped manually
			cout << "TEST" << endl;
			ResetEvent(param->rghEvent[3]);
			hr = param->pSDCB->GetCurrentPosition(NULL, &pos);
			if (pos < cBuffUse)
			{ /* 1st part */
				param->pSDCB->Lock(cBuffUse, cBuffUse, &ptrCapture, &sizeCapture, NULL, NULL, NULL);
				if (hr != DS_OK)
				{
					cout << "[EventListener] An error as ocurred when 00." << endl;
					return 0;
				}
				memcpy_s(buff, cBuffUse, ptrCapture, sizeCapture);
				if (RecordPlay == 1)
					WriteToBuffer(*RecordPlayBuff, buff, 0, 0);
				cout << "1 END - " << cBuffUse << endl;
			}
			else if (pos < (cBuffUse * 2))
			{ /* 2nd part */
				param->pSDCB->Lock(cBuffUse, cBuffUse, &ptrCapture, &sizeCapture, NULL, NULL, NULL);
				if (hr != DS_OK)
				{
				cout << "[EventListener] An error as ocurred when 00." << endl;
					return 0;
				}
				memcpy_s(buff, cBuffUse, ptrCapture, sizeCapture);
				if (RecordPlay == 1)
					WriteToBuffer(*RecordPlayBuff, buff, cBuffUse, 0);
				cout << "2 END - " << cBuffUse << endl;
			}
			else
			{ /* 3rd part*/
				param->pSDCB->Lock(cBuffUse * 2, cBuffUse, &ptrCapture, &sizeCapture, NULL, NULL, NULL);
				if (hr != DS_OK)
				{
					cout << "[EventListener] An error as ocurred when 00." << endl;
					return 0;
				}
				memcpy_s(buff, cBuffUse, ptrCapture, sizeCapture);
				if (RecordPlay == 1)
					WriteToBuffer(*RecordPlayBuff, buff, cBuffUse * 2, 0);
				cout << "3 END - " << cBuffUse << endl;
			}
			param->pSDCB->Unlock(ptrCapture, sizeCapture, NULL, NULL);
			if (hr != DS_OK)
			{
				cout << "[EventListener] An error as ocurred when 01." << endl;
				return 0;
			}
			break;
		}
		buff;
		send(param->ListeningSocket, buff, cBuffUse, NULL);
	}
	return 0;
}