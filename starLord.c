#include "set.h"
#include "common.h"
#define CONN_MAX 1000

struct config
{
    const char * addr;
    const char * port;
};

int parse_args(int argc, char * argv[], struct config * cfg);
void terminate(int signum);
void respond();

//Keep state information
struct set st;
struct addrinfo * addr_list;

// connection file descriptor
int connfd;

long long connections = 0;

int main(int argc, char * argv[])
{
    // handles return values from our functions for error checking
    int ret;
    struct config cfg;

    // parse args into config structure
    ret = parse_args(argc, argv, &cfg);
    if(ret!=0)
        return 1;

    struct addrinfo * addr;

    //create tcp addrinfo for port and check for success
    ret = create_tcp_addrinfo(&addr_list, cfg.addr, cfg.port);

    if(ret != 0)
        return 1;

    // grab first address
    addr = addr_list;

    // bind to a socket and check for success
    int sockfd = create_socket(addr);

    if(sockfd < 0)
        return 1;

    // bind socket
    ret = bind_addr(sockfd, addr);

    if (ret != 0)
        return 1;

    // listen on socket
    ret = listen_socket(sockfd);

    if (ret != 0)
        return 1;

    set_init(&st);

    signal(SIGINT, &terminate);
    
    // accept incoming connections
    while(1)
    {
        struct sockaddr remote;
        socklen_t rlen = sizeof(remote);
        connfd = tcp_accept(sockfd,&remote,&rlen);
        
        connections++;
        set_add(&st,get_addr(&remote));

        printf("Connection:%lld\n",connections);
        respond();
    }

    terminate(0);
    return 0;
}

int parse_args(int argc, char * argv[], struct config * cfg)
{
    int usage = 0;

    // show usage on zero arguments
    if (argc == 1 || strcmp(argv[1], "-") == 0)
        usage = 1;

    // loop through all arguments with getopt and set variables as needed
    int c;
    while ((c = getopt(argc, argv, "p:")) != -1)
    {
        switch (c)
        {
            case 'p':
                cfg->port = optarg;
                break;

            case '?':
                usage = 1;
                break;

            default:
                usage = 1;
                break;
        }
    }

    if (usage)
    {
        fprintf(stderr, "Usage: %s -p <port>\n", argv[0]);
        return 1;
    }

    return 0;
}

void respond()
{
    char buf[99999]; 

    int len = read(connfd, buf, sizeof(buf));
    printf("\n%s\n",buf);
    len = write(connfd,buf,len);
    close(connfd);
}

void terminate(int signum)
{
    printf("\n%lld\t", connections);

    struct node * ptr = set_first(&st);
    while(ptr != NULL)
    {
        printf("%s, ", set_data(ptr));
        ptr = set_next(ptr);
    }

    printf("\n");

    set_free(&st);

    freeaddrinfo(addr_list);

    exit(0);
}
