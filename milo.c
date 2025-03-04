#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

char *cesar(char *cadena, int conver, size_t size) {
    char *newstring = malloc(size+1);
    for (size_t i=0; i < size; i++) {
        newstring[i] = posicion(cadena[i], conver);
    } 
    newstring[size] = '\0';
    return newstring;
}


int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("Please provide a single string argument\n");
        return EXIT_FAILURE;
    }

    char *input = argv[1];
    size_t inputlen = strlen(input);

    // Cesar shift
    char *encriptado = cesar(input, 13, inputlen);


    // Creating a socket
    int tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_socket < 0) {
        perror("Failed to create socket");
        return EXIT_FAILURE;
    }  

    // Binding the socket to an address 
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1025);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int ret = bind(tcp_socket, (struct sockaddr*) &addr, sizeof(struct sockaddr_in));
    if (ret < 0) {
        perror("Failed to bind socket to an address");
        return EXIT_FAILURE;
    }

    // Connection the socket to the listener
    addr.sin_port = htons(1026);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    ret = connect(tcp_socket, (struct sockaddr*) &addr, sizeof(struct sockaddr_in));
    if (ret < 0) {
        perror("Failed to connect to listener");
        return EXIT_FAILURE;
    }
    
    // Sending the data or message to the listener
    ssize_t size = send(tcp_socket, encriptado, inputlen, 0);
    if (size < 0) {
        perror("Failed to send message to listener");
        return EXIT_FAILURE;
    }

    free(encriptado);
    
    close(tcp_socket);

    return 0;
}