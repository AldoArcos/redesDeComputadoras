#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>

//programacion avanzada en unix


void *rutinaServidor(void *arg)
{
    
}


int main()
{
    //empezamos con la declaración de los threads
    pthread_t thread1;
    //pthread_t thread2;

    int value1 = 100;

    //variables del programa original
    int udp_socket, lbind, tam = 0, lrecv;
    struct sockaddr_in local, remota;
    unsigned char msj[511];
    unsigned char paqRec[512];
    udp_socket = socket(AF_INET, SOCK_DGRAM, 0);


    //vemos si el hilo se crea
    if(pthread_create(&thread1, NULL, rutinaServidor, &value1))
    {
        perror("Hubo un error al crear el hilo\n");
        return -1;
    }


    if(udp_socket == -1)
    {
        perror("\nError al abrir el socket");
    }
    else
    {
        perror("\nExito al abrir el socket");
        local.sin_family = AF_INET;
        //el cliente siempre pone 0 para que se nos dé un puerto efímero
        local.sin_port = htons(0);      
        local.sin_addr.s_addr = INADDR_ANY;
        //se relaciona mi estructura local con el socket
        lbind = bind(udp_socket, (struct sockaddr *)&local, sizeof(local));

        if(lbind == -1)
        {
            perror("\nError en bind");
            exit(0);
        }
        else
        {
            perror("\nExito en bind");
            //scanf("%[^\n]", msj);
            remota.sin_family = AF_INET;
            remota.sin_port = htons(8080);                          //si no sirve, cambiar a 8080, estaba en 53
            remota.sin_addr.s_addr = inet_addr("172.100.95.81");     //cambiar a mi dirección ip, estaba en 8.8.8.8
            //iniciamos el ciclo, capturamos el mensaje
            while(1)
            {
                printf("\nEscribe el mensaje: ");
                fflush(stdin);
                //scanf("%[^\n]", msj);
                fgets(msj, 511, stdin);
                tam = sendto(udp_socket, msj, strlen(msj) + 1, 0, (struct  sockaddr *)&remota, sizeof(remota));
                if(tam == -1)
                {
                    perror("\nError al enviar el mensaje");
                }
                else
                {
                    lrecv = sizeof(remota);
                    tam = recvfrom(udp_socket, paqRec, 512, 0, (struct sockaddr *)&remota, &lrecv);

                    
                    if(tam == -1)
                    {
                        perror("\nError al recibir el mensaje");
                    }
                    else
                    {

                        printf("\nEl server dice: %s", paqRec);
                    }
                }
            }
            
        }
    }
    close(udp_socket);

    return 0;
}