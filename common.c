#include "common.h"

double get_wall_time()
{
    struct timeval time;
    if (gettimeofday(&time,NULL))
        return 0;

    return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

const char * get_addr(struct sockaddr * remote)
{
    struct sockaddr_in * raddr = (struct sockaddr_in *)remote;

    // use library function to get string representation
    return inet_ntoa(raddr->sin_addr);
}

int create_tcp_addrinfo(struct addrinfo ** addr_list, const char * host, const char * port)
{
    // define and give memory to address structure
    struct addrinfo addr_skel;
    memset(&addr_skel, 0, sizeof(addr_skel));

    // set values for structure to handle UDP requests
    addr_skel.ai_family = AF_UNSPEC;
    addr_skel.ai_flags = AI_PASSIVE;
    addr_skel.ai_socktype = SOCK_STREAM;
    addr_skel.ai_protocol = IPPROTO_TCP;

    // create linked list of addrinfo structs
    int ret = getaddrinfo(host, port, &addr_skel, addr_list);

    if (ret != 0)
    {
        if (errno == 0)
            fprintf(stderr, "getaddrinfo failed\n");
        else
            perror("getaddrinfo failed");
        return ret;
    }

    return 0;
}

int create_socket(struct addrinfo * addr)
{
    // create a socket from the addr info
    int fd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);

    if (fd < 0)
    {
        perror("socket failed");
        return fd;
    }

    return fd;
}

int bind_addr(int sockfd, struct addrinfo * addr)
{
    // bind to address info
    int ret = bind(sockfd, addr->ai_addr, addr->ai_addrlen);

    if (ret != 0)
    {
        perror("bind failed");
        return ret;
    }

    return 0;
}

int listen_socket(int sockfd)
{
    // listen to socket info
    int ret = listen(sockfd, 5);

    if (ret != 0)
    {
        perror("listen failed");
        return ret;
    }

    return 0;
}

int tcp_accept(int sockfd, struct sockaddr * addr, socklen_t * rlen)
{
    // accept a connection
    int newfd = accept(sockfd, addr, rlen);

    if (newfd < 0)
    {
        perror("accept failed");
        return newfd;
    }

    return newfd;
}

int tcp_connect(int sockfd, struct addrinfo * addr)
{
    // connect to address info
    int ret = connect(sockfd, addr->ai_addr, addr->ai_addrlen);

    if (ret != 0)
    {
        perror("connect failed");
        return ret;
    }

    return 0;
}
