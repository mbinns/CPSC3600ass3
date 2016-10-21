#include "common.h"

struct config
{
    const char * port;
	const char * message;
	const char * servAdd;
	int view;

};

int parse_args(int argc, char * argv[], struct config * cfg);
void terminate(int signum);

//Keep state information
struct addrinfo * addr_list;

long long connections = 0;

int main(int argc, char * argv[])
{


    //char buf[BUF_MAX];
    
    int ret;

    struct config cfg;

    // parse args into config structure
    ret = parse_args(argc, argv, &cfg);
    if(ret!=0)
        return 1;

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
    while ((c = getopt(argc, argv, "p:s:va:")) != -1)
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
    printf("Place Holer");

    freeaddrinfo(addr_list);
    exit(0);
}
