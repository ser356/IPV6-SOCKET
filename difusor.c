/*
** Fichero: multicast.c
** Autores: Sergio García Seco
** Usuario: i0791431
** Función: Difusor de mensajes
** DNI del alumno: 
*/
#include "difusor.h"
int main(int argc, char **argv)
{
    // Socket IPV6 TCP
    int sockfd,if_indice;
    struct sockaddr_in6 servaddr;

    if(argc!= 7)
    {
        // Si no se introducen los siete argumentos necesarios, se asignará un valor por defecto
        printf("Error en los argumentos, se asignarán valores por defecto\n");
        strcpy(argv[1],DEFAULT_MSG);
        strcpy(argv[2],DEFAULT_IP);
        strcpy(argv[3],DEFAULT_IF);
        strcpy(argv[4],DEFAULT_PORT);
        strcpy(argv[5],DEFAULT_HOPS);
        strcpy(argv[6],DEFAULT_DELAY);
    }
    else
    {
        //Definimos el socket como UDPv6
        if((sockfd = socket(AF_INET6, SOCK_DGRAM, 0)) < 0)
        {
            perror("Error en la creación del socket\n");
            exit(1);
        }
        // Asignamos la dirección IP y el puerto
        //bzero sirve para rellenar con ceros servaddr
        bzero(&servaddr, sizeof(servaddr));
        // Asignamos la familia de direcciones, en este caso IPv6
        servaddr.sin6_family = AF_INET6;
        // Asignamos el puerto
        servaddr.sin6_port = htons(atoi(argv[4]));
        // El servidor debe escuchar en todas las interfaces de red
        servaddr.sin6_addr = in6addr_any;


    }

}
