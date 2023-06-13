#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/types.h>

//programacion avanzada en unix

int main()
{
    int udp_socket, lbind, tam;
    struct sockaddr_in local, remota;
    unsigned char msj[100] = "Hola red, soy Aldo";

    udp_socket = socket(AF_INET, SOCK_DGRAM, 0);

    if(udp_socket == -1)
    {
        perror("\nError al abrir el socket");
    }
    else
    {
        perror("\nExito al abrir el socket");
        local.sin_family = AF_INET;
        local.sin_port = htons(0);
        local.sin_addr.s_addr = INADDR_ANY;
        lbind = bind(udp_socket, (struct sockaddr *)&local, sizeof(local));
        if(lbind == -1)
        {
            perror("\nError en bind");
            exit(0);
        }
        else
        {
            perror("\nExito en bind");
            remota.sin_family = AF_INET;
            remota.sin_port = htons(53);
            remota.sin_addr.s_addr = inet_addr("8.8.8.8");
            tam = sendto(udp_socket, msj, 20, 0, (struct  sockaddr *)&remota, sizeof(remota));
            if(tam == -1)
            {
                perror("\nError al enviar el mensaje");
            }
            else
            {
                perror("\nExito al enviar");
            }
        }
    }
    close(udp_socket);

    return 0;
}