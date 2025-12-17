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
    printf("Démarrage du client Kubernetes...\n");
    
    int sockfd, error, i, nbOct;
    struct addrinfo hints, *res;
    char buffer[50], port[10] = "6000";

    if (argc < 2)
    {
        fprintf(stderr, "Erreur: l'adresse/nom du destinataire est manquant.\n");
        fprintf(stderr, "Usage: %s dest\n", argv[0]);
        exit(1);
    }

    // --- RETRY LOOP START ---
    int max_attempts = 30;
    int connected = 0;

    for (int attempt = 1; attempt <= max_attempts; attempt++)
    {
        printf("Tentative de connexion %d/%d à %s sur le port %s...\n", attempt, max_attempts, argv[1], port);

        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;

        if ((error = getaddrinfo(argv[1], port, &hints, &res)) != 0)
        {
            fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(error));
            sleep(5);
            continue;
        }

        if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0)
        {
            perror("Erreur socket()");
            freeaddrinfo(res);
            sleep(5);
            continue;
        }

        printf("Client: tentative de connexion sur %s sur le port %s\n", argv[1], port);

        if (connect(sockfd, res->ai_addr, res->ai_addrlen) == 0)
        {
            printf("Connexion réussie à %s sur le port %s !\n", argv[1], port);
            printf("Client démarré et connecté au serveur via Kubernetes Service !\n");
            connected = 1;
            freeaddrinfo(res);
            break;
        }

        perror("Error: connect failed");
        close(sockfd);
        freeaddrinfo(res);
        sleep(5);
    }

    if (!connected)
    {
        fprintf(stderr, "Impossible de se connecter au serveur après %d tentatives.\n", max_attempts);
        exit(1);
    }
    // --- RETRY LOOP END ---

    // --- Original sending loop ---
    for (i = 0; i < 100; i++)
    {
        if (i % 2) strcpy(buffer, "toto");
        else strcpy(buffer, "titi");

        if ((nbOct = send(sockfd, buffer, strlen(buffer), 0)) < 0)
            erreur("Error: send failed", 2);
        else
        {
            printf("Le client a envoyé: %s\n", buffer);
            sleep(3);
        }
    }
    // --- End sending loop ---

    close(sockfd);
    return 0;
}

void erreur(char* string, int exit_status)
{
    perror(string);
    exit(exit_status);
}