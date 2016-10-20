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
int connfd[CONN_MAX];

//Keep state information
struct set st;
struct addrinfo * addr_list;

long long connections = 0;

int main(int argc, char * argv[])
{
    //char buf[BUF_MAX];
    
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
    
    // set all connections slots to -1 saying their are no connections
    int slot = 0;
    for(int i = 0; i < CONN_MAX; i++)
    {
        connfd[i] = -1;
    }

    // accept incoming connections
    while(1)
    {
        struct sockaddr remote;
        socklen_t rlen = sizeof(remote);

        connfd[slot] = tcp_accept(sockfd,&remote,&rlen);
        
        if(connfd[slot] < 0)
        {
           fprintf(stderr,"Could no accept connection");
        }
        else
        {
            connections++;
            set_add(&st,get_addr(&remote));
            if(fork() == 0)
            {
                printf("Hello :)");
                exit(0);
            }
        }        
        while (connfd[slot]!=-1) slot = (slot+1)%CONN_MAX;
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
