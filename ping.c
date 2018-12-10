#include <Tonight\tonight.h>
#include <Tonight\list.h>
#include "ping.h"

ping_t* ping(string argv, unsigned int timeout)
{
	ping_t* pEchoReply;
	
	HANDLE hIcmpFile;
	unsigned long ipaddr = INADDR_NONE;
	DWORD dwRetVal = 0;
	char SendData[32] = "Data Buffer";
	LPVOID ReplyBuffer = NULL;
	DWORD ReplySize = 0;
	
	ipaddr = inet_addr(argv);
	hIcmpFile = IcmpCreateFile();
	
	if (hIcmpFile == INVALID_HANDLE_VALUE)
	{
		return NULL;
	}
	
	ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData);
	ReplyBuffer = Memory.alloc(ReplySize);
	
	if (ReplyBuffer == NULL)
	{
		return NULL;
	}
	
	dwRetVal = IcmpSendEcho(
		hIcmpFile,
		ipaddr,
		SendData,
		sizeof(SendData),
		NULL,
		ReplyBuffer,
		ReplySize,
		timeout
	);
	
	if (dwRetVal != 0)
	{
		pEchoReply = (ping_t*)ReplyBuffer;
		struct in_addr ReplyAddr;
		ReplyAddr.S_un.S_addr = pEchoReply->Address;
		
		return pEchoReply;
	}
	else
	{
		return NULL;
	}
}

void beep(int number)
{
	Writer out = new Writer(Tonight.Std.Console.Output);
	int i;
	
	for(i = 0; i < number; i++)
	{
		out.text("\a");
		Tonight.sleep(500);
	}
}

void wait_beep(bool active)
{
	if(active)
	{
		while(!Tonight.pressKey())
		{
			beep(3);
			Tonight.sleep(1000);
		}
		
		while(Tonight.pressKey())
		{
			Tonight.getKey();
		}
	}
}

IP_status* new_IP_status(string ip, string name, int status)
{
	IP_status* _new = Memory.alloc(sizeof(IP_status));
	_new->ip = String.copy(ip);
	_new->name = String.copy(name);
	_new->status = status;
	return _new;
}

void free_IP_status(IP_status* point)
{
	if(point)
	{
		String.free(point->ip);
		String.free(point->name);
		Memory.free(point);
	}
}

void freeStringArray(pointer array)
{
	string s;
	
	foreach(s $in array)
	{
		String.free(s);
	}
	
	Array.free(array);
}
