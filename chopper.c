#include "common.h"
#include <curl/curl.h>

struct config
{
    const char * port;
	const char * message;
	const char * servAdd;
    const char * host;
    char http_stat[BUF_MAX];
	int view;
    int add;

}config;

struct MemoryStruct {
  char *memory;
  size_t size;
}MemoryStruct;

int parse_args(int argc, char * argv[], struct config * cfg);
void terminate(int signum);
long int send_get_request();
void parse_status(int);
// elapsed time
double elapsed;

// Keep state info for following vars
// counter for message attempts
int attempt = 0;

//Keep state information
struct addrinfo * addr_list;
struct config cfg;

long long connections = 0;
CURL * curl; 
char server_buf[BUF_MAX];

static char* get_add = "GET /add?data=%s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n";
static char* get_view = "GET /view? HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n";
static char* http_status = "%s %d";

int main(int argc, char * argv[])
{
    int ret;

    // parse args into config structure
    ret = parse_args(argc, argv, &cfg);
    curl = curl_easy_init();

    if(ret!=0)
        return 1;
    //Handles the termination signal
    signal(SIGINT, &terminate);

    //Starts the message timer
    double timer = get_wall_time();

    long int status_code = send_get_request();
    parse_status(status_code);
    attempt++;

    //Calculates the difference in time from send to recive
    timer = get_wall_time() - timer;

    elapsed += timer;

    // frees the address list
    terminate(0);
    return 0;
}

void parse_status(int status)
{
    char stat[BUF_MAX];
    switch(status)
        {
            case 200:
                snprintf(stat, sizeof(stat), http_status,"HTTP OK",200);
                break;
            case 400:
                snprintf(stat, sizeof(stat), http_status,"HTTP Bad Request",400);
                break;
            case 404:
                snprintf(stat, sizeof(stat), http_status,"HTTP Not Found",404);
                break;
            case 405:
                snprintf(stat, sizeof(stat), http_status,"HTTP Method Not Implemented",405);
                break;
        }
    strcpy(cfg.http_stat,stat);
}
size_t get_data(void *ptr, size_t size, size_t nmemb, void *stream){
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)stream;
 
  mem->memory = realloc(mem->memory, mem->size + realsize + 1);
  if(mem->memory == NULL) {
    /* out of memory! */ 
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }
 
  memcpy(&(mem->memory[mem->size]), ptr, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
    
  return realsize;
}

long int send_get_request()
{
    char URI[BUF_MAX];
    snprintf(URI, sizeof(URI),"%s:%s",cfg.servAdd,cfg.port);

    char REQUEST[BUF_MAX];
    if(cfg.view)
    {
        snprintf(REQUEST, sizeof(REQUEST),get_view,cfg.message,cfg.host);
    }else if(cfg.add)
    {
        snprintf(REQUEST, sizeof(REQUEST),get_add,cfg.message,cfg.host);
    }

    long http_code = 0;
    //To hold memory
    struct MemoryStruct chunk;
    
    //Will be grown by realloc
    chunk.memory = malloc(1);
    //No data
    chunk.size = 0;

    if(curl) {
      //Specify URL
      curl_easy_setopt(curl, CURLOPT_URL, URI);
     
      /* use a GET to fetch this */
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, get_data);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
      curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, REQUEST);
     
      /* Perform the request */
      curl_easy_perform(curl);
      curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
    }
    strcpy(server_buf,chunk.memory);
    return http_code;
}

int parse_args(int argc, char * argv[], struct config * cfg)
{
    int usage = 0;
	int pFlag,sFlag,hflag;

    // show usage on zero arguments
    if (argc == 1 || strcmp(argv[1], "-") == 0)
        usage = 1;

    // loop through all arguments with getopt and set variables as needed
    int c;
    while ((c = getopt(argc, argv, "p:s:va:h:")) != -1)
    {
        switch (c)
        {
            case 'p':
                cfg->port = optarg;
				pFlag=1;
                break;

			case 'a':
                cfg->message = optarg;
                cfg->add = 1;
                break;

			case 's':
                cfg->servAdd = optarg;
				sFlag=1;
                break;

			case 'v':
                cfg->view = 1;
                break;

            case 'h':
                cfg->host = optarg;
                hflag=1;
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
        fprintf(stderr, "Usage: %s -p <port> -s <server> [-a <message>] [-v (http view call)]\n", argv[0]);
        return 1;
    }
	if (!pFlag&&!sFlag)
	{
		fprintf(stderr, "Missing required port and server flags -p -s\n");
		return 1;
	}
	if (!pFlag)
	{
		fprintf(stderr, "Missing required port flag -p\n");
		return 1;
	}
	if (!sFlag)
	{
		fprintf(stderr, "Missing required server flag -s\n");
		return 1;
	}
	if (!hflag)
	{
		fprintf(stderr, "Missing required host_header flag -h\n");
		return 1;
	}

    if(cfg->view)
    {
        cfg->add = 1;
    }

    return 0;
}

void terminate(int signum)
{
    printf("%d\t%.6lf\t%s\t%s\t\n%s\n", attempt, elapsed,cfg.http_stat, cfg.message, server_buf);

    freeaddrinfo(addr_list);
    curl_easy_cleanup(curl);

    exit(0);
}
