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
int sockfd;
void erreur(char* string, int exit_status);
void zombie(int signal);
void controleC(int signal);
int manageConnection(int confd);
int main()
{
int confd, pid, size, error, tr=1;
struct sockaddr_in serverAddr, clientAddr;
struct addrinfo hints, *res, *lecture;
void *addr;
char clientAddrASCII[50];
if(signal(SIGCHLD,zombie)<0) erreur("Erreur: signal() SIGCHLD",4);
if(signal(SIGINT,controleC)<0) erreur("Erreur: signal() SIGCHLD",4);
memset(&hints,0,sizeof(hints));
hints.ai_family = AF_INET;
hints.ai_socktype = SOCK_STREAM;
hints.ai_flags = AI_PASSIVE;
if((error=getaddrinfo(NULL,"5000",&hints,&res)) !=0 ) gai_strerror(error);
if((sockfd=socket(res->ai_family, res->ai_socktype, res->ai_protocol))<0) erreur("Erreur socket():",2);
if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&tr,sizeof(int)) == -1) erreur("Erreur setsockopt() SO_REUSEADDR",2);
if(bind(sockfd, (struct sockaddr *) res->ai_addr, res->ai_addrlen)<0) erreur("Erreur bind()",2);
if(listen(sockfd, SOMAXCONN)<0) erreur("Erreur listen()",2);
size=sizeof(clientAddr);

while((confd=accept(sockfd, (struct sockaddr *) &clientAddr, (socklen_t *) &size))>0)
{
pid=fork();
switch(pid)
{
case 0: fprintf(stderr,"Nouvelle connexion addr:%s port client:%d port local:6000\n",inet_ntop(AF_INET,&clientAddr.sin_addr,clientAddrASCII,sizeof(clientAddrASCII)),ntohs(clientAddr.sin_port));
manageConnection(confd);
exit(0);
break;
case -1: erreur("Erreur: fork()",4);
break;
default: close(confd);
break;
}
}
erreur("Erreur accept()",2);
return(0);
}
void erreur(char* string, int exit_status)
{
perror(string);
exit(exit_status);
}
void zombie(int signal)
{
int pid;
while((pid=waitpid(-1,NULL,WNOHANG))>0)
{
//fprintf(stderr,"DEBUG: terminaison fils de pid %d\n",pid);
}
}
void controleC(int signal)
{
printf("Fin du serveur\n");
close(sockfd);
exit(0);
}
int manageConnection(int confd)
{
int retourRead;
char bufferRecv[500];
while( (retourRead=read(confd,bufferRecv,sizeof(bufferRecv)))>0)
{
bufferRecv[retourRead]='\0';
printf("The serveur has received: %s\n",bufferRecv);

}
close(confd);
}