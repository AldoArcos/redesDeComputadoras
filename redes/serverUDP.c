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
    int udp_socket, lbind, tam, lrecv;
    struct sockaddr_in server, cliente;
    unsigned char paqRec[512];

    udp_socket = socket(AF_INET, SOCK_DGRAM, 0);

    if(udp_socket == -1)
    {
        perror("\nError al abrir el socket");
    }
    else
    {
        perror("\nExito al abrir el socket");
        server.sin_family = AF_INET;
        server.sin_port = htons(8080);
        server.sin_addr.s_addr = INADDR_ANY;
        lbind = bind(udp_socket, (struct sockaddr *)&server, sizeof(server));
        if(lbind == -1)
        {
            perror("\nError en bind");
            exit(0);
        }
        else
        {
            perror("\nExito en bind");
            lrecv = sizeof(cliente);
            //el tamaño max recomendado para el buffer es de 512, el cero es una bandera (funciona de forma estandar o bloqueante)
            //del manual 
            tam = recvfrom(udp_socket, paqRec, 512, 0, (struct sockaddr *)&cliente, &lrecv);
            if(tam == -1)
            {
                perror("\nError al recibir el mensaje");
            }
            else
            {
                printf("\nEl mensaje es: %s", paqRec);
            }
        }
    }
    close(udp_socket);

    return 0;
}