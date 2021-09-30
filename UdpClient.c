#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ADDRESS "127.0.0.1"
#define PORT 8000

int main(void) {
    struct sockaddr_in server;
    socklen_t server_len = sizeof(server);
    int socket_fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (socket_fd == -1) {
        perror("Socket creation failed\n");
        exit(EXIT_FAILURE);
    }

    memset(&server, 0, sizeof(struct sockaddr_in));

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    if (inet_pton(AF_INET, ADDRESS, &server.sin_addr) == 0) {
        perror("Address is invalid\n");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    char exit_string[] = "exit";
    char close_string[] = "close";
    while (1) {
        printf("Enter two integers (e.g. 1+1): ");
        char question[21];
        scanf("%s", question);

        if (strcmp(question, exit_string) == 0)
            break;

        if (sendto(socket_fd, question, 21, 0, (struct sockaddr *) &server, server_len) < 0) {
            perror("Client write failed\n");
            close(socket_fd);
            exit(EXIT_FAILURE);
        }

        if (strcmp(question, close_string) == 0)
            break;

        char result[10];
        int read = recvfrom(socket_fd, result, 10, 0, (struct sockaddr *) &server, &server_len);
        if (read < 0) {
            perror("Client read failed\n");
            close(socket_fd);
            exit(EXIT_FAILURE);
        }
        printf("The result is %s\n", result);
    }

    shutdown(socket_fd, SHUT_RDWR);
    close(socket_fd);

    return 0;
}