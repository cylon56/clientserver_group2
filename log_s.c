// 	file: log_s.c
// 	author: Michael Lewellen
// 	date: 04/18/2017
// 	purpose: Group Project Second Deliverable
//	details: This code has been taken from server.c from the first deliverable that was created by Nick Strauss and modified to be used as a log sever by Michael Lewellen

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>

using namespace std;

void error(const char *msg) //function to print any errors
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) 
{
	
	// Initiate starting variables
    int sockUDP, portno, pidUDP, pid;
	int opt =1;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    //int n;
	
	// Calls if there is no argument for the port
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port passed as an argument\n");
        exit(1);
    }
	
	//Using socket() to establish the server
	sockUDP = socket(AF_INET, SOCK_DGRAM, 0);
	
    
	// Lets the user know if there was an error opening the socket
    if (sockUDP < 0) 
       error("ERROR opening the socket UDP");
	
	//Server settings
	bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
	
	//bind the socket to the address
	setsockopt(sockUDP,SOL_SOCKET,SO_REUSEADDR,(const char *)&opt,sizeof(int));
    
	//Binding the socket
    if (bind(sockUDP, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
		error("ERROR on binding");
	
	//Handling zombie processes
	if (signal(SIGCHLD, SIG_IGN) == SIG_ERR) {
		error("ERROR with zombie process handler");
	}
	
	
	int num;
	socklen_t fromlen;
	struct sockaddr_in server;
	struct sockaddr_in from;
	char bufUDP[1024];
	fromlen = sizeof(struct sockaddr_in);
	
	
	pidUDP = fork();
	if (pidUDP == 0) {
	while (1) 
	{ // Receive and write 

		num = recvfrom(sockUDP,bufUDP,1024,0,(struct sockaddr *)&from,&fromlen);
		if (num < 0)  error("recvieve from");
			
		pid = fork();
		if (pid < 0) error("ERROR on creating the fork");
			
		if(pid == 0)//use child to pass message to log file
		{
			FILE *f = freopen( "echo.log", "a", stdout );//send output to log file
			write(1,"Received a datagram: ",21);
			write(1,bufUDP,num);
			bufUDP[num] = '\0';
			printf("%s\n", bufUDP);
			fclose(f);
			close(sockUDP);
			exit(0);
			
		}
	// End of the loop
	}
	}
	
	//Close and exit
	close(sockUDP);
    return 0; 
}
