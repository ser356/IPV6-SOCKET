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
    char *DEFAULT_MSG = "Mensaje por defecto";
    char *DEFAULT_IP = "ff02::1";
    char *DEFAULT_IF = "eth0";
    int DEFAULT_PORT = 54321;
    int DEFAULT_HOPS = 1;
    int DEFAULT_DELAY = 1;
    

    socklen_t			len;
    // Socket IPV6 TCP
    int sockfd;
    // Interfaz
    int if_indice;
    // Estructura para la dirección del servidor
    //dynamic memory allocation char **values 7 strings
    char values[7][100];
    char msg[100];
    struct sockaddr_in6 servaddr,clientaddress;

	char equipo [INET6_ADDRSTRLEN];

    if (argc != 7)
    {
        // Si no se introducen los siete argumentos necesarios, se asignará un valor por defecto
        printf("Error en los argumentos, se asignarán valores por defecto\n");
        strcpy(values[0], DEFAULT_MSG);
        strcpy(values[1], DEFAULT_IP);
        strcpy(values[2], DEFAULT_IF);
        sprintf(values[3], "%d", DEFAULT_PORT);
        sprintf(values[4], "%d", DEFAULT_HOPS);
        sprintf(values[5], "%d", DEFAULT_DELAY);

    }
    else
    {
        //Definimos el socket como UDPv6
        if ((sockfd = socket(AF_INET6, SOCK_DGRAM, 0)) < 0)
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
        servaddr.sin6_addr = in6addr_any;
        // Asignamos la dirección IP
        inet_pton(AF_INET6, argv[2], &servaddr.sin6_addr);
        // Asignamos el interfaz
        if_indice = if_nametoindex(argv[3]);
        // Validamos el interfaz
        if (setsockopt(sockfd, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, &if_indice, sizeof(if_indice)) < 0)
        {
            perror("Error en la asignación del interfaz\n");
            exit(1);
        }


        // Asignamos el número de saltos
        int hops = atoi(argv[5]);
        if (setsockopt(sockfd, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, &hops, sizeof(hops)) < 0)
        {
            perror("Error en la asignación del número de saltos\n");
            exit(1);
        }
        
        
        //bind
        if (bind(sockfd, (SA *) &servaddr,len) < 0) {
		perror ("Error en bind\n");
		exit (-1);
		}
        // Enviamos el mensaje
        if (sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        {
            perror("Error en el envío del mensaje\n");
            exit(1);
        }
        // Cerramos el socket
        close(sockfd);

        

    
    }
}
