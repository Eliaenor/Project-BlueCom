#pragma once


int	ft_strinitSerial(char* str, int size);

template<typename T>
char* serial(T var, char* data){
	cout << "Error : Use a User* or a Channel* var type." << endl;
	return NULL;
}

template<>
char* serial<User*>(User* var, char* data);

template<>
char* serial<Channel*>(Channel *var, char* data);

template<typename T, typename P>
int unserial(T var, P str){
	cout << "Error : Use a User* or a Channel* var type." << endl;
	return 1;
}

template<>
int unserial<User*>(User* var, char *data);

template<>
int unserial<Channel*>(Channel *var, char *data);