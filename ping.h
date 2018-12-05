#include <winsock2.h>
#include <iphlpapi.h>
#include <Tonight\tonight.h>

typedef ICMP_ECHO_REPLY ping_t;

extern ping_t* ping(string, unsigned int);
