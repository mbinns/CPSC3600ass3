#include "set.h"
#include "common.h"


struct config
{
    const char * addr;
    const char * port;
};
struct request
{
	const char * method;
	const char * path;
	const char * protocol;
	const char * host;
};

struct response
{
	//have to have below this
	const char * close;
	const char * date;
	const char * modified;
	const char * content_len;
	const char * content_typ;
	const char * server_head;
};



int parse_args(int argc, char * argv[], struct config * cfg);

void terminate(int signum);

//Keep state information
struct set st;
struct addrinfo * addr_list;

long long connections = 0;
int connfd;

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
    
    // accept incoming connections
    while(1)
    {
        struct sockaddr remote;
        socklen_t rlen = sizeof(remote);
        connfd = tcp_accept(sockfd,&remote,&rlen);
        
        connections++;
        set_add(&st,get_addr(&remote));

        printf("Connection:%lld\n",connections);
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

int parse_head(char * msg, struct request * req){
	const char b[1] = " ";
	const char s[2] = ":";
	const char r[3] = "\r";
	const char n[3] = "\n";
	char * token;
	
	/* get the first token */
	token = strtok(msg,b);
	strcpy(req->method,token);
	
	if(!strcmp(req->method, "GET")){
		fprintf(stderr, "Invalid method error 405");
		return 1;
	}
	token = strtok(NULL,b);
	strcpy(req->path,token);

	token = strtok(NULL,r);
	strcpy(req->protocol,token);
	
	/* walk through other tokens */
	while( token != NULL )
	{
		token = strtok(NULL, s);
		if(strcmp(token, "Host")){
			req->host = strtok(NULL,r);
			if(req->host == NULL){
				fprintf(stderr, "Error. No Host Provided.");
				return 1;
			}
		}else
			token = strtok(NULL,s);
		
	}
	if(req->host == NULL){
		fprintf(stderr, "Error. No Host Provided.");
		return 1;
	}
	return 0;
}

void terminate(int signum)
{
    printf("%lld\t", connections);

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
