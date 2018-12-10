#include <winsock2.h>
#include <iphlpapi.h>
#include <icmpapi.h>

typedef ICMP_ECHO_REPLY ping_t;

typedef struct{
	string ip;
	string name;
	int status;
}IP_status;

extern void beep(int number);
extern void wait_beep(bool active);
extern IP_status* new_IP_status(string ip, string name, int status);
extern void free_IP_status(IP_status* point);
extern ping_t* ping(string, unsigned int);
extern void freeStringArray(pointer array);
