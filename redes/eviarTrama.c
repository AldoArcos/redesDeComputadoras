#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
// ethertype = 0x0c0c en wireshark para poder escanear

unsigned char tramaEnv[1514], tramaRec[514];
unsigned char MACbc[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
unsigned char ethertype[2] = {0x0c, 0x0c};
char nombre[1500] = "Arcos Hermida Aldo Alejandro";

unsigned char MACorigen[6];
unsigned char IPOrigen[6];
unsigned char NetMask[6];

void imprimirTrama(unsigned char * paq, int len)
{
    for(int i = 0 ; i < len ; i++)
    {
        if(i%16 == 0)
        {
            printf("\n");
        }
        printf("%.2x:", paq[i]);
    }
    printf("\n");
}

void recibirTrama(int ds, unsigned char * trama)
{
    int tam;

    while(1)
    {
        tam = recvfrom(ds, trama + 6, 1524, 0, NULL, 0);
        if(tam == -1)
        {
            perror("\nError al recibir");
            exit(0);
        }
        else
        {
            if(!memcmp(trama + 0,   MACorigen, 6))
            {
                imprimirTrama(trama, tam);
                //break;
            }
                
        }
    }
    
}

void enviarTrama(int ds, unsigned char *trama, int index)
{
    int tam;
    struct sockaddr_ll interfaz;
    memset(&interfaz, 0x00, sizeof(interfaz));
    interfaz.sll_family = AF_PACKET;
    interfaz.sll_protocol = htons(ETH_P_ALL);
    interfaz.sll_ifindex = index;
    tam = sendto(ds, trama, 60, 0, (struct sockaddr *)&interfaz, sizeof(interfaz));
    
    if(tam == -1)
    {
        perror("\nError al enviar.");
        exit(0);
    }
    else
    {
        perror("\nExito al enviar.");
    }
     
}

void estructuraTrama(unsigned char *trama, int index)
{
    memcpy(trama + 0, MACbc, 6);
    memcpy(trama + 6, MACorigen, 6);
    memcpy(trama + 12, ethertype, 2);
    memcpy(trama + 14, nombre, sizeof(nombre));
}


int obtenerDatos(int ds)
{
    struct ifreq nic;
    unsigned char nombre[20];
    int indexloc;

    printf("Inserta el nombre de la interfaz: ");
    gets(nombre);
    strcpy(nic.ifr_name, nombre);

    if(ioctl(ds, SIOCGIFINDEX, &nic) == -1)
    {
        perror("\nError al obtener el index.");
        exit(0);
    }
    else
    {
        printf("\nEl indice es: %d\n", nic.ifr_ifindex);
        
        if(ioctl(ds, SIOCGIFHWADDR, &nic) == -1)
        {
            perror("\nError al obtener la MAC.");
            exit(0);
        }
        else
        {
            memcpy(MACorigen, nic.ifr_hwaddr.sa_data, 6);
            printf("La dirección MAC origen es: ");
            for(int i = 0 ; i < 6 ; i++)
                printf("%.2x:", MACorigen[i]);
            
            printf("\n");

            //imprimir la ip
            if(ioctl(ds, SIOCGIFADDR, &nic) == -1)
            {
                perror("\nError al obtener la IP.");
                exit(0);
            }
            else
            {
                indexloc = nic.ifr_ifindex;
                memcpy(IPOrigen, nic.ifr_addr.sa_data + 2, 4);
                printf("La dirección IP origen es: ");
                for(int i = 0 ; i < 4 ; i++)
                    printf("%d.", IPOrigen[i]);
                
                printf("\n");

                //obtener la netmask
                if(ioctl(ds, SIOCGIFNETMASK, &nic) == -1)
                {
                    perror("\nError al obtener la netmask.");
                    exit(0);
                }
                else
                {
                    memcpy(NetMask, nic.ifr_netmask.sa_data + 2, 4);
                    printf("La Netmask es: ");
                    for(int i = 0 ; i < 4 ; i++)
                        printf("%d.", NetMask[i]);
                    
                    printf("\n");

                }

            }
        }
    }

    return indexloc;
}

int main()
{
    int packet_socket, index;
    packet_socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

    if(packet_socket == -1)
    {
        perror("\nError al abrir el socket");
        exit(0);
    }
    else
    {
        perror("\nExito al abrir el socket");
        index = obtenerDatos(packet_socket);
        //estructuraTrama(tramaEnv);
        //enviarTrama(packet_Socket, index, tramaEnv);
        recibirTrama(packet_socket, tramaRec);

    }

   // estructuraTrama(tramaEnv, index);

    close(packet_socket); 
    return 0;
}