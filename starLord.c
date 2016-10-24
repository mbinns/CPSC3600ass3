#include "set.h"
#include "common.h"

struct config
{
    const char * port;
}config;

struct request
{
	char * method;
	char * path;
    char * data;
	char * protocol;
	char * host;
}request;

struct response
{
	//have to have below this
	char date[BUF_MAX];
	char modified[BUF_MAX];
	char content_len[BUF_MAX];
	char content_typ[BUF_MAX];
	char server_head[BUF_MAX];
}response;

/* HTTP response and header for a successful request.  */
static char* ok_response =
  "HTTP/1.1 200 OK\r\n"
  "Content-type: text/html\r\n"
  "Connection: close\r\n"
  "Date: %s\r\n"
  "Last-Modified: %s\r\n"
  //"Content-Length: %d\r\n\r\n"
  "\r\n"
  "%s";

/* HTTP response, header, and body indicating that the we didn't
   understand the request.  */
static char* bad_request_response = 
  "HTTP/1.1 400 Bad Request\r\n"
  "Content-type: text/html\r\n"
  "Connection: close\r\n"
  "Date: %s\r\n"
  "Last-Modified: %s\r\n"
  "Server: Group15/v1.0\r\n\r\n";

/* HTTP response, header, and body template indicating that the
   requested document was not found.  */
static char* not_found_response = 
  "HTTP/1.1 404 Not Found\r\n"
  "Content-type: text/html\r\n"
  "Connection: close\r\n"
  "Date: %s\r\n"
  "Last-Modified: %s\r\n"
  "Server: Group15/v1.0\r\n\r\n";

/* HTTP response, header, and body template indicating that the
   method was not understood.  */
static char* bad_method_response = 
  "HTTP/1.1 405 Method Not Implemented\r\n"
  "Content-type: text/html\r\n"
  "Connection: close\r\n"
  "Allow: GET\r\n"
  "Date: %s\r\n"
  "Last-Modified: %s\r\n"
  "Server: Group15/v1.0\r\n\r\n";

int parse_args(int argc, char * argv[], struct config * cfg);
int parse_head(char * , struct request * );
void respond(int status);
void terminate(int signum);
int buf_add(char *);
int buf_view(char *);

//Keep state information
struct set st;
struct addrinfo * addr_list;
struct request * req;
struct response * resp;

long long connections = 0;
int connfd;
char buffer[BUF_MAX];

int main(int argc, char * argv[])
{
    req = malloc(sizeof(request));
    req->method = NULL;
    req->path = NULL;
    req->data = NULL;
    req->protocol = NULL;
    req->host = NULL;

    resp = malloc(sizeof(response));

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
    
    // set some response info
    char date[BUF_MAX] = "Date: %a, %d %b %Y %H:%M:%S %Z";
    
    time_t sec = time(NULL);
    struct tm * tm = gmtime(&sec);

    strftime(resp->date, sizeof(resp->date),date,tm);
    // accept incoming connections
    while(1)
    {
        struct sockaddr remote;
        socklen_t rlen = sizeof(remote);
        connfd = tcp_accept(sockfd,&remote,&rlen);
        
        connections++;
        set_add(&st,get_addr(&remote));
        
        //TODO Remove print
        printf("Connection:%lld\n",connections);
        
        int len = read(connfd, buf, sizeof(buf));
        if( len <= 0)
            continue;
	
        //printf("%s", buf);
        ret = parse_head(buf, req);
       
        //TODO Remove print statements
        printf("Method:%s\n", req->method);
        printf("Path:%s\n", req->path);
        printf("Data:%s\n", req->data);
        printf("Protocol:%s\n", req->protocol);
        printf("Host:%s\n", req->host);
        req->method = NULL;
        req->path = NULL;
        req->data = NULL;
        req->protocol = NULL;
        req->host = NULL;
    }

    terminate(0);
    return 0;
}

int buf_add(char * msg)
{
    char mod[BUF_MAX] = "Date: %a, %d %b %Y %H:%M:%S %Z";
    
    time_t sec = time(NULL);
    struct tm * tm = gmtime(&sec);

    strftime(resp->modified, sizeof(resp->modified),mod,tm);
    
    if(msg != NULL)
    {
        strcat(buffer,req->host);
        strcat(buffer,"\n");
        strcat(buffer,msg);
        strcat(buffer,"\n");
    }
    return 1;
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

	char *token;
	/* get the first token */
	token = strtok(msg,b);
    if(token == NULL)
    {
        respond(400);
    }

	req->method = token;
   
	if(strcmp(req->method, "GET") != 0){
        respond(405);
	}
	 
	token = strtok(NULL,b);
	req->path=token;

	token = strtok(NULL,"\r\n");
	req->protocol=token;
    if(strcmp(req->protocol, "HTTP/1.1") != 0)
    {
        respond(400);
    }

	token=strstr(req->path,"add");
        if(token==NULL) {
	   token=strstr(req->path,"view");
	   if(token==NULL) {
	      respond(404);
	      return 1;
	   }
	}

	token=strstr(req->path,"view");
       if(token==NULL) {
	   token=strstr(req->path,"add");
	   if(token==NULL) {
	      respond(404);
	      return 1;
	   }
	}
	
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
			req->host = strtok(NULL,"\r\n")+1;
            break;
        }
	}

	if(req->host == NULL){
        respond(400);
		return 1;
	}
    
    token = strstr(req->path,"data=");
    if(token != NULL)
    {
        token = strtok(token, "&\r\n");
        req->data = token+5;
    }

    respond(200);
	return 0;
}

void respond(int status)
{
    int len;
    char response[BUF_MAX];
    switch(status)
    {
        case 200:
            if(strstr(req->path, "add"))
            {
                len = buf_add(req->data);
            }
            snprintf(response, sizeof(response), ok_response,resp->date,resp->modified, buffer);
            len = write(connfd, response, strlen(response));
            close(connfd);
            break;
        case 400:
            len = write(connfd, bad_request_response, strlen(bad_request_response)); 
            close(connfd);
            break;
        case 404:
            snprintf(response, sizeof(response), not_found_response,resp->date,resp->modified, req->path);
            len = write(connfd, response, strlen(response));
            close(connfd);
            break;
        case 405:
            snprintf(response, sizeof(response), bad_method_response,resp->date,resp->modified, req->method);
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
