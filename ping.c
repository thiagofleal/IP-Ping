#include "ping.h"
#include <icmpapi.h>

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
