
#include "stdafx.h"

ClassServeur::ClassServeur(int p)
{
	port = p;
	running = false;
	index = 0;
}

int ClassServeur::init()
{
	struct addrinfo *host = NULL;
	char            HostName[100];
	WSADATA	        wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0){
		cerr << "WSAStartup a echoue " << endl;
		return 1;
	}
	if (gethostname(HostName, 100) == SOCKET_ERROR){
		cerr << "gethostname() a rencontre l'erreur " << WSAGetLastError() << endl;
		return 1;
	}
	char portStr[6];
	_itoa_s(port,portStr, 6,10);
	if ((getaddrinfo(HostName,portStr,NULL, &host)) != 0 ){
		cerr << "getaddrinfo() a rencontre l'erreur " << WSAGetLastError() << endl;
		return 1;
	}
	ServerAddr = *(host->ai_addr);
	cout << "server correctement initialise" << endl;
	return 0;
}

int ClassServeur::start()
{
	SOCKADDR_IN  			ClientAddr;
	int          			ClientAddrLen;
	HANDLE       			hProcessThread;
	SOCKET       			Newconnection;
	struct thread_param		p;
	PWSTR                   str;


	if ((ListeningSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET){
		cerr << "ne peut creer la socket. Erreur n° " << WSAGetLastError() << endl;
		WSACleanup();
		return 1;
	}
	ServerAddr.sa_family = AF_INET;
	if (bind(ListeningSocket, &ServerAddr, sizeof(ServerAddr)) == SOCKET_ERROR){
		cerr << "bind a echoue avec l'erreur " << WSAGetLastError() << endl;
		cerr << "Le port est peut-etre deja utilise par un autre processus " << endl;
		closesocket(ListeningSocket);
		WSACleanup();
		return 1;
	}
	if (listen(ListeningSocket, 5) == SOCKET_ERROR){
		cerr << "listen a echoue avec l'erreur " << WSAGetLastError() << endl;
		closesocket(ListeningSocket);
		WSACleanup();
		return 1;
	}
	cout << "serveur demarre : a l'ecoute du port " << port << endl;
	running = true;
	ClientAddrLen = sizeof(ClientAddr);
	ft_init_channels();
	while (running)
	{
		if ((Newconnection = accept(ListeningSocket, (SOCKADDR *)&ClientAddr, &ClientAddrLen)) == INVALID_SOCKET){
			cerr << "accept a echoue avec l'erreur " << WSAGetLastError() << endl;;
			closesocket(ListeningSocket);
			WSACleanup();
			return 1;
		}
		p.ser = this;
		p.soc = Newconnection;
		str = NULL;
		InetNtop(AF_INET, &ClientAddr.sin_addr, str, 17);
		cout << "client connecte :  IP : " << str << " ,port = " << ntohs(ClientAddr.sin_port) << endl;
		if (index == 2147483647)
			index = 0;
		p.ID = index++;
		hProcessThread = CreateThread(NULL, 0, &ClassServeur::ThreadLauncher, &p, 0, NULL);
		if (hProcessThread == NULL)
		{
			cerr << "CreateThread a echoue avec l'erreur " << GetLastError() << endl;
		}
	}
	return 0;
}

int ClassServeur::pause()
{
	running = false;
	cout << "ClassServeur en pause" << endl;
	closesocket(ListeningSocket);
	return 0;
}

DWORD ClassServeur::ClientThread(SOCKET soc, int ID)
{
	User* info = new User;
	HANDLE hProcessThread;
	multisend_param p;
	reverse_iterator<list<User*>::iterator> iter;
	
	cout << "thread NewUser demarre" << endl;
	int res = 1;
	char buff[1024];
	res = recv(soc, buff, 256, NULL);
	cout << "Received :" << buff << endl;
	buff[256] = '\0';
	if (strncmp(buff, "/TID/", 5) == 0)
	{
		for (iter = log_list.rbegin(); iter != log_list.rend(); iter++)
		{
			if (atoi(buff + 5) == (*iter)->ID)
			{
				(*iter)->voice_soc = soc;
				cout << "Audio Socket added to User :" << (*iter)->login << "(" << (*iter)->TID << ")" << endl;
				delete info;
				index--;
				info = (*iter);
			}
		}
	}
	else
	{
		info->ID = ID;
		_itoa_s(ID, info->TID, 10);
		info->TID[10] = '\0';
		cout << info->TID << " = TID send..." << endl;
		send(soc, info->TID, 11, NULL);
		cout << "SENDED" << endl;
		info->login = string(buff);
		info->description = string("Desc");
		info->channel = 0;
		info->user_type = 0;
		info->user_soc = soc;
		log_list.push_back(info);
		addUserToChan(0, info);
		return 0;
	}
	send_init_info(info);
	DWORD err;
	p.ser = this;
	p.multi_info = *info;
	hProcessThread = CreateThread(NULL, 0, &ClassServeur::ThreadLauncher_Multivoice, &p, 0, &err);
	CloseHandle(hProcessThread);
	while (res > 0)
	{
		ZeroMemory(buff, 1024);
		res = recv(info->user_soc, buff, 1024, NULL);
		buff[1023] = '\0';
		if (res > 0)
		{
			cout << "receve " << res << "octets from" << info->login << endl;
			cout << "message from "<< info->login << " receved :" << buff << endl;
			p.ser = this;
			p.multi_info = *info;
			strcpy_s(p.multi_buff, 1024, buff);
			if (strncmp("NFO/", buff, 4) == 0)
			{
				cout << "Info server: " << buff << endl;
				p.TID = info->TID;
				hProcessThread = CreateThread(NULL, 0, &ClassServeur::ThreadLauncher_Multiinfo, &p, 0, NULL);
			}
			else
				hProcessThread = CreateThread(NULL, 0, &ClassServeur::ThreadLauncher_Multisend, &p, 0, NULL);
		}
		else if (res == 0)
		{
			log_list.remove(info);
			cout << "connection closed." << endl;
		}
		else
		{
			log_list.remove(info);
			cout << "error : recv failed" << endl;
		}
	}

	/*    A mettre ici : code relatif au protocole utilise    */

	return 0;
}

int ClassServeur::ft_send(User* info, char* buff)
{
	string msg;

	list<Channel*>::iterator iter;
	for (iter = chan_list.begin(); iter != chan_list.end(); iter++)
	{
		if (info->channel == (*iter)->ID)
		{
			list<User*>::iterator iter2;
			for (iter2 = (*iter)->list_user.begin(); iter2 != (*iter)->list_user.end(); iter2++)
			{
				if (info->ID != (*iter2)->ID)
				{
					// Why ID don't disapear ?
					cout << "BUFF = " << buff << endl;
					msg.clear();
					msg += string("MSG/") += string(info->TID) += string("/") += string(buff);
					send((*iter2)->user_soc, msg.c_str(), (msg.length() + 1), NULL);
				}
			}
		}
	}
	return 0;
};

int ClassServeur::ft_voice(User* info)
{
	int res = 1;
	list<User*>::iterator iter;
	char buff[cBuffUse + 1];
	buff[0] = info->ID;

	while (res > 0)
	{
		ZeroMemory(buff + 1, cBuffUse - 1);
		res = recv(info->voice_soc, buff + 1, cBuffUse, NULL);
		if (res > 0)
		{
			for (iter = log_list.begin(); iter != log_list.end(); iter++)
			{
				if (info->ID != (*iter)->ID)
					send((*iter)->voice_soc, buff, (cBuffUse + 1), NULL);
			}
		}
		else if (res == 0)
		{
			removeUserFromChan(info->channel, info->ID);
			log_list.remove(info);
			cout << "connection closed." << endl;
		}
		else
		{
			removeUserFromChan(info->channel, info->ID);
			log_list.remove(info);
			cout << "error : recv failed" << endl;
		}
	}
	return 0;
};

int ClassServeur::ft_info(User* info, char* buff, char* msgserv)
{
	string msg;

	list<User*>::iterator iter;
	for (iter = log_list.begin(); iter != log_list.end(); iter++)
	{
		if (info->ID != (*iter)->ID)
		{
			msg.clear();
			msg += string("SRV/") += string(buff) += string("/") += string(msgserv);
			send((*iter)->user_soc, msg.c_str(), (msg.length() + 1), NULL);
		}
	}
	return 0;
};

void ClassServeur::ft_init_channels()
{
	//get chan list and info
	Channel* chan = new Channel;

	chan->name = string("Lobby");
	chan->ID = 0;
	chan->desc = string("Desc Lobby");
	chan->parent = NULL;

	chan_list.push_back(chan);
	return;
};

void ClassServeur::send_init_info(User *info)
{
	list<Channel*>::iterator iter;
	string msg;
	char *data = new char[292];
	ft_strinitSerial(data, 292);

	for (iter = chan_list.begin(); iter != chan_list.end(); iter++)
	{
		serial((*iter), data);
		msg.clear();
		msg += string("SRV/") += string("001") += string("/") += string(data, 292);
		send(info->user_soc, msg.c_str(), (msg.length() + 1), NULL);
	}
	delete data;
	msg.clear();
	msg += string("SRV/") += string("END") += string("/") += string("0");
	send(info->user_soc, msg.c_str(), (msg.length() + 1), NULL);
	return;
};

void ClassServeur::addUserToChan(int chanID, User* info)
{
	list<Channel*>::iterator iter;
	char *data = new char[293];
	ft_strinitSerial(data, 293);

	for (iter = chan_list.begin(); iter != chan_list.end(); iter++)
	{
		if (chanID == (*iter)->ID)
		{
			info->channel = chanID;
			serial(info, data);
			ft_info(info, "005", data);
			(*iter)->list_user.push_back(info);
			delete data;
			return;
		}
	}
	delete data;
	info->channel = 0;
	addUserToChan(0, info);
	return;
};

void ClassServeur::removeUserFromChan(int IDChan, int IDUser)
{
	list<Channel*>::iterator iter;
	char *data = new char[293];
	ft_strinitSerial(data, 293);

	for (iter = chan_list.begin(); iter != chan_list.end(); iter++)
	{
		if (IDChan == (*iter)->ID)
		{
			list<User*>::iterator iter2;
			for (iter2 = (*iter)->list_user.begin(); iter2 != (*iter)->list_user.end(); iter2++)
			{
				if ((*iter2)->ID == IDUser)
				{
					serial((*iter2), data);
					ft_info((*iter2), "005", data);
					(*iter)->list_user.remove((*iter2));
					return;
				}
			}
		}
	}
	return ;
}