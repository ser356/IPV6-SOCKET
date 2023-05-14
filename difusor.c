/*
** Fichero: multicast.c
** Autores: Sergio García Seco
** Usuario: i0791431
** Función: Difusor de mensajes
** DNI del alumno:
*/
#include "difusor.h"
void sigint_handler();
int registroSIGNAL();
int sockfd; // Socket de conexión
int main(int argc, char **argv)
{
    if (registroSIGNAL() == -1)
    {
        perror("Error en el registro de la señal SIGINT\n");
        exit(-1);
    }
    // Socket IPV6 TCP
    char *msg, *ip, *ifname, *port, *hops, *delay;
    int  if_indice;

    struct sockaddr_in6 servaddr, cliaddr;
    struct ipv6_mreq ipv6mreq;

    if (argc != 7)
    {

        msg = DEFAULT_MSG;
        ip = DEFAULT_IP;
        ifname = DEFAULT_IF;
        port = DEFAULT_PORT;
        hops = DEFAULT_HOPS;
        delay = DEFAULT_DELAY;

        fprintf(stdout, "Currently using default values:\n");
        fprintf(stdout, "Message: %s\n", msg);
        fprintf(stdout, "IP: %s\n", ip);
        fprintf(stdout, "Interface: %s\n", ifname);
        fprintf(stdout, "Port: %s\n", port);
        fprintf(stdout, "Hops: %s\n", hops);
        fprintf(stdout, "Delay: %s\n", delay);
    }
    else
    {
        msg = argv[1];
        ip = argv[2];
        ifname = argv[3];
        port = argv[4];
        hops = argv[5];
        delay = argv[6];
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
    cliaddr.sin6_port = htons(atoi(port));
    if (inet_pton(AF_INET6, ip, &cliaddr.sin6_addr) <= 0)
    {
        perror("Error en la asignación de la dirección IP\n");
        exit(1);
    }
    bzero(&ipv6mreq, sizeof(ipv6mreq));
    if (inet_pton(AF_INET6, ip, &ipv6mreq.ipv6mr_multiaddr) == -1)
    {
        perror("inet_pton2: error while converting the address \n");
        exit(-3);
    }
    if ((if_indice = if_nametoindex(ifname)) == 0)
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
    if (setsockopt(sockfd, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, hops, sizeof(hops)) == -1)
    {
        perror("setsockopt: error while setting the hops\n");
        exit(-6);
    }
    while (1)
    {
        if (sendto(sockfd, msg, strlen(msg), 0, (SA *)&cliaddr, sizeof(cliaddr)) < 0)
        {
            perror("Error en el envío del mensaje\n");
            exit(1);
        }
        else
        {
            fprintf(stdout, "Message sent: %s\n", msg);
        }

        sleep(atoi(delay));
    }
}

// Registro de SIGINT
int registroSIGNAL()
{
    struct sigaction action;
    action.sa_handler = sigint_handler;
    action.sa_flags = 0;

    int var = sigemptyset(&(action.sa_mask));

    var += sigaction(SIGINT, &action, NULL);
    return var == 0 ? 0 : -1;
}

// Manejador de SIGINT
void sigint_handler()
{
    fprintf(stderr, "\nCatched SIGINT!\n");
    close(sockfd);
    exit(0);
}