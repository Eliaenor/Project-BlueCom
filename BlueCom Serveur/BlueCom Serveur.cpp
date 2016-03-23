// BlueCom Serveur.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

void f_exit()
{
	string quit;

	cout << "Please enter 'exit' to quit." << endl;
	cin >> quit;
	while (strcmp(quit.c_str(), "exit") != 0)
	{
		cin >> quit;
	}
	return;
}


int _tmain(int argc, _TCHAR* argv[])
{
	ClassServeur *MyServer = new ClassServeur(12345);

	if (MyServer->init() != 0)
	{
		cerr << "ne peut initialiser le serveur" << endl;
		f_exit();
		return 1;
	}
	if (MyServer->start() != 0)
	{
		cerr << "ne peut demarrer le serveur" << endl;
		f_exit();
		return 1;
	}
	f_exit();
	return 0;
}