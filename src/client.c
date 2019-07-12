#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "lib.h"

#define BUFSIZE 4096 // max number of bytes we can get at once

/**
 * Struct to hold all three pieces of a URL
 */
typedef struct urlinfo_t {
  char *hostname;
  char *port;
  char *path;
} urlinfo_t;

/**
 * Tokenize the given URL into hostname, path, and port.
 *
 * url: The input URL to parse.
 *
 * Store hostname, path, and port in a urlinfo_t struct and return the struct.
*/
urlinfo_t *parse_url(char *url)
{
  // copy the input URL so as not to mutate the original
  char *hostname = strdup(url);
  char *port;
  char *path;

  urlinfo_t *urlinfo = malloc(sizeof(urlinfo_t));

  /*
    We can parse the input URL by doing the following:

    1. Use strchr to find the first slash in the URL (this is assuming there is no http:// or https:// in the URL).
    2. Set the path pointer to 1 character after the spot returned by strchr.
    3. Overwrite the slash with a '\0' so that we are no longer considering anything after the slash.
    4. Use strchr to find the first colon in the URL.
    5. Set the port pointer to 1 character after the spot returned by strchr.
    6. Overwrite the colon with a '\0' so that we are just left with the hostname.
  */
//----Sprint:
 //1.Use strchr to find the first slash in the URL:
  char *first_slash = strchr(hostname, '/');
 //2.Set the path pointer to 1 character after the spot returned by strchr:
  path = first_slash + 1;
 //3.Overwrite the slash with a '\0' so that we are no longer considering anything after the slash:
 //set first_slash = \0
  *first_slash = '\0';
 //4.Use strchr to find the first colon in the URL:
  char *first_colon = strchr(hostname, ':');
 //5.Set the port pointer to 1 character after the spot returned by strchr:
  port = first_colon + 1;
//6.Overwrite the colon with a '\0' so that we are just left with the hostname.
  *first_colon = '\0';

//implement: assign to appropriate field struct

  urlinfo->hostname = hostname;
  urlinfo->port = port;
  urlinfo->path = path;

  return urlinfo;
}

/**
 * Constructs and sends an HTTP request
 *
 * fd:       The file descriptor of the connection.
 * hostname: The hostname string.
 * port:     The port string.
 * path:     The path string.
 *
 * Return the value from the send() function.
*/
int send_request(int fd, char *hostname, char *port, char *path)
{
  const int max_request_size = 16384;
  char request[max_request_size];

//Implemtent: ---- Sprint:

//2. construct HTTP Req:
//ex: GET /path HTTP/1.1
//    Host: hostname:port
//    Connection: close

  int request_length = sprintf(request,
  "GET /%s HTTP/1.1.\n"
  "HOST: %s:%s\n"
  "CONNECTION: close\n"
  "\n",
  path, hostname, port);

  //send rqst:
  int rv = send(fd, request, request_length, 0);

  if (rv < 0) {
    perror("Send");
  }

  return rv;

}

  


int main(int argc, char *argv[])
{  
  int sockfd, numbytes;  
  char buf[BUFSIZE];

  if (argc != 2) {
    fprintf(stderr,"usage: client HOSTNAME:PORT/PATH\n");
    exit(1);
  }

  /*
    1. Parse the input URL
    2. Initialize a socket by calling the `get_socket` function from lib.c
    3. Call `send_request` to construct the request and send it
    4. Call `recv` in a loop until there is no more data to receive from the server. Print the received response to stdout.
    5. Clean up any allocated memory and open file descriptors.
  */
  
  //Implemtent: ---- Sprint:
  //3.Connect to the server
  //4.Send the request string down the socket
  //5.Receive the response from the server and print it to stdout

  //1. Parse the input URL:
  urlinfo_t *urlinfo = parse_url(argv[1]);
  //2. Initialize a socket by calling the `get_socket` function from lib.c:
  sockfd = get_socket(urlinfo->hostname, urlinfo->port);
  //3. Call `send_request` to construct the request and send it:
  send_request(sockfd, urlinfo->hostname, urlinfo->port, urlinfo->path);
  //4a. Call `recv` in a loop until there is no more data to receive from the server. 
  while ((numbytes = recv(sockfd, buf, BUFSIZE - 1, 0)) > 0) {
  //4b. Print the rcv'd response to stdout:
    printf("%s\n", buf);
  }
  //5. Clean up any allocated memory and open file descriptors:
  free(urlinfo);
  close(sockfd);
  

  return 0;
}
