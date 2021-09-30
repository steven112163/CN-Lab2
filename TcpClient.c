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
    struct sockaddr_in client;
    int socket_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (socket_fd == -1) {
        perror("Socket creation failed\n");
        exit(EXIT_FAILURE);
    }

    memset(&client, 0, sizeof(struct sockaddr_in));

    client.sin_family = AF_INET;
    client.sin_port = htons(PORT);

    if (inet_pton(AF_INET, ADDRESS, &client.sin_addr) == 0) {
        perror("Address is invalid\n");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    if (connect(socket_fd, (struct sockaddr *)&client, sizeof(struct sockaddr_in)) == -1) {
        perror("Connection failed\n");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    char exit_string[] = "exit";
    char close_string[] = "close";
    while (1) {
        printf("Enter two integers (e.g. 1+1): ");
        char question[21];
        scanf("%s", question);

        if (send(socket_fd, question, 21, 0) < 0) {
            perror("Client write failed\n");
            close(socket_fd);
            exit(EXIT_FAILURE);
        }

        if (strcmp(question, exit_string) == 0 || strcmp(question, close_string) == 0)
            break;

        char result[10];
        int read = recv(socket_fd, result, 10, 0);
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

