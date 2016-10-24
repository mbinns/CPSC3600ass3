#include "set.h"
#include "common.h"
#include <curl/curl.h>

struct config
{
    const char * port;
}config;

struct request
{
	char * method;
	char * path;
	char * protocol;
	char * host;
}request;

struct response
{
	//have to have below this
	char * close;
	char * date;
	char * modified;
	char * content_len;
	char * content_typ;
	char * server_head;
}response;

/* HTTP response and header for a successful request.  */
static char* ok_response =
  "HTTP/1.1 200 OK\n"
  "Content-type: text/html\n"
  "\n"
  "<html>\n"
  " <body>\n"
  "  <h1>HTTP/1.1 200 OK</h1>\n"
  "  <p>This server understood your request</p>\n"
  " </body>\n"
  "</html>\n";

/* HTTP response, header, and body indicating that the we didn't
   understand the request.  */
static char* bad_request_response = 
  "HTTP/1.1 400 Bad Request\n"
  "Content-type: text/html\n"
  "\n"
  "<html>\n"
  " <body>\n"
  "  <h1>Bad Request</h1>\n"
  "  <p>This server did not understand your request.</p>\n"
  " </body>\n"
  "</html>\n";

/* HTTP response, header, and body template indicating that the
   requested document was not found.  */
static char* not_found_response = 
  "HTTP/1.1 404 Not Found\n"
  "Content-type: text/html\n"
  "\n"
  "<html>\n"
  " <body>\n"
  "  <h1>Not Found</h1>\n"
  "  <p>The requested URL %s was not found on this server.</p>\n"
  " </body>\n"
  "</html>\n";

/* HTTP response, header, and body template indicating that the
   method was not understood.  */
static char* bad_method_response = 
  "HTTP/1.1 405 Method Not Implemented\n"
  "Content-type: text/html\n"
  "\n"
  "<html>\n"
  " <body>\n"
  "  <h1>Method Not Implemented</h1>\n"
  "  <p>The method %s is not implemented by this server.</p>\n"
  " </body>\n"
  "</html>\n";

int parse_args(int argc, char * argv[], struct config * cfg);
int parse_head(char * , struct request * );
void respond(int status);
void terminate(int signum);

//Keep state information
struct set st;
struct addrinfo * addr_list;
struct request * req;

long long connections = 0;
int connfd;

int main(int argc, char * argv[])
{
    req = malloc(sizeof(request));

    char buf[BUF_MAX];
    
    // handles return values from our functions for error checking
    int ret;
    struct config cfg;

    // parse args into config structure
    ret = parse_args(argc, argv, &cfg);
    if(ret!=0)
        return 1;

    struct addrinfo * addr;

    //create tcp addrinfo for port and check for success
    ret = create_tcp_addrinfo(&addr_list, NULL, cfg.port);

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
        
        int len = read(connfd, buf, sizeof(buf));
        if( len <= 0)
            continue;
	
        //printf("%s", buf);
        ret = parse_head(buf, req);
        
        printf("Method:%s\n", req->method);
        printf("Path:%s\n", req->path);
        printf("Protocol:%s\n", req->protocol);
        printf("Host:%s\n", req->host);
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

	char * b = " ";
    //char * s = ":";
	//char * r = "\r\n";
	//const char n[3] = "\n";

	char * token;
	
	/* get the first token */
	token = strtok(msg,b);
	
	req->method = token;
   
	if(strcmp(req->method, "GET") != 0){
        respond(405);
	}
	 
	token = strtok(NULL,b);
	req->path=token;
	
	token = strtok(NULL,"\r\n");
	req->protocol=token;
	
	/* walk through other tokens */
	while(token)
	{
		token = strtok(NULL, ":");
        if(token == NULL)
        {
            break;
        }
		if(strcmp(token, "\nHost") == 0)
        {
			req->host = strtok(NULL,"\r\n");
            break;
        }
	}
	
	if(req->host == NULL){
        respond(400);
		return 1;
	}
    respond(200);
	return 0;
}

void respond(int status)
{
    int len;
    char response[BUF_MAX];
    //TODO write the response headers
    switch(status)
    {
        case 200:
            len = write(connfd, ok_response, strlen(ok_response));
            close(connfd);
            break;
        case 400:
            len = write(connfd, bad_request_response, strlen(bad_request_response)); 
            close(connfd);
            break;
        case 404:
            snprintf(response, sizeof(response), not_found_response, req->path);
            len = write(connfd, response, strlen(response));
            close(connfd);
            break;
        case 405:
            snprintf(response, sizeof(response), bad_method_response, req->method);
            len = write(connfd, response, strlen(response));
            close(connfd);
            break;
    }
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
