#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

char posicion(char c, int conver) {
    if (c >= 'A' && c <= 'Z') {
        c -= 'A';
        c += conver;
        c %= 26;
        c += 'A';
    } else if (c >= 'a' && c <= 'z') {
        c -= 'a';
        c += conver;
        c %= 26;
        c += 'a';
    }
    return c;
}

void cesar(char *cadena, int conver, size_t size) {
    for (size_t i=0; i < size; i++) {
        cadena[i] = posicion(cadena[i], conver);
    } 
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("Usage: %s <número desencriptado> \n", argv[0]);
        return EXIT_FAILURE;
    }

    int n = atoi(argv[1]);

    // Creating a socket
    int listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener < 0) {
        perror("Failed to create socket");
        return EXIT_FAILURE;
    }  

    // Binding the socket to an address 
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1026);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int ret = bind(listener, (struct sockaddr*) &addr, sizeof(struct sockaddr_in));
    if (ret < 0) {
        perror("Failed to bind socket to an address");
        return EXIT_FAILURE;
    }

    // Now setting up the listener
    ret = listen(listener, 16);
    if (ret < 0) {
        perror("Failed setting the socker into listeing mode");
        return EXIT_FAILURE;
    }

    // Accept the connection as listener
    struct sockaddr_in client_addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    int tcp_socket  = accept(listener, (struct sockaddr*) &client_addr, &addr_size);
    if (tcp_socket < 0) {
        perror("Failed to accept as listener");
        return EXIT_FAILURE;
    }

    // Printing the connected socket address
    printf("Connected to %s:%hu \n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));


    // Receiving the message
    char *buf = malloc(1024);
    ssize_t size = recv(tcp_socket, buf, 1024, 0);
    if (size < 0) {
        perror("Failed to receive message");
        return EXIT_FAILURE;
    }

    printf("Encriptado: %.*s\n", (int) size, buf);

    cesar(buf, n, size); 

    printf("Desencriptado: %.*s\n", (int) size, buf);

    free(buf);

    close(listener);


    return 0;
}