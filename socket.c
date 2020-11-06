#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


/* Server port  */
#define PORT 4242


/* Buffer length */
#define BUFFER_LENGTH 4096



/*
 * Main execution of the server program of the simple protocol
 */
int
main(void) {

    /* Cliente e Servdor */
    struct sockaddr_in client, server;

    
    int serverfd, clientfd;

    char buffer[BUFFER_LENGTH];

    fprintf(stdout, "Iniciando servidor \n");

    /* Criando o socket */
    serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if(serverfd == -1) {
        perror("Não foi possível criar o servidor:");
        return EXIT_FAILURE;
    }
    fprintf(stdout, "Servidor criado com sucesso: %d\n", serverfd);


    /* Definindo as propriedades do socket */
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    memset(server.sin_zero, 0x0, 8);


    /* Erro caso a porta esteja em uso */
    int yes = 1;
    if(setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR,
                  &yes, sizeof(int)) == -1) {
        perror("Erro:");
        return EXIT_FAILURE;
    }


    
    if(bind(serverfd, (struct sockaddr*)&server, sizeof(server)) == -1 ) {
        perror("Erro:");
        return EXIT_FAILURE;
    }


    /* Iniciando conexão */
    if(listen(serverfd, 1) == -1) {
        perror("Listen error:");
        return EXIT_FAILURE;
    }
    fprintf(stdout, "Funcionando na porta: %d\n", PORT);

    socklen_t client_len = sizeof(client);
    if ((clientfd=accept(serverfd,
        (struct sockaddr *) &client, &client_len )) == -1) {
        perror("Erro de conexão com o cliente:");
        return EXIT_FAILURE;
    }


    /* Mensagem de bem-vindo */
    strcpy(buffer, "Olá, seja bem-vindo!\n\0");


    /* Enviando mensagens */
    if (send(clientfd, buffer, strlen(buffer), 0)) {
        fprintf(stdout, "Cliente conectado \nEsperando mensagem ...\n");

        
        do {

            
            memset(buffer, 0x0, BUFFER_LENGTH);

            
            int message_len;
            if((message_len = recv(clientfd, buffer, BUFFER_LENGTH, 0)) > 0) {
                buffer[message_len - 1] = '\0';
                printf("Client says: %s\n", buffer);
            }


            /* Finalizando conexão com a mensagem de adeus */
            if(strcmp(buffer, "adeus") == 0) {
                send(clientfd, "adeus", 3, 0);
            } else {
                send(clientfd, "yep\n", 4, 0);
            }

        } while(strcmp(buffer, "adeus"));
    }

    
    close(clientfd);

    
    close(serverfd);

    printf("Conexão encerrada\n\n");

    return EXIT_SUCCESS;
}