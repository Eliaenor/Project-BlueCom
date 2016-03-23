#pragma once

class ClassClient;

struct thread_param{
	ClassClient* ser;
	SOCKET soc;
};

typedef struct s_user{
	string login;
	string description;
	int channel;
	int ID;
	char TID[11];
	int user_type;
	LPDIRECTSOUND8 lpds;
	LPDIRECTSOUNDBUFFER lpDsb = NULL;
}User;

typedef struct s_channel
{
	string name;
	int ID;
	string desc;
	struct s_channel *parent;
	list<struct s_channel*> sub;
	list<User*> list_user;

}Channel;

struct events_param{
	HANDLE rghEvent[cEvents];
	LPDIRECTSOUNDCAPTUREBUFFER pSDCB = NULL;
	HANDLE EventGo;
	SOCKET ListeningSocket;
};

class ClassClient
{
private:
	string           username;
	string           psw;
	char             TID[11];
	list<Channel*>   channels;
	int              ClientPort;
	SOCKET	         ListeningSocket;
	SOCKET           VoiceSocket;
	bool             connected;
	SOCKADDR         ClientAddr;

	
	//Var for record
	LPDIRECTSOUNDCAPTUREBUFFER pDSCB = NULL;
	HANDLE rghEvent[cEvents];
	struct events_param p;
	//Var for playing
	LPDIRECTSOUND8 lpds;
	LPDIRECTSOUNDBUFFER lpDsb = NULL;
	LPDIRECTSOUNDBUFFER lpDsbS = NULL;

public:
	ClassClient();
	~ClassClient();
	int           init();
	int           start(const char* addr, const char* port);
	int           startVoice(const char* addr, const char* port);
	HWAVEIN getHandle();
	string getUsername();
	void setUsername();
	void setTID();
	SOCKET getSocket();
	SOCKET getVoiceSocket();
	void setVoiceSocket();
	LPDIRECTSOUNDBUFFER* getLPDSB(int flag);
	bool DSinit();
	void getChanList();
	void startRecord();
	bool createPlay(LPDIRECTSOUNDBUFFER *lplpDsb, int i);
	void destroyPlay(LPDIRECTSOUNDBUFFER *lplpDsb, int i);
	void stopRecord();
	void exit();
};

