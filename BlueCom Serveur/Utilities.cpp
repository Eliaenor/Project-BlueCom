#include "stdafx.h"

int get_ID(char *str)
{
	int   ret;
	char  ID[11];
	int   i = 4;
	int   y = 0;

	while (str[i] != '/')
	{
			ID[y] = str[i];
			y++; i++;
	}
	ID[y] = '\0';
	ret = atoi(ID);
	return ret;
}

int	ft_strinitSerial(char* str, int size)
{
	int i = 0;

	while (i < size)
	{
		str[i] = '0';
		i++;
	}
	str[i - 1] = '\0';
	return 0;
}

int ft_strcpy(char* tbl, const char* str, int i)
{
	int x = 0;

	for (x = 0; str[x] != '\0'; x++)
	{
		tbl[i] = str[x];
		i++;
	}
	tbl[i] = str[x];
	return 0;
};

int ft_strlen(char* str)
{
	int i = 0;
	if (str != NULL)
	{
		while (str[i] != 4 || str[i + 1] != '\0')
			i++;
		return i + 1;
	}
	return 0;
};

template<>
char * serial<User*>(User* var, char* data)
{
	if (strlen(data) < 292)
	{
		cout << "Serial : Error with Var2, please make sure that Var2 is 292 octects sized and NULL terminated." << endl;
		return NULL;
	}
	ft_strcpy(data, var->login.c_str(), 0);
	ft_strcpy(data, var->description.c_str(), 32);
	data[288] = var->channel;
	data[289] = var->ID;
	data[290] = var->user_type;
	data[291] = 4;
	cout << "Serialize User : " << var->login << "(" << var->TID << ")" << endl;
	return data;
}

template<>
char* serial<Channel*>(Channel *var, char* data)
{
	if (strlen(data) < 291)
	{
		cout << "Serial : Error with Var2, please make sure that Var2 is 291 octects sized and NULL terminated." << endl;
		return NULL;
	}
	ft_strcpy(data, var->name.c_str(), 0);
	ft_strcpy(data, var->desc.c_str(), 32);
	data[288] = var->ID;
	if (var->parent == NULL)
		data[289] = 0;
	else
		data[289] = var->parent->ID;
	data[290] = 4;
	cout << "serialize Channel : " << var->name << "(" << var->ID << ")" << endl;
	return data;
}


template<>
int unserial<User*>(User* var, char *data)
{
	if (ft_strlen(data) < 292)
	{
		cout << "Unserial : Error with Var2, please make sure that Var2 is 292 octects sized and NULL terminated." << endl;
		return NULL;
	}
	var->login = data;
	var->description = data + 32;
	var->channel = data[288];
	var->ID = data[289];
	_itoa_s(var->ID, var->TID, 10);
	var->user_type = data[290];
	cout << "Unserialize User : " << var->login << "(" << var->TID << ")" << endl;
	return 0;
}

template<>
int unserial<Channel*>(Channel *var, char *data)
{
	int ret;

	int test = ft_strlen(data);
	if (ft_strlen(data) < 291)
	{
		cout << "Unserial : Error with Var2, please make sure that Var2 is 291 octects sized and NULL terminated." << endl;
		return 0;
	}
	var->name = data;
	var->desc = data + 32;
	var->ID = data[288];
	ret = data[289];
	cout << "Unserialize Channel : " << var->name << "(" << var->ID << ")" << endl;
	return ret;
}