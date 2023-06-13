#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

int main()
{
    int udp_socket;

    udp_socket = socket(AF_INET, SOCK_DGRAM, 0);

    if(udp_socket == -1)
    {
        perror("\nError al abrir el socket");
    }
    else
    {
        perror("\nExito al abrir el socket");
    }
    close(udp_socket);

    return 0;
}