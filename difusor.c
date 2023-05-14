/*
** Fichero: multicast.c
** Autores: Sergio García Seco
** Usuario: i0791431
** Función: Difusor de mensajes
** DNI del alumno:
*/
#include "difusor.h"
void sigint_handler(int sig);
int registroSIGNAL();

int main(int argc, char **argv)
{
    if(registroSIGNAL()==-1){
        perror("Error en el registro de la señal SIGINT\n");
        exit(-1);
    }
    // Socket IPV6 TCP
    char var[7][100];
    int sockfd, if_indice;

    struct sockaddr_in6 servaddr, cliaddr;
    struct ipv6_mreq ipv6mreq;

    if (argc != 7)
    {
        printf("Error en los argumentos, se asignarán valores por defecto\n");
        strcpy(var[0], DEFAULT_MSG);
        strcpy(var[1], DEFAULT_IP);
        strcpy(var[2], DEFAULT_IF);
        strcpy(var[3], DEFAULT_PORT);
        strcpy(var[4], DEFAULT_HOPS);
        strcpy(var[5], DEFAULT_DELAY);
    }
    else
    {
        strcpy(var[0], argv[1]);
        strcpy(var[1], argv[2]);
        strcpy(var[2], argv[3]);
        strcpy(var[3], argv[4]);
        strcpy(var[4], argv[5]);
        strcpy(var[5], argv[6]);
    }
    if ((sockfd = socket(AF_INET6, SOCK_DGRAM, 0)) < 0)
    {
        perror("Error en la creación del socket\n");
        exit(1);
    }
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin6_family = AF_INET6;
    servaddr.sin6_addr = in6addr_any;
    bzero(&cliaddr, sizeof(cliaddr));

    cliaddr.sin6_family = AF_INET6;
    cliaddr.sin6_port = htons(atoi(var[3]));
    if (inet_pton(AF_INET6, var[1], &cliaddr.sin6_addr) <= 0)
    {
        perror("Error en la asignación de la dirección IP\n");
        exit(1);
    }
    bzero(&ipv6mreq, sizeof(ipv6mreq));
    if (inet_pton(AF_INET6, var[1], &ipv6mreq.ipv6mr_multiaddr) == -1)
    {
        perror("inet_pton2: error while converting the address \n");
        exit(-3);
    }
    if ((if_indice = if_nametoindex(var[2])) == 0)
    {
        perror("if_nametoindex error\n");
        exit(-4);
    }
    ipv6mreq.ipv6mr_interface = if_indice;
    if (setsockopt(sockfd, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, &ipv6mreq, sizeof(ipv6mreq)) == -1)
    {
        perror("setsockopt: error while adding the membership to the group\n");
        exit(-5);
    }
    while (1)
    {
        if (sendto(sockfd, var[0], strlen(var[0]), 0, (SA *)&cliaddr, sizeof(cliaddr)) < 0)
        {
            perror("Error en el envío del mensaje\n");
            exit(1);
        }
        sleep(atoi(var[5]));
    }
}

// Registro de SIGINT
int registroSIGNAL()
{
    struct sigaction action,old;
    action.sa_handler = sigint_handler;
    action.sa_flags = 0;
   
    int var = sigemptyset(&(action.sa_mask));

    var += sigaction(SIGINT, &action, &old);
    return var==0?0:-1;
}

// Manejador de SIGINT
void sigint_handler(int sig)
{
    fprintf(stderr, "\nSe ha recibido la señal SIGINT\n");
    exit(0);
}