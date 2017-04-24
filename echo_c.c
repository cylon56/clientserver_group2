//Edward Shih
//04.23.2017
//CS3376.501 Project
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char* e) {
 perror(e);
}

int main(int argc, char *argv[])
{

bool PickOne = true;


    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *serverY;

    int sock;
   unsigned int length;
   struct sockaddr_in server, from;
   struct hostent *hp;

   char buffer[256];

    if (argc < 4) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
         printf("Not enough args\n");
       exit(0);
    }

if( atoi(argv[3]) == 1)
{
PickOne = false;
  printf("Should be a Data Gram \n");
}



  //===================================================================
  if(PickOne)
  {
   portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    serverY = gethostbyname(argv[1]);

    if (serverY == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)serverY->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         serverY->h_length);
    serv_addr.sin_port = htons(portno);



    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");
  }
  //===================================================================
   else
   {
      sock= socket(AF_INET, SOCK_DGRAM, 0);
   if (sock < 0) error("socket");

   server.sin_family = AF_INET;
   hp = gethostbyname(argv[1]);
   if (hp==0) error("Unknown host");

   bcopy((char *)hp->h_addr,
        (char *)&server.sin_addr,
         hp->h_length);
   server.sin_port = htons(atoi(argv[2]));
   length=sizeof(struct sockaddr_in);
   }
     //===================================================================

    printf("Please enter the message: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);

    if(PickOne)
    {
     n = write(sockfd,buffer,strlen(buffer));
  }
   else
   {
       n=sendto(sock,buffer,
            strlen(buffer),0,(const struct sockaddr *)&server,length);
   }



    if (n < 0)
         error("ERROR writing to socket");
    bzero(buffer,256);

    if(PickOne)
    {
    n = read(sockfd,buffer,255);
    }
    else
    {
       n = recvfrom(sock,buffer,256,0,(struct sockaddr *)&from, &length);
   }

    //



    if (n < 0)
         error("ERROR reading from socket");
    printf("%s\n",buffer);


    if(PickOne)
    {
     close(sockfd);
    }
    else
    {

       write(1,"Got an ack: ",12);
   write(1,buffer,n);
      close(sock);
    }

     //
    return 0;
}
