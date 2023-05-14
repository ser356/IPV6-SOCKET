#include "suscriptor.h"

int main(int argc, char **argv)
{
    char *ip, *port, *ifname, buffer[1024];
    int sockfd, n;
    struct sockaddr_in6 servaddr, cliaddr;
    struct ipv6_mreq mreq;
    socklen_t len;

    if (argc != 3)
    {
        ip = DEFAULT_IP;
        port = DEFAULT_PORT;
        ifname = DEFAULT_IF;
    }
    else
    {
        ip = argv[1];
        port = argv[2];
        ifname = argv[3];
    }

    if ((sockfd = socket(AF_INET6, SOCK_DGRAM, 0)) < 0)
    {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }
    
    bzero(&cliaddr, sizeof(cliaddr));
    cliaddr.sin6_family = AF_INET6;
    cliaddr.sin6_addr = in6addr_any;
    cliaddr.sin6_port = htons(atoi(port));
    if (bind(sockfd, (struct sockaddr *)&cliaddr, sizeof(cliaddr)) < 0)
    {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin6_family = AF_INET6;
    servaddr.sin6_port = htons(atoi(port) - 1);
    if (inet_pton(AF_INET6, ip, &servaddr.sin6_addr) <= 0)
    {
        perror("Error converting IP address");
        exit(EXIT_FAILURE);
    }
    bzero(&mreq, sizeof(mreq));
    mreq.ipv6mr_interface = if_nametoindex(ifname);
    if (inet_pton(AF_INET6, ip, &mreq.ipv6mr_multiaddr) <= 0)
    {
        perror("Error converting IP address");
        exit(EXIT_FAILURE);
    }
    // add membership to the multicast group
    if (setsockopt(sockfd, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
    {
        perror("Error joining multicast group");
        exit(EXIT_FAILURE);
    }
     
    time_t timevar;
    time(&timevar);
    fprintf(stdout, "Socket set up on %s %s %s at %s\n", ip, port, ifname, ctime(&timevar));
    while (1)
    {
        len = sizeof(servaddr);
        if ((n = recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr *)&servaddr, &len)) < 0)
        {
            perror("Error receiving message");
            exit(EXIT_FAILURE);
        }
        buffer[n] = '\0';
        fflush(stdout);
        printf("Mensaje recibido: %s\n", buffer);
    }

    close(sockfd);
    return 0;
}