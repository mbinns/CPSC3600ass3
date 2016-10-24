#include "common.h"
#include <curl/curl.h>

struct config
{
    const char * port;
	const char * message;
	const char * servAdd;
    const char * host;
	int view;

}config;

int parse_args(int argc, char * argv[], struct config * cfg);
void terminate(int signum);
long int send_get_request();

// elapsed time
double elapsed;

// Keep state info for following vars
// counter for message attempts
int attempt = 0;

// allocate buffer like a string
char buf[BUF_MAX];

//Keep state information
struct addrinfo * addr_list;
struct config cfg;

long long connections = 0;

int main(int argc, char * argv[])
{
    //char buf[BUF_MAX];
    
    int ret;

    // parse args into config structure
    ret = parse_args(argc, argv, &cfg);
    if(ret!=0)
        return 1;
    //Handles the termination signal
    signal(SIGINT, &terminate);

/*
    //Starts the message timer
    double timer = get_wall_time();

    // allocate buffer like a string
    buf[len] = '\0';

    // receive the return message from the server and check for error
    //ret = read(sockfd, buf, len);

    //Calculates the difference in time from send to recive
    timer = get_wall_time() - timer;

    if(ret > 0)
    {
        attempt++;
    }

    elapsed += timer;
*/

    long int status_code = send_get_request();
    printf("Status code: %ld", status_code);

    // frees the address list
    terminate(0);
    return 0;
}

long int send_get_request()
{
    char URI[BUF_MAX];
    long http_code = 0;
    snprintf(URI, sizeof(URI),"%s:%s",cfg.servAdd,cfg.port);

    CURL * curl = curl_easy_init();
    if(curl) {
      curl_easy_setopt(curl, CURLOPT_URL, URI);
     
      /* use a GET to fetch this */
      curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET /iasd HTTP/1.1\r\nHost: www.example.com\r\nConnection: close\r\n\r\n");
     
      /* Perform the request */
      curl_easy_perform(curl);
      curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);

      curl_easy_cleanup(curl);
    }
    return http_code;
}

int parse_args(int argc, char * argv[], struct config * cfg)
{
    int usage = 0;
	int pFlag,sFlag;

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

    return 0;
}

void terminate(int signum)
{
    //printf("%s",cfg.message);

    freeaddrinfo(addr_list);
    exit(0);
}
