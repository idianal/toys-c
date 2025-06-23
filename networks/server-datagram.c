// =====
// server-datagram.c - Create a server that uses datagram sockets
// =====

#include <arpa/inet.h>
#include <errno.h>
#include <limits.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_PENDING_CONNECTIONS 10

void print_usage_guide()
{
    printf("Usage: server-datagram <port>\n");
    printf("Port must be a number between 1024 and 65535\n");
}

void validate_args(int argc, char ** argv) {
    if (argc != 2) {
        print_usage_guide();
        exit(1);
    }

    errno = 0;
    char * port = argv[1];
    long port_num = strtol(port, NULL, 0);

    if (errno != 0 || port_num < 1024 || port_num > 65535) {
        print_usage_guide();
        exit(1);
    }
}

void get_socket_info(char * port, struct addrinfo ** result) {
    // Set up criteria for selecting socket addresses
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_DGRAM;

    // Get socket addresses
    int status;
    if ((status = getaddrinfo(NULL, port, &hints, result)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(1);
    }
}

void get_host_name(char * result, int length) {
    if (gethostname(result, length) == -1) {
        perror("Failed to get server host name");
        exit(1);
    }
}

int bind_to_socket(struct addrinfo * socket_info) {
    struct addrinfo * curr;
    int socket_num;

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

        // Configure socket option to allow reuse
        int reuse_address = 1;

        if (setsockopt(
                socket_num,
                SOL_SOCKET,
                SO_REUSEADDR,
                &reuse_address,
                sizeof(reuse_address)) == -1) {
            perror("Failed to set socket option");
            exit(1);
        }

        // Bind to the socket
        if (bind(socket_num, curr->ai_addr, curr->ai_addrlen) == -1) {
            close(socket_num);
            perror("Failed to bind to socket");
            continue;
        }

        break;
    }

    if (curr == NULL) {
        fprintf(stderr, "Failed to bind to any sockets\n");
        exit(1);
    }

    return socket_num;
}

void reap_dead_process(int signal_num) {
    (void) signal_num;

    // Save error number
    int saved_error_num = errno;

    while (waitpid(-1, NULL, WNOHANG) > 0);

    // Restore error number
    errno = saved_error_num;
}

void set_signal_handling() {
    struct sigaction sa;
    sa.sa_handler = reap_dead_process;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("Failed to set signal handling");
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

void receive_packets(int socket_num, char * host_name) {
    int num_bytes;
    int max_length = 100;
    char buffer[max_length];
    struct sockaddr_storage client_address;
    socklen_t client_address_size = sizeof(client_address);

    while (1) {
        printf("%s: Waiting for requests...\n", host_name);

        // Receive data from the socket
        if ((num_bytes = recvfrom(
                socket_num,
                buffer,
                max_length - 1,
                0,
                (struct sockaddr *) &client_address,
                &client_address_size)) == -1) {
            perror("Failed to process request");
            continue;
        }

        // Print information about received data
        char client_address_string[INET6_ADDRSTRLEN];
        printf(
            "Received %d-byte packet from %s\n",
            num_bytes,
            inet_ntop(
                client_address.ss_family,
                get_ip((struct sockaddr *) &client_address),
                client_address_string,
                sizeof(client_address_string)));

        if (num_bytes > 0)
            buffer[num_bytes] = '\0';

        printf("Packet contains '%s'\n", buffer);
    }

    close(socket_num);
}

int main(int argc, char ** argv) {
    validate_args(argc, argv);

    struct addrinfo * socket_info;
    get_socket_info(argv[1], &socket_info);

    char host_name[HOST_NAME_MAX + 1];
    get_host_name(host_name, sizeof(host_name));

    int socket_num = bind_to_socket(socket_info);

    freeaddrinfo(socket_info);
    set_signal_handling();

    receive_packets(socket_num, host_name);

    return 0;
}
