#pragma once

class ClassServeur;

struct thread_param{
	ClassServeur* ser;
	SOCKET soc;
	int ID;
};

typedef struct s_user{
	string login;
	string description;
	int channel;
	int ID;
	char TID[11];
	int user_type;
	SOCKET user_soc;
	SOCKET voice_soc;
}User;

typedef struct t_user{
	string login;
	string ID;
	int chan_group;
}t_user;

typedef struct s_channel
{
	string name;
	int ID;
	string desc;
	struct s_channel *parent;
	list<struct s_channel*> sub;
	list<User*> list_user;

}Channel;

struct multisend_param{
	ClassServeur * ser;
	User multi_info;
	char multi_buff[1024];
	char *TID;
};

class ClassServeur
{
private:
	int           port;
	SOCKET	      ListeningSocket;
	bool          running;
	SOCKADDR      ServerAddr;
	void          ft_init_channels();
	void          send_init_info(User *info);
	void          addUserToChan(int chanID, User *info);
	DWORD         ClientThread(SOCKET, int);
	int ft_send(User *info, char* buff);
	int ft_voice(User* info);
	int ft_info(User* info, char* buff, char* msgserv);
	void removeUserFromChan(int IDChan, int IDUser);
	int index;
	list<User*> log_list;
	list<Channel*> chan_list;

public:
	ClassServeur(int);
	~ClassServeur();
	int           init();
	int           start();
	int           pause();
	static DWORD WINAPI ThreadLauncher(void *p){
		struct thread_param *Obj = reinterpret_cast<struct thread_param*>(p);
		ClassServeur *s = Obj->ser;
		return s->ClientThread(Obj->soc, Obj->ID);
	}
	static DWORD WINAPI ThreadLauncher_Multivoice(void *p){
		struct multisend_param *Obj = reinterpret_cast<struct multisend_param*>(p);
		ClassServeur *s = Obj->ser;
		return s->ft_voice(&(Obj->multi_info));
	}
	static DWORD WINAPI ThreadLauncher_Multisend(void *p){
		struct multisend_param *Obj = reinterpret_cast<struct multisend_param*>(p);
		ClassServeur *s = Obj->ser;
		return s->ft_send(&(Obj->multi_info), Obj->multi_buff);
	}
	static DWORD WINAPI ThreadLauncher_Multiinfo(void *p){
		struct multisend_param *Obj = reinterpret_cast<struct multisend_param*>(p);
		ClassServeur *s = Obj->ser;
		return s->ft_info(&(Obj->multi_info), Obj->multi_buff + 4, Obj->TID);
	}
};
