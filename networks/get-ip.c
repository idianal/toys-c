// =====
// get-ip.c - Print IP addresses for a given host
// =====

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

int main(int argc, char * argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: get-ip <hostname>\n");
        return 1;
    }

    // Set up criteria for selecting socket addresses
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    // Get socket addresses
    int status;
    struct addrinfo * res;

    if ((status = getaddrinfo(argv[1], NULL, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 2;
    }

    printf("IP addresses for %s:\n", argv[1]);

    // Print socket addresses
    struct addrinfo * current;
    char ip_string[INET6_ADDRSTRLEN];

    for (current = res; current != NULL; current = current->ai_next) {
        void * address;
        char * version;

        // Get pointer to the address
        if (current->ai_family == AF_INET) {
            // IPv4
            struct sockaddr_in * ipv4_address_info =
                (struct sockaddr_in *) current->ai_addr;
            address = &(ipv4_address_info->sin_addr);
            version = "IPv4";
        } else {
            // IPv6
            struct sockaddr_in6 * ipv6_address_info =
                (struct sockaddr_in6 *) current->ai_addr;
            address = &(ipv6_address_info->sin6_addr);
            version = "IPv6";
        }

        // Print address
        inet_ntop(current->ai_family, address, ip_string, sizeof ip_string);
        printf("  %s: %s\n", version, ip_string);
    }

    freeaddrinfo(res);
    return 0;
}
