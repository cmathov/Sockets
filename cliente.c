#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8080
#define BUFSIZE 1024

int main() {
    WSADATA wsa;
    SOCKET client_socket;
    struct sockaddr_in server;
    char buffer[BUFSIZE];

    // Inicializar Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Error en WSAStartup(): %d\n", WSAGetLastError());
        return 1;
    }

    // Crear el socket del cliente
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("No se pudo crear el socket: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Configurar la estructura sockaddr_in del servidor
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(SERVER_IP);
    server.sin_port = htons(PORT);

    // Conectarse al servidor
    if (connect(client_socket, (struct sockaddr *)&server, sizeof(server)) < 0) {
        printf("Conexión fallida: %d\n", WSAGetLastError());
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }

    printf("Conectado al servidor.\n");

    while (1) {
        // Recibir mensaje del servidor
        memset(buffer, 0, BUFSIZE);
        if (recv(client_socket, buffer, BUFSIZE, 0) == SOCKET_ERROR) {
            printf("Error al recibir el mensaje del servidor\n");
            break;
        }

        // Mostrar mensaje del servidor
        printf("%s", buffer);

        // Leer input del usuario
        fgets(buffer, BUFSIZE, stdin);
        
        // Si el input esta vacio cambiarlo a "vacio"
        buffer[strcspn(buffer, "\n")] = 0;
        if (buffer[0] == '\0') {
            strcpy(buffer, "vacio");
        }

        // Enviar mensaje al servidor
        if (send(client_socket, buffer, strlen(buffer), 0) < 0) {
            printf("Error al enviar el mensaje al servidor\n");
            break;
        }
    }

    printf("Desconectandose del servidor.\n");
    closesocket(client_socket);
    WSACleanup();

    return 0;
}
