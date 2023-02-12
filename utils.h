#include <sys/socket.h>

typedef enum TransportType
{
    TT_UNKNOWN = 0,
    TT_TCP = SOCK_STREAM,
    TT_UDP = SOCK_DGRAM
} TransportType;

#define MAX_CLIENT_DATA_SIZE 4096
#define MAX_QUEUED_CONN 10

int createListenPort(const char *address, int port, TransportType type);
bool handleListener(int socket_fd);
