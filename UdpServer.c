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
    socklen_t client_len = sizeof(client);
    int socket_fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

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

    char buffer[BUFFER_SIZE];
    char exit_string[] = "exit";
    char close_string[] = "close";
    printf("Server listening on port %d\n", PORT);
    while (1) {
        int receive_fd = recvfrom(socket_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr *) &client, &client_len);
        char client_ip[20];
        strcpy(client_ip, inet_ntoa(client.sin_addr));
        printf("Packet from: %s\n", client_ip);

        if (receive_fd < 0) {
            perror("Receiving the packet failed\n");
            close(receive_fd);
            close(socket_fd);
            exit(EXIT_FAILURE);
        }

        if (strcmp(buffer, exit_string) == 0) {
            shutdown(receive_fd, SHUT_RDWR);
            close(receive_fd);
            continue;
        }

        if (strcmp(buffer, close_string) == 0) {
            shutdown(receive_fd, SHUT_RDWR);
            close(receive_fd);
            break;
        }

        int first, second;
        sscanf(buffer, "%d+%d", &first, &second);
        char result[BUFFER_SIZE];
        sprintf(result, "%lld", (long long) first + second);
        printf("Result: %d + %d = %s\n", first, second, result);

        if (sendto(socket_fd, result, BUFFER_SIZE, 0, (struct sockaddr *) &client, client_len) < 0) {
            perror("Client write failed\n");
            close(receive_fd);
            close(socket_fd);
            exit(EXIT_FAILURE);
        }

        shutdown(receive_fd, SHUT_RDWR);
        close(receive_fd);
    }

    close(socket_fd);
    printf("Server closed\n");
    return 0;
}