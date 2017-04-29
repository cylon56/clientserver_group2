// 	file: echo_s.c
// 	author: Nicholas Strauss
// 	date: 04/20/2017
// 	purpose: Group Project Second Deliverable
//	details: 	This code has been taken (as instructed by the professor) from 
//				http://www.linuxhowtos.org/C_C++/socket.htm and
// 				has been modified by the author, Nicholas Strauss.
//Part 2 completed
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>

#include "echo_s.h"

using namespace std;

int port_global = 9999;	

void error(const char *msg) { // function to print any errors
    perror(msg);
    exit(1);
}


//Made by Mark Sowders, a lot of it is gotten from https://en.wikipedia.org/wiki/C_date_and_time_functions with modifications to fit our needs
char* addDate(char* InString)
{

    time_t current_time;
    char* c_time_string;

    /* Obtain current time. */
    current_time = time(NULL);
	
    if (current_time == ((time_t)-1))
    {
        (void) fprintf(stderr, "Failure to obtain the current time.\n");
    }
    /* Convert to local time format. */
    c_time_string = ctime(&current_time);
    if (c_time_string == NULL)
    {	
        (void) fprintf(stderr, "Failure to convert the current time.\n");
    }
//Amends the date/time to the end of the given string        
strcat(InString,c_time_string);
    return InString;
}
//Deliverable 3- user2 edit: added int  logPort to method call
void toLogServer (char* string, char* ipaddress, int logPort) { // creates a client that takes the buffer from the server, and sends it to log server
	int sock, n;
	unsigned int length;
	struct sockaddr_in server;
	struct hostent *hp;
	char* buffer;
  

  printf("ToLogServer was called \n ");


	sock= socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) error("socket");


	if(strcmp(ipaddress, "127.0.0.1") != 0)
	{
	//	printf("%s\n", "triggered");
		server.sin_addr.s_addr = inet_addr(ipaddress);
		server.sin_family = AF_INET;
		hp = gethostbyname(ipaddress); // This could have a problem, it assumes you are testing on one machine. If this causes an error, try cs1.utdallas.edu instead
		if (hp==0) error("Unknown host");
	}
	else {
		server.sin_addr.s_addr = inet_addr("127.0.0.1");
		server.sin_family = AF_INET;
		hp = gethostbyname("127.0.0.1"); // This could have a problem, it assumes you are testing on one machine. If this causes an error, try cs1.utdallas.edu instead
		if (hp==0) error("Unknown host");
	}

	if(logPort <= 0){//Third Deliverable pt2 defaults to port no 9999
		logPort = 9999;
	}

	bcopy((char *)hp->h_addr, (char *)&server.sin_addr, hp->h_length);
	server.sin_port = htons(logPort);
	

	length=sizeof(struct sockaddr_in);
   	

	buffer = addDate(string);
   	
	n = sendto(sock,buffer,strlen(buffer),0,(const struct sockaddr *)&server,length);
	
	if (n < 0) error("Sendto");
	close(sock);
}

void dostuff (int sock, char* ipaddress, int logPort) { //Method that is provided from http://www.linuxhowtos.org/C_C++/socket.htm and is used as instructed by the professor
	int theNumber, pid;
	char buffer[256];
	char* tempBuf;
      
	bzero(buffer,256);
	theNumber = read(sock,buffer,255);
	if (theNumber < 0) error("ERROR reading from socket");
	pid = fork();
	if (pid < 0)
		error("child process");
	if (pid == 0) {
		toLogServer(buffer, ipaddress, logPort);
		exit(0);
	}
	else {
		tempBuf = strtok(buffer, "\"");
		printf("Here is the message: %s\n",tempBuf);
		theNumber = write(sock,"I got your message",18);
		if (theNumber < 0) error("ERROR writing to socket");
	}
}


void SignalHandler (int Signal) {

	printf("This is before the log server call \n");
	char end[20] = "echo_s is stopping";
	toLogServer(end, "127.0.0.1", port_global); 
	printf("echo_s is stopping \n");
	exit(0);
}


