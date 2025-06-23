// =====
// client-datagram.c - Create a client that uses datagram sockets
// =====

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

void print_usage_guide()
{
    printf("Usage: client-datagram <server> <port> <message>\n");
    printf("Server must be the server's host name\n");
    printf("Port must be a number between 1024 and 65535\n");
    printf("Message must be a string of characters\n");
}

void validate_args(int argc, char ** argv) {
    if (argc != 4) {
        print_usage_guide();
        exit(1);
    }

    errno = 0;

    char * server = argv[1];

    if (errno != 0 || server == NULL || server[0] == '\0') {
        print_usage_guide();
        exit(1);
    }

    char * port = argv[2];
    long port_num = strtol(port, NULL, 0);

    if (errno != 0 || port_num < 1024 || port_num > 65535) {
        print_usage_guide();
        exit(1);
    }

    char * message = argv[3];

    if (errno != 0 || message == NULL || message[0] == '\0') {
        print_usage_guide();
        exit(1);
    }

}

void get_socket_info(char * server, char * port, struct addrinfo ** result) {
    // Set up criteria for selecting socket addresses
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    // Get socket addresses
    int status;
    if ((status = getaddrinfo(server, port, &hints, result)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(1);
    }
}

void * get_ip(struct sockaddr * address) {
    if (address->sa_family == AF_INET)
        // IPv4
        return &(((struct sockaddr_in *)address)->sin_addr);
    else
        // IPv6
        return &(((struct sockaddr_in6 *)address)->sin6_addr);
}

void send_packet(struct addrinfo * socket_info, char * message) {
    struct addrinfo * curr;
    int socket_num;
    char server_address_string[INET6_ADDRSTRLEN];

    for (curr = socket_info; curr != NULL; curr = curr->ai_next) {
        // Create socket file descriptor
        if ((socket_num =
            socket(
                curr->ai_family,
                curr->ai_socktype,
                curr->ai_protocol)) == -1) {
            perror("Failed to create socket");
            continue;
        }

        inet_ntop(
            curr->ai_family,
            get_ip((struct sockaddr *) curr->ai_addr),
            server_address_string,
            sizeof(server_address_string));

        break;
    }

    // Send packet
    int num_bytes;
    if ((num_bytes = sendto(
            socket_num,
            message,
            strlen(message),
            0,
            curr->ai_addr,
            curr->ai_addrlen)) == -1) {
        perror("Failed to send packet");
        exit(1);
    }

    close(socket_num);
}

int main(int argc, char ** argv) {
    validate_args(argc, argv);

    struct addrinfo * socket_info;
    get_socket_info(argv[1], argv[2], &socket_info);

    send_packet(socket_info, argv[3]);
    freeaddrinfo(socket_info);

    return 0;
}
