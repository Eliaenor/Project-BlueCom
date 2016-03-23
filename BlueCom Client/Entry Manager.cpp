#include "stdafx.h"

void cmdManager(char * buff, ClassClient *MyClient)
{
	if (strcmp(buff, "/start") == 0)
		MyClient->startRecord();
	else if (strcmp(buff, "/stop") == 0)
	{
		MyClient->stopRecord();
		send(MyClient->getSocket(), "NFO/009", 8, NULL);
		if (getRecordPlay() == 1)
		{
			LPDIRECTSOUNDBUFFER *lplpDsb = MyClient->getLPDSB(1);
			(*lplpDsb)->Stop();
		}
	}
	else if (strcmp(buff, "/play") == 0)
		MyClient->createPlay(MyClient->getLPDSB(1), 1);
	else if (strcmp(buff, "/unplay") == 0)
		MyClient->destroyPlay(MyClient->getLPDSB(1), 1);

	/*else if (strcmp(buff, "/join") == 0)
		MyClient->destroyPlay(MyClient->getLPDSB1(), 1);
	else if (strcmp(buff, "/quit") == 0)
		MyClient->destroyPlay(MyClient->getLPDSB(1), 1);
	else if (strcmp(buff, "/kick") == 0)
		MyClient->destroyPlay(MyClient->getLPDSB(1), 1);
	else if (strcmp(buff, "/ban") == 0)
		MyClient->destroyPlay(MyClient->getLPDSB(1), 1);*/
	return;
}

void entryManager(char* buff, ClassClient *MyClient)
{
	if (buff[0] == '/')
		cmdManager(buff, MyClient);
	else
		send(MyClient->getSocket(), buff, (strlen(buff) + 1), NULL);
	return;
}