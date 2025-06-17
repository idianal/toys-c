// =====
// client-stream.c - Create a client that uses stream sockets
// =====

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

void print_usage_guide()
{
    printf("Usage: client-stream <server> <port>\n");
    printf("Server must be the server's host name\n");
    printf("Port must be a number between 1024 and 65535\n");
}

void validate_args(int argc, char ** argv) {
    if (argc != 3) {
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
}

void get_socket_info(char * server, char * port, struct addrinfo ** result) {
    // Set up criteria for selecting socket addresses
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

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

int connect_to_socket(struct addrinfo * socket_info) {
    struct addrinfo * curr;
    int socket_num_listener;
    char server_address_string[INET6_ADDRSTRLEN];

    for (curr = socket_info; curr != NULL; curr = curr->ai_next) {
        // Create socket file descriptor
        if ((socket_num_listener =
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

        printf("Attempting connection to %s\n", server_address_string);

        // Connect to the socket
        if (connect(
                socket_num_listener,
                curr->ai_addr,
                curr->ai_addrlen) == -1) {
            close(socket_num_listener);
            perror("Failed to connect to socket");
            continue;
        }

        break;
    }

    if (curr == NULL) {
        fprintf(stderr, "Failed to connect to any sockets\n");
        exit(1);
    }

    // Print connection details
    inet_ntop(
        curr->ai_family,
        get_ip((struct sockaddr *) curr->ai_addr),
        server_address_string,
        sizeof(server_address_string));

    printf("Connected to %s\n", server_address_string);

    return socket_num_listener;
}

char * receive_message(int socket_num, char * message, int max_length) {
    int num_bytes;
    if ((num_bytes = recv(socket_num, message, max_length - 1, 0)) == -1) {
        perror("Failed to receive message");
        exit(1);
    }

    message[num_bytes] = '\0';
    return message;
}

int main (int argc, char ** argv) {
    validate_args(argc, argv);

    struct addrinfo * socket_info;
    get_socket_info(argv[1], argv[2], &socket_info);

    int socket_num = connect_to_socket(socket_info);

    freeaddrinfo(socket_info);

    int max_length = 100;
    char message[max_length];

    printf(
        "Message received: '%s'\n",
        receive_message(socket_num, message, max_length));

    close(socket_num);

    return 0;
}
