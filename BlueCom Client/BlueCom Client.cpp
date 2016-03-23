// BlueCom Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

int _tmain(int argc, _TCHAR* argv[])
{
	ClassClient *MyClient = new ClassClient();
	HANDLE hProcessThread;
	SOCKET p;
	SOCKET p2;
	string addr;
	string tmp1;
	string tmp2;

	if (MyClient->init() != 0)
	{
		cerr << "ne peut initialiser le serveur" << endl;
		f_exit();
		return 1;
	}
	cout << "Server Adress (format xxx.xxx.xxx.xxx:xxxxx) :";
	getline(cin, addr);
	if (strcmp(addr.c_str(), "1") == 0)
	{
		tmp1 = string("127.0.0.1");
		tmp2 = string("12345");
	}
	else if (strcmp(addr.c_str(), "2") == 0)
	{
		tmp1 = string("25.87.36.145");
		tmp2 = string("12345");
	}
	else
	{
		tmp1 = addr.substr(0, addr.find(":"));
		tmp2 = addr.substr(addr.find(":") + 1);
	}
	cout << "Connecting to " << tmp1 << ":" << tmp2 << " ..." << endl;
	if (MyClient->start(tmp1.c_str(), tmp2.c_str()) != 0)
	{
		cerr << "Impossible de ce connecter au serveur." << endl;
		f_exit();
		return 1;
	}
	MyClient->setUsername();
	MyClient->setTID();
	if (MyClient->startVoice(tmp1.c_str(), tmp2.c_str()) != 0)
	{
		cerr << "Impossible de ce connecter au serveur (Voice)." << endl;
		f_exit();
		return 1;
	}
	MyClient->setVoiceSocket();
	char buff[256];
	MyClient->getChanList();
	p = MyClient->getSocket();
	hProcessThread = CreateThread(NULL, 0, &TextLauncher, &p, 0, NULL);
	if (hProcessThread == NULL)
	{
		cerr << "CreateThread a echoue avec l'erreur " << GetLastError() << endl;
	}
	p2 = MyClient->getVoiceSocket();
	hProcessThread = CreateThread(NULL, 0, &VoiceLauncher, &p2, 0, NULL);
	if (hProcessThread == NULL)
	{
		cerr << "CreateThread a echoue avec l'erreur " << GetLastError() << endl;
	}
	MyClient->DSinit();
	MyClient->createPlay(MyClient->getLPDSB(2), 2);
	while (strcmp(buff, "/exit") != 0)
	{
		cout << "Client: ";
		cin.getline(buff, 256);
		entryManager(buff, MyClient);
	}
	MyClient->exit();
	cout << "Quitting" << endl;
	return 0;
}

