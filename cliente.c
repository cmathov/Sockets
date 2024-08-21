#include <stdio.h>
#include <winsock2.h>

#define PUERTO 8080
#define IP "127.0.0.1"

boolean recibirMensajeYContestar(SOCKET clientSocket){
    char buffer[1024];
    char numero[10];
    int recvSize = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (recvSize == SOCKET_ERROR) {
        printf("Fallo al recibir el mensaje\n");
    } else {
        buffer[recvSize] = '\0'; // Asegurarse de terminar la cadena
        if(strcmp(buffer, "SALIR") != 0){ //Si el mensaje no es "SALIR"
            printf("%s", buffer);

            // Leer número ingresado por el usuario
            fflush(stdin);
            if (fgets(numero, sizeof(numero), stdin) == NULL) {
                sprintf(numero, "0");
            }
            numero[strcspn(numero, "\n")] = '\0';

            // Enviar respuesta al servidor
            send(clientSocket, numero, strlen(numero), 0);

            return TRUE;   
        }
        return FALSE;
    }
}

int main() {
    WSADATA wsaData;
    SOCKET clientSocket;
    struct sockaddr_in serverAddr;


    // Inicializar Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Fallo al inicializar Winsock\n");
        return 1;
    }

    // Crear socket
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        printf("Fallo al crear el Socket\n");
        WSACleanup();
        return 1;
    }

    // Configurar dirección del servidor
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(IP);
    serverAddr.sin_port = htons(PUERTO);

    // Conectar al servidor
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Fallo al conectar con el servidor\n");
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // Recibir mensaje del servidor
    boolean seguir = TRUE;
    while (seguir){
        seguir = recibirMensajeYContestar(clientSocket);
    }


    // Cerrar socket
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
