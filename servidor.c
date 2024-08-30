#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <winsock2.h>

#define PORT 8080
#define BUFSIZE 1024

void generarUsuario(int longitud, char *cadena){
   const char vocales[] = {"aeiou"};
   const char consonantes[] = {"bcdfghjklmnpqrstvwxyz"};
   int numVocales = 5;
   int numConsonantes = 21;

   srand(time(0));

   int random = rand() % 2;  // Permite elegir el tipo de la primer letra (consonate o vocal) al azar
   for (int i = 0; i < longitud; i++) {
      if ((i+random) % 2 == 0) {
            int indice = rand() % numVocales;
            cadena[i] = vocales[indice];
      } else {
            int indice = rand() % numConsonantes;
            cadena[i] = consonantes[indice];
      }
   }
   cadena[longitud] = '\0';
}

void generarContra(int longitud, char *cadena) {
   const char caracteres[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
   int numCaracteres = sizeof(caracteres) - 1;

   srand(time(0));

   for (int i = 0; i < longitud; i++) {
      int indice = rand() % numCaracteres;
      cadena[i] = caracteres[indice];
   }
   cadena[longitud] = '\0';
}

void manejar_cliente(SOCKET client_socket) {
   char buffer[BUFSIZE];
   int opc;
   char *menu = "\e[1;1H\e[2JBuenos dias, ¿Que desea generar?\n 1) Un nombre de usuario\n 2) Una contraseña\n 3) Salir\n\nOpcion: ";
   char message[200];
   while (1) {
      send(client_socket, menu, strlen(menu), 0);

      // Recibir la opción del cliente
      memset(buffer, 0, BUFSIZE);
      int bytes_received = recv(client_socket, buffer, BUFSIZE, 0);
      if (bytes_received == SOCKET_ERROR) {
         printf("Error al recibir el mensaje del cliente\n");
         return;
      }

      // Convertir la opción a entero
      opc = strtol(buffer, NULL, 10);

      if(opc == 1){  // Generar nombre de usuario
         sprintf(message, "\nElija la cantidad de caracteres para su nombre de usuario (Entre 5 y 15)\n\nCaracteres: ");
         send(client_socket, message, strlen(message), 0);
         
         // Recibir cantidad de caracteres
         bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
         if (bytes_received == SOCKET_ERROR) {
            printf("Fallo al recibir el mensaje\n");
            return;
         }

         // Convertir la cadena recibida a un entero
         opc = strtol(buffer, NULL, 10);

         // Mientras la cantidad de caracteres no sea correcta seguir pidiendo input
         while((opc >= 5 && opc <= 15) == FALSE){
            sprintf(message, "\nError cantidad de caracteres incorrecta, ingrese un numero mayor a 5 e igual o menor a 15\n\n");
            send(client_socket, message, strlen(message), 0);
            bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
            if (bytes_received == SOCKET_ERROR) {
               printf("Fallo al recibir el mensaje\n");
               return;
            }
            opc = strtol(buffer, NULL, 10);
         }

         // Generar usuario y enviarlo
         char usuario[opc + 1];
         generarUsuario(opc, usuario);
         sprintf(message, "\nSu usuario es: %s", usuario);
         send(client_socket, message, strlen(message), 0);

         // Recibir confirmacion vacia
         bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
         if (bytes_received == SOCKET_ERROR) {
            printf("Fallo al recibir el mensaje\n");
            return;
         }
      } else if(opc == 2){ // Opcion 2 generar contraseña
         sprintf(message, "\nElija la cantidad de caracteres para su contraseña (Entre 8 y 49)\n\nCaracteres: ");
         send(client_socket, message, strlen(message), 0);
         
         // Recibir cantidad de caracteres
         bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
         if (bytes_received == SOCKET_ERROR) {
            printf("Fallo al recibir el mensaje\n");
            return;
         } 

         // Convertir la cadena recibida a un entero
         opc = strtol(buffer, NULL, 10);

         // Mientras la cantidad de caracteres no sea correcta seguir pidiendo input
         while((opc >= 8 && opc < 50) == FALSE){
            sprintf(message, "\nError cantidad de caracteres incorrecta, ingrese un numero mayor a 8 y menor a 50\n\n");
            send(client_socket, message, strlen(message), 0);
            bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
            if (bytes_received == SOCKET_ERROR) {
               printf("Fallo al recibir el mensaje\n");
               return;
            }
            opc = strtol(buffer, NULL, 10);
         }
         
         // Generar copntraseña y enviarla
         char contra[opc + 1];
         generarContra(opc, contra);
         sprintf(message, "\nSu contraseña es: %s", contra);
         send(client_socket, message, strlen(message), 0);

         // Recibir confirmacion vacia
         bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
         if (bytes_received == SOCKET_ERROR) {
            printf("Fallo al recibir el mensaje\n");
            return;
         }
      }else if (opc == 3) {  // Salir
         printf("Cliente desconectado.\n");
         break;
      }else{
         //Opcion incorrecta
      }
   }
}

int main() {
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server, client;
    int client_len = sizeof(client);

    // Inicializar Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Error en WSAStartup(): %d\n", WSAGetLastError());
        return 1;
    }

    // Crear el socket del servidor
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("No se pudo crear el socket: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Configurar la estructura sockaddr_in
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    // Asociar el socket al puerto
    if (bind(server_socket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Bind falló: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    // Escuchar conexiones entrantes
    listen(server_socket, 3);
    printf("Esperando conexiones...\n");

    // Aceptar y manejar conexiones entrantes
    while ((client_socket = accept(server_socket, (struct sockaddr *)&client, &client_len)) != INVALID_SOCKET) {
        printf("Cliente conectado.\n");
        manejar_cliente(client_socket);
        closesocket(client_socket);
        printf("Esperando nuevo cliente...\n");
    }

    closesocket(server_socket);
    WSACleanup();

    return 0;
}