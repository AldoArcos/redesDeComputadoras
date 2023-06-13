#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <string.h>

//programacion avanzada en unix

int main()
{
    int udp_socket, lbind, tam = 0, lrecv;
    struct sockaddr_in server, cliente;
    unsigned char paqRec[512], msj[512];

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
            //perror("\nExito en bind");
            //iniciamos el bucle
            while(1)
            {
                    lrecv = sizeof(cliente);
                    tam = recvfrom(udp_socket, paqRec,512, 0, (struct sockaddr *)&cliente, &lrecv);

                
                if(tam == -1)
                {
                    perror("\nError al recibir el mensaje");
                }
                else
                {
                    printf("\nEl cilente dice: %s", paqRec);
                    printf("\nEscriba su mensaje: ");
                    fgets(msj, 511, stdin);
                    tam = sendto(udp_socket, msj, strlen(msj) + 1, 0, (struct  sockaddr *)&cliente, sizeof(cliente));
                    
                    if(tam == -1)
                    {
                        perror("\nError al enviar el mensaje");
                    }
                    else
                    {
                        //printf("\nExito al enviar");
                    }

                }
            }
            
        }
    }
    close(udp_socket);

    return 0;
}