#ifndef COMMON_H
#define COMMON_H
#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define BUF_MAX 1024

// The get_wall_time routine gets a representation of the current wall time in seconds.
double get_wall_time();
// The get_addr routine returns a string representation of the IP address of a sockaddr.
const char * get_addr(struct sockaddr * remote);
// The create_tcp_addrinfo routine creates a TCP addrinfo structure given a host and a port that can be used to open sockets and send messages.
int create_tcp_addrinfo(struct addrinfo ** addr_list, const char * host, const char * port);
// The create_socket routine creates a socket from a given addrinfo.
int create_socket(struct addrinfo * addr);
// The bind_addr routine binds a socket to a given addrinfo.
int bind_addr(int sockfd, struct addrinfo * addr);
// The listen_socket function listens to the socket
int listen_socket(int sockfd);
// The tcp_accept will accept a single connection on the socket
int tcp_accept(int sockfd, struct sockaddr * addr, socklen_t * rlen);
// The tcp_connect will preform the tcp 3-way handshake and create a connection between server and client
int tcp_connect(int sockfd, struct addrinfo * addr);
#endif
