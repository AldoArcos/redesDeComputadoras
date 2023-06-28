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
#include <sys/time.h>
#include <unistd.h>
#include "sqlite3.h"

unsigned char tramaEnv[1514], tramaRec[1514];
unsigned char IPOrigen[6];
unsigned char NetMask[6];
//encabezado MAC
unsigned char MACbc[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
unsigned char MACorigen[6];
//encabezado ethertype para el protocolo ARP (0806)
unsigned char etherARP[2] = {0x08, 0x06};
//mensaje de ARP
unsigned char HardwareType[2] = {0x00, 0x01};
unsigned char ProtocolType[2] = {0x08, 0x00};
//codigo de operacion 0x0001 para solicitud, 0x002 para respuesta
unsigned char OpSolARP[2] = {0x00, 0x01};
unsigned char OpRespARP[2] = {0x00, 0x02};
unsigned char THA[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned char TPA[4];
//unsigned char devices[15][100];
int cont = 0;

const char *filenameDB = "/home/aldo/Desktop/redes/p3/db/DBp3";

void imprimirEnArchivo() {
    FILE *archivo;

    archivo = fopen("scanner.txt", "a"); 

    if (archivo == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return;
    }

    fprintf(archivo, "%d.%d.%d.%d  %.2x:%.2x:%.2x:%.2x:%.2x:%.2x \n)", TPA[0], TPA[1], TPA[2], TPA[3], THA[0], THA[1], THA[2], THA[3], THA[4], THA[5]);

    fclose(archivo); 
}


void insertData(sqlite3 *db)
{
    char query[100];
    sprintf(query, "INSERT INTO devices (tpa, tha) VALUES ('%d.%d.%d.%d','%.2x:%.2x:%.2x:%.2x:%.2x:%.2x')", TPA[0], TPA[1], TPA[2], TPA[3], THA[0], THA[1], THA[2], THA[3], THA[4], THA[5]);
    printf("QUERY GENERADA: %s", query);
    sqlite3_exec(db, query, NULL, NULL, NULL);
}

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

void recibirTrama(int ds, unsigned char * trama, sqlite3 *db)
{
    FILE *archivo;
    char cadena[] = "¡Hola, mundo!";
    int tam, flag = 0;
    struct timeval start, end;
    long mtime = 0, seconds, useconds;
    gettimeofday(&start, NULL);

    while(mtime < 200)
    {
        tam = recvfrom(ds, trama, 1514, MSG_DONTWAIT, NULL, 0);
        if(tam == -1)
        {
            //perror("\nError al recibir");
        }
        else
        {
            if(memcmp(trama + 0, MACorigen, 6) == 0 && memcmp(trama + 12, etherARP, 2) == 0 && memcmp(trama + 28, TPA, 4)==0 && memcmp(trama + 20, OpRespARP,2) == 0 && memcmp(trama + 38, IPOrigen, 4) == 0)
            {
                memcpy(THA, trama + 6, 6);
              //  memcpy(devices[0][cont], THA, 6);
               /* printf("\n\nLa dirección MAC de la IP ");
                for(int i = 0 ; i < 4 ; i++)
                    printf("%d.", TPA[i]);
                printf(" es: ");*/
                imprimirTrama(THA, 6);
                insertData(db);
                imprimirEnArchivo();
                flag = 1;
                cont++;
            } 
            gettimeofday(&end, NULL);
            seconds = end.tv_sec - start.tv_sec;
            useconds = end.tv_usec - start.tv_usec;
            mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;   

            if(flag == 1)   
                break;     
        }
    }
    printf("\nelapsed time: %ld milliseconds\n", mtime); 
}

void enviarTrama(int ds, unsigned char *trama, int index)
{
    int tam;
    struct sockaddr_ll interfaz;
    memset(&interfaz, 0x00, sizeof(interfaz));
    interfaz.sll_family = AF_PACKET;
    interfaz.sll_protocol = htons(ETH_P_ALL);
    interfaz.sll_ifindex = index;
    tam = sendto(ds, trama, 42, 0, (struct sockaddr *)&interfaz, sizeof(interfaz));
    
    if(tam == -1)
    {
        perror("\nError al enviar.");
        exit(0);
    }
    else
    {
        //printf("\nExito al enviar.");
        return;
    }
    
    
}

void estructuraTrama(unsigned char *trama, int index)
{
    //encabezado MAC
    memcpy(trama + 0, MACbc, 6);
    memcpy(trama + 6, MACorigen, 6);
    //memcpy(trama + 12, ethertype, 2);
    memcpy(trama + 12, etherARP, 2);

    //mensaje ARP
    //se utiliza 0001 para inicar que es del tipo ethernet
    memcpy(trama + 14, HardwareType, 2);
    memcpy(trama + 16, ProtocolType, 2);
    trama[18] = 6;
    trama[19] = 4;
    memcpy(trama + 20, OpSolARP, 2);
    memcpy(trama + 22, MACorigen, 6);
    memcpy(trama + 28, IPOrigen, 4);
    memcpy(trama + 32, THA, 6);
    memcpy(trama + 38, TPA, 4);

    printf("Solicitud ARP:\n");
    imprimirTrama(trama, 60);
    printf("\n\n");

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
        indexloc = nic.ifr_ifindex;
        printf("\nEl indice es: %d\n", indexloc);
        
        if(ioctl(ds, SIOCGIFHWADDR, &nic) == -1)
        {
           // perror("\nError al obtener la MAC.");
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
                //indexloc = nic.ifr_ifindex;
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
    //parte de la DB
    sqlite3 *db = NULL;

    if(sqlite3_open(filenameDB, &db) != SQLITE_OK)
    {
        printf("Error al abrir la base de datos: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    if(packet_socket == -1)
    {
        perror("\nError al abrir el socket");
        exit(0);
    }
    else
    {
        //perror("\nExito al abrir el socket");
        index = obtenerDatos(packet_socket);
        unsigned char lastByte;
        memcpy(TPA, IPOrigen, 3);
        for(int i = 0 ; i < 255 ; i++)
        {
            lastByte = (unsigned char) i;
            TPA[3] = lastByte;
            estructuraTrama(tramaEnv, index);
            enviarTrama(packet_socket, tramaEnv, index);
            recibirTrama(packet_socket, tramaRec, db);
        }
    }

    close(packet_socket); 
    sqlite3_close(db);

    return 0;
}
