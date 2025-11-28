#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
void erreur(char* string, int exit_status);
int main(int argc, char* argv[])

{
int sockfd, error, i, nbOct;
//struct sockaddr_in serverAddr, clientAddr;
struct addrinfo hints, *res, *lecture;
void *addr;
char buffer[50], port[10]="5000";
if(argc<2)
{
fprintf(stderr,"Erreur: l'adresse/nom du destinataire est manquant.\n");
fprintf(stderr,"Usage: %s dest\n",argv[0]);
exit(1);
}
sleep(3);
memset(&hints,0,sizeof(hints));
hints.ai_family = AF_INET;
hints.ai_socktype = SOCK_STREAM;
if((error=getaddrinfo(argv[1],port,&hints,&res))!=0) gai_strerror(error);
if((sockfd=socket(res->ai_family, res->ai_socktype, res->ai_protocol))<0) erreur("Erreur socket():",2);
printf("Client: connexion sur %s sur le port %s\n",argv[1],port);
if(connect(sockfd, res->ai_addr, res->ai_addrlen)==0)
{
for(i=0;i<100;i++)
{
if(i%2) strcpy(buffer,"toto");
else strcpy(buffer,"titi");
if( (nbOct=send(sockfd,buffer,strlen(buffer),0)) < 0 ) erreur("Error: send failed",2);
else{
printf("Le client a envoyer: %s\n",buffer);
sleep(3);
}
}
} else {
erreur("Error: connect failed",2);
}
return(0);
}
void erreur(char* string, int exit_status)
{
perror(string);
exit(exit_status);
}