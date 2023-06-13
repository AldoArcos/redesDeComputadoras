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

unsigned char MACorigen[6];
unsigned char IPOrigen[6];
unsigned char NetMask[6];


void obtenerDatos(int ds)
{
    struct ifreq nic;
    unsigned char nombre[20];

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

    
}

int main()
{
    int packet_socket;
    packet_socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

    if(packet_socket == -1)
    {
        perror("\nError al abrir el socket");
        exit(0);
    }
    else
    {
        perror("\nExito al abrir el socket");
        obtenerDatos(packet_socket);
    }

    close(packet_socket); 
    return 0;
}