int main(int argc, char *argv[]) {

signal(SIGINT, SignalHandler);

	// Initiate starting variables
    int sockT, sockU, portno, logPort, pidTCP, pidUDP, pidFirstSplit, pidSecondSplit, pidThirdSplit;
	int sockTCP[argc-1];
	int sockUDP[argc-1];
	
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int n = argc-1;
	char* ipaddress = "127.0.0.1"; //set for 127.0.0.1 in case no ip address is passed, changed from 0 in part 5

	// Calls if there is no argument for the port
	// changed from argc > 4 -> argc > 6 during Third deliverable pt 2
	// removed argc > 6 in part 5
    if (argc < 2) {
        error("ERROR, no port passed as an argument\n");
        exit(1);
    }

	int count = 0;
	logPort = 0;

	//Using socket( to establish the server via a port number, using a for loop for the number of port numbers
	for (int i = 1; i < argc; i++) 
	{
		
		//Third Deliverable Improvement by Michael Lewellen, bug fixed by Nicholas Strauss
		if(strcmp(argv[i], "-logip") == 0)
		{
			ipaddress = argv[i+1];
			i = i + 1;
		}
		//Third Deliverable user2 task- Edward Shih
		else if (strcmp(argv[i], "-logport") == 0){
			logPort = atoi(argv[i+1]);
			port_global = logPort;
			i = i + 1;
		
		}
		else {
		sockT = socket(AF_INET, SOCK_STREAM, 0);
		sockU = socket(AF_INET, SOCK_DGRAM, 0);
		
		// Lets the user know if there was an error opening the socket
		if (sockT < 0) 
			error("ERROR opening the socket TCP");
		if (sockU < 0) 
			error("ERROR opening the socket UDP");
		
		sockTCP[count] = sockT;
		sockUDP[count] = sockU;
		
		//remove '[' and ']' from every argument, due to requested format.
		char * tempString = argv[i];
		if(tempString[0] == '[')
			tempString++;
		if(tempString[strlen(tempString)-1] == ']')
			tempString[strlen(tempString)-1] = 0;
		argv[i] = tempString;
		
		//Server settings, removing certain characters if needed
		bzero((char *) &serv_addr, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		portno = atoi(argv[i]);
		serv_addr.sin_addr.s_addr = INADDR_ANY;
		serv_addr.sin_port = htons(portno);
    
		//Binding the socket
		if (bind(sockTCP[i-1], (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
			error("ERROR on binding");
		if (bind(sockUDP[i-1], (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
			error("ERROR on binding");
		
		count = count + 1;

		}
		}
	//Handling zombie processes
	if (signal(SIGCHLD, SIG_IGN) == SIG_ERR) {
		error("ERROR with zombie process handler");
	}
	/*  TODO: Edit the line numbers of this comment to be accurate to the following code
	
		Lines 103 through 267 are the lines for taking up to 3 ports in one server.
		It works by spliting the code into three processes, which the latter processes only occur if there is a port to use it with.
	*/	
	pidFirstSplit = fork(); //start first split, between one port and two ports
	if (pidFirstSplit < 0)
		error("ERROR on creating the fork");
	if (pidFirstSplit == 0) { //create a UDP and TCP server
		
		int pid = fork();
		if (pid < 0)
			error("ERROR on creating the fork");
		if (pid == 0) { //UDP
			int num;
			socklen_t fromlen;
			//struct sockaddr_in server; may not be used
			struct sockaddr_in from;
			char bufUDP[1400];
			char* tempBuf;
			fromlen = sizeof(struct sockaddr_in);
			while (1) { // Receive and send 
				num = recvfrom(sockUDP[0],bufUDP,1400,0,(struct sockaddr *)&from,&fromlen);
				if (num < 0) 
					error("recvieve from");
				write(1,"Received a datagram: ",21);
				pidUDP = fork();//send data to the log server, also send data to the client
				if (pidUDP < 0)
					error("ERROR on creating the fork");
				if (pidUDP == 0) {
					close(sockUDP[0]);
					toLogServer(bufUDP, ipaddress, logPort);
					exit(0);
				}
				else {
					tempBuf = strtok(bufUDP, "\"");
					write(1,tempBuf,strlen(tempBuf));
					num = sendto(sockUDP[0],"Got your message\n",17,0,(struct sockaddr *)&from,fromlen);
					if (num  < 0) 
						error("sendto");
				}
			} //End of the loop
		}
		else { //TCP
			
			int newsockTCP;
			
			//Listening for the socket
			listen(sockTCP[0],5);
			clilen = sizeof(cli_addr);
		
			//Continuous server. Calls dostuff() for the accept and recvieve steps
			while (1) 
			{
				newsockTCP = accept(sockTCP[0], (struct sockaddr *) &cli_addr, &clilen);
				if (newsockTCP < 0) 
					error("ERROR on accepting the socket");
				pidTCP = fork();
				if (pidTCP < 0)
					error("ERROR on creating the fork");
				if (pidTCP == 0) {
					close(sockTCP[0]);
					dostuff(newsockTCP, ipaddress, logPort);
					exit(0);
				}
				else close(newsockTCP);
			} //End of the loop
		} //End of TCP
	} //End of first port
	else { //Split between the second and third process
		if(argc > 2 && strcmp(ipaddress, "0") == 0) { //only go on if two ports are used
			pidSecondSplit = fork(); //start second split, between two ports and three ports
			if (pidSecondSplit < 0)
				error("ERROR on creating the fork");
			if (pidSecondSplit == 0) { //create a UDP and TCP server
		
				int pid = fork();
				if (pid < 0)
					error("ERROR on creating the fork");
				if (pid == 0) { //UDP
					int num;
					socklen_t fromlen;
					//struct sockaddr_in server; may not be used.
					struct sockaddr_in from;
					char bufUDP[1400];
					char* tempBuf;
					fromlen = sizeof(struct sockaddr_in);
					while (1) { // Receive and send 
						num = recvfrom(sockUDP[1],bufUDP,1400,0,(struct sockaddr *)&from,&fromlen);
						if (num < 0) 
							error("recvieve from");
						write(1,"Received a datagram: ",21);
						pidUDP = fork(); //send data to the log server, also send data to the client
						if (pidUDP < 0)
							error("ERROR on creating the fork");
						if (pidUDP == 0) {
							close(sockUDP[1]);
							toLogServer(bufUDP, ipaddress, logPort);
							exit(0);
						}
						else {
							tempBuf = strtok(bufUDP, "\"");
							write(1,tempBuf,strlen(tempBuf));
							num = sendto(sockUDP[1],"Got your message\n",17,0,(struct sockaddr *)&from,fromlen);
							if (num  < 0) 
								error("sendto");
						}
					} //End of the loop
				}
				else { //TCP
			
					int newsockTCP;
			
					//Listening for the socket
					listen(sockTCP[1],5);
					clilen = sizeof(cli_addr);
			
					//Continuous server. Calls dostuff() for the accept and recvieve steps
					while (1) {
						newsockTCP = accept(sockTCP[1], (struct sockaddr *) &cli_addr, &clilen);
						
						if (newsockTCP < 0) 
							error("ERROR on accepting the socket");
						pidTCP = fork();
						if (pidTCP < 0)
							error("ERROR on creating the fork");
						if (pidTCP == 0) {
							close(sockTCP[1]);
							dostuff(newsockTCP, ipaddress, logPort); 
							exit(0);
						}
						else close(newsockTCP);
					} //End of the loop
				}
			}
			else {
				if(argc > 3 && strcmp(ipaddress, "0") == 0) { //Only go on if three ports are used
					pidThirdSplit = fork(); //start second split, between two ports and three ports
					if (pidThirdSplit < 0)
						error("ERROR on creating the fork");
					if (pidThirdSplit == 0) { //create a UDP and TCP server
					int pid = fork();
					if (pid < 0)
						error("ERROR on creating the fork");
					if (pid == 0) { //UDP
						int num;
						socklen_t fromlen;
						//struct sockaddr_in server; may not be used.
						struct sockaddr_in from;
						char bufUDP[1400];
						char* tempBuf;
						fromlen = sizeof(struct sockaddr_in);
						while (1) { // Receive and send 
							num = recvfrom(sockUDP[2],bufUDP,1400,0,(struct sockaddr *)&from,&fromlen);
							
							if (num < 0) 
								error("recvieve from");
							write(1,"Received a datagram: ",21);
							pidUDP = fork();//send data to the log server, also send data to the client
							if (pidUDP < 0)
								error("ERROR on creating the fork");
							if (pidUDP == 0) {
								close(sockUDP[2]);
								toLogServer(bufUDP, ipaddress, logPort);
								exit(0);
							}
							else {
								tempBuf = strtok(bufUDP, "\"");
								write(1,tempBuf,strlen(tempBuf));
								num = sendto(sockUDP[2],"Got your message\n",17,0,(struct sockaddr *)&from,fromlen);
								if (num  < 0) 
									error("sendto");
							}
							tempBuf = strtok(bufUDP, "\"");
							write(1,tempBuf,strlen(tempBuf));
							num = sendto(sockUDP[2],"Got your message\n",17,0,(struct sockaddr *)&from,fromlen);
							if (num  < 0) 
								error("sendto");
						} //End of the loop
					}
					else { //TCP
				
						int newsockTCP;
				
						//Listening for the socket
						listen(sockTCP[2],5);
						clilen = sizeof(cli_addr);
			
						//Continuous server. Calls dostuff() for the accept and recvieve steps
						while (1) {
							newsockTCP = accept(sockTCP[2], (struct sockaddr *) &cli_addr, &clilen);
							if (newsockTCP < 0) 
								error("ERROR on accepting the socket");
								pidTCP = fork();
							if (pidTCP < 0)
								error("ERROR on creating the fork");
							if (pidTCP == 0) {
								close(sockTCP[2]);
								dostuff(newsockTCP, ipaddress, logPort); 
								exit(0);
							}
							else close(newsockTCP);
						} //End of the loop
					} //End of TCP
					} //End of third split
					else {} //parent goes to close sockets
				} //End of if(argc > 3)
			} //End of third port
		} //End of if(argc > 2)
	} //End of second port AND the code

	//Close and exit, which will not be reached.
	for(int i = 0; i < n; i++) {
		close(sockTCP[i]);
		close(sockUDP[i]);
	}
	return 0; 
}
