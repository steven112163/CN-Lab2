#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 8000
#define BUFFER_SIZE 22

int main(void) {
    struct sockaddr_in server, client;
    int socket_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (socket_fd == -1) {
        perror("Socket creation failed\n");
        exit(EXIT_FAILURE);
    }

    memset(&server, 0, sizeof(struct sockaddr_in));

    server.sin_family = AF_INET;
    server.sin_port = htons(8000);
    server.sin_addr.s_addr = INADDR_ANY;

    if (bind(socket_fd, (struct sockaddr *) &server, sizeof(server)) == -1) {
        perror("Socket binding failed\n");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(socket_fd, 1) == -1) {
        perror("Socket listen failed\n");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];
    char exit_string[] = "exit";
    char close_string[] = "close";
    printf("Server listening on port %d\n", PORT);
    while (1) {
        int connection_fd = accept(socket_fd, NULL, NULL);

        if (connection_fd < 0) {
            perror("Connection failed\n");
            close(socket_fd);
            exit(EXIT_FAILURE);
        }

        socklen_t client_len = sizeof(client);
        getpeername(connection_fd, (struct sockaddr *) &client, &client_len);
        char client_ip[20];
        strcpy(client_ip, inet_ntoa(client.sin_addr));
        printf("Connection from: %s\n", client_ip);

        short close_server = 0;
        while (1) {
            int read = recv(connection_fd, buffer, BUFFER_SIZE, 0);
            if (read < 0) {
                perror("Client read failed\n");
                close(connection_fd);
                close(socket_fd);
                exit(EXIT_FAILURE);
            }

            if (strcmp(buffer, exit_string) == 0) {
                printf("Exit from %s\n\n", client_ip);
                break;
            }

            if (strcmp(buffer, close_string) == 0) {
                close_server = 1;
                break;
            }

            int first, second;
            sscanf(buffer, "%d+%d", &first, &second);
            char result[BUFFER_SIZE];
            sprintf(result, "%lld", (long long) first + second);
            printf("Result: %d + %d = %s\n", first, second, result);

            if (send(connection_fd, result, BUFFER_SIZE, 0) < 0) {
                perror("Client write failed\n");
                close(connection_fd);
                close(socket_fd);
                exit(EXIT_FAILURE);
            }
        }

        shutdown(connection_fd, SHUT_RDWR);
        close(connection_fd);

        if (close_server)
            break;
    }

    close(socket_fd);
    printf("Server closed\n");
    return 0;
}