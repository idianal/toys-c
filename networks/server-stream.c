// =====
// server-stream.c - Create a server that uses stream sockets
// =====

#include <arpa/inet.h>
#include <errno.h>
#include <limits.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_PENDING_CONNECTIONS 10

void print_usage_guide()
{
    printf("Usage: server-stream <port>\n");
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
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;

    // Get socket addresses
    int status;
    if ((status = getaddrinfo(NULL, port, &hints, result)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(1);
    }
}

int bind_to_socket(struct addrinfo * socket_info) {
    struct addrinfo * curr;
    int socket_num_listener;

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

        // Configure socket option to allow reuse
        int reuse_address = 1;

        if (setsockopt(
                socket_num_listener,
                SOL_SOCKET,
                SO_REUSEADDR,
                &reuse_address,
                sizeof(reuse_address)) == -1) {
            perror("Failed to set socket option");
            exit(1);
        }

        // Bind to the socket
        if (bind(socket_num_listener, curr->ai_addr, curr->ai_addrlen) == -1) {
            close(socket_num_listener);
            perror("Failed to bind to socket");
            continue;
        }

        break;
    }

    if (curr == NULL) {
        fprintf(stderr, "Failed to bind to any sockets\n");
        exit(1);
    }

    return socket_num_listener;
}

void wait_for_requests(int socket_num_listener, char * host_name) {
    if (listen(socket_num_listener, MAX_PENDING_CONNECTIONS) == -1) {
        perror("Failed to listen to requests");
        exit(1);
    }

    printf("%s: Waiting for requests...\n", host_name);
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

void get_host_name(char * result, int length) {
    if (gethostname(result, length) == -1) {
        perror("Failed to get server host name");
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

void process_request(int socket_num_listener, char * host_name) {
    int socket_num_connection;
    struct sockaddr_storage client_address;
    socklen_t client_address_size;
    char client_address_string[INET6_ADDRSTRLEN];

    // Create return message
    char * message_prefix = "Hello from server ";
    char * message_suffix = "!";

    size_t message_length = strlen(message_prefix) +
        strlen(host_name) +
        strlen(message_suffix);

    char * message = malloc(message_length + 1);

    snprintf(
        message,
        message_length + 1,
        "%s%s%s",
        message_prefix,
        host_name,
        message_suffix);

    while(1) {
        // Accept connection
        client_address_size = sizeof(client_address);
        socket_num_connection =
            accept(
                socket_num_listener,
                (struct sockaddr *) &client_address,
                &client_address_size);

        if (socket_num_connection == -1) {
            perror("Failed to accept a connection");
            continue;
        }

        inet_ntop(
            client_address.ss_family,
            get_ip((struct sockaddr *) &client_address),
            client_address_string,
            sizeof(client_address_string));

        printf("Received connection from %s\n", client_address_string);

        if (!fork()) {
            // Child process doesn't need the listener
            close(socket_num_listener);

            // Send return message
            if (send(
                    socket_num_connection,
                    message,
                    strlen(message),
                    0) == -1) {
                perror("Failed to send return message");
            }

            close(socket_num_connection);
            exit(0);
        }

        // Parent process doesn't need the connection
        close(socket_num_connection);
    }

    free(message);
}

int main(int argc, char ** argv) {
    validate_args(argc, argv);

    struct addrinfo * socket_info;
    get_socket_info(argv[1], &socket_info);

    char host_name[HOST_NAME_MAX + 1];
    get_host_name(host_name, sizeof(host_name));

    int socket_num_listener = bind_to_socket(socket_info);
    wait_for_requests(socket_num_listener, host_name);

    freeaddrinfo(socket_info);
    set_signal_handling();

    process_request(socket_num_listener, host_name);

    return 0;
}
