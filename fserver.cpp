/*
** Becky Solomon
** CS 372-400
** Fall 2015
** Program 2
** fserver.cpp 
**
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>


#define BACKLOG 10     // how many pending connections queue will hold

#define MAXCOMMANDSIZE 1000 // max number of bytes allowed in client's handle

#define MAXDATASIZE 6022386

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

//sets up connection to client for receiving data on port portno
int setup_control_connect(char* portno){
    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;
    int numbytes;
    const char* hostname = "localhost";

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, portno, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    if (p == NULL) {
            fprintf(stderr, "client: failed to find address\n");
            return 2;
        }
    
    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    printf("server: waiting for connections...\n");

    sin_size = sizeof their_addr;
    //printf("server: waiting for client\n");
    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        
    close(sockfd);

    if (new_fd == -1) {
        perror("accept");
        exit(1);
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure

    printf("return socket %d\n", new_fd);
	return new_fd;
}

//sets up connection to client for receiving commands on port portno
int setup_data_connect(char* portno) {

    printf("setting up data connection\n");
    int sockfd;
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN]; 
    const char* hostname = "localhost";
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(hostname, portno, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    printf("Got addrinfo\n");

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("Server data connection: error opening socket");
            continue;
        }
        printf("portno: %s, sockfd: %d\n", portno, sockfd);
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("Server data connection: error connecting");
            continue;
        }
        break;
    }


    if (p == NULL) {
        fprintf(stderr, "Server data connect: failed to find address\n");
        exit(1);
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    printf("Server control connection: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure
    
    return sockfd;
}

void get_data(int socket_fd, char* buf){

    int numbytes;

    memset(buf, 0, MAXDATASIZE);

    printf("socket_fd: %d\n", socket_fd);
    if ((numbytes = recv(socket_fd, buf, MAXDATASIZE-1, 0)) == -1) {
        perror("recv");
        exit(1);
    }
    printf("received %d bytes\n", numbytes);
    buf[numbytes] = 0;
    return;
}

void process_command(int socket_fd, char** commandp, char** data_portp, char** filenamep){

    printf("processing command...\n");
	
    static char buf[MAXDATASIZE];
    char* filename;
    char* data_port;
    char* command;
    char* temp;

    
	//receive
	get_data(socket_fd, buf);

    //parse command
    printf("parsing command...\n");
    command = buf;
    temp = strchr(buf, ' ');
    *temp = 0;

    data_port = temp+1;
    temp = strchr(data_port, ' ');
    *temp = 0;

    filename = temp+1;
    temp = strchr(filename, ' ');
    //*temp = 0;

    printf("command: %s, data_port: %s, filename: %s\n", command, data_port, filename);

    close(socket_fd);
	
    *commandp = command;
    *data_portp = data_port;
    *filenamep = filename;

    return;

}

void send_file(int sock, char* filename){
    char buf[1025];
    int numBytes, sentBytes, offset;
    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("Can't open file %s for reading\n", filename);
        return;
    }
    while (!feof(file)){

        numBytes = fread(buf, 1, sizeof(buf), file);
        printf("numbytes: %d\n", numBytes);
        if (numBytes < 1) {
            printf("Can't read from file %s\n", filename);
            fclose(file);
            exit(1);
        }
        offset = 0;
        do {
            sentBytes = send(sock, &buf[offset], numBytes - offset, 0);
            if (sentBytes < 1) {
                printf("Can't write to socket\n");
                fclose(file);
                exit(1);
            }

            offset += sentBytes;

        } while (offset < numBytes);
    }

    fclose(file);
    return;
}

int main(int argc, char *argv[])
{
    int data_fd, control_fd, numbytes; 
    char* command = NULL;
    char* data_port = NULL;
    char* filename = NULL;
    char buf[MAXDATASIZE];
    char* control_portno;
    

    //validate command line arguments    
    if (argc != 2) {
        fprintf(stderr,"usage: server <portnumber>\n");
        exit(1);
    }
    control_portno = argv[1];

    //connect to client for receiving commands
    control_fd = setup_control_connect(control_portno);

    //get command
    printf("Xserver main: control conection established\n");
    process_command(control_fd, &command, &data_port, &filename);

    //connect to client for sending data
    data_fd = setup_data_connect(data_port);
    

    //command was -l
    if (filename == '\0') {
        system("ls -l");
    }
    //command was -g
    else {
        //send file
        send_file(data_fd, filename);
    }


    

    return 0;
}

