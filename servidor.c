#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <winsock2.h>

#define PUERTO 8080

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
   cadena[longitud] = '\0'; // Añadir el carácter nulo al final de la cadena
}

void generarContra(int longitud, char *cadena) {
   const char caracteres[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
   int numCaracteres = sizeof(caracteres) - 1;

   srand(time(0));

   for (int i = 0; i < longitud; i++) {
      int indice = rand() % numCaracteres;
      cadena[i] = caracteres[indice];
   }
   cadena[longitud] = '\0'; // Añadir el carácter nulo al final de la cadena
}

int main() {
   WSADATA wsaData;
   SOCKET serverSocket, clientSocket;
   struct sockaddr_in serverAddr, clientAddr;
   int clientAddrSize = sizeof(clientAddr);
   char buffer[1024];
   int recvSize;
   int opc = 0;
   char message[200];

   // Inicializar Winsock
   if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
      printf("Fallo al inicializar Winsock\n");
      return 1;
   }

   // Crear socket
   serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   if (serverSocket == INVALID_SOCKET) {
      printf("Fallo al crear el Socket\n");
      WSACleanup();
      return 1;
   }

   // Configurar dirección del servidor
   serverAddr.sin_family = AF_INET;
   serverAddr.sin_addr.s_addr = INADDR_ANY;
   serverAddr.sin_port = htons(PUERTO);

   // Enlazar el socket
   if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
      printf("Fallo al enlazar\n");
      closesocket(serverSocket);
      WSACleanup();
      return 1;
   }

   // Escuchar conexiones
   if (listen(serverSocket, 3) == SOCKET_ERROR) {
      printf("Fallo al escuchar la conexion\n");
      closesocket(serverSocket);
      WSACleanup();
      return 1;
   }

   printf("Esperando una conexion...\n");

   // Aceptar una conexión
   clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrSize);
   if (clientSocket == INVALID_SOCKET) {
      printf("Fallo al aceptar la conexion\n");
      closesocket(serverSocket);
      WSACleanup();
      return 1;
   }

   printf("Conexion exitosa\n");

   boolean seguir = TRUE;
   while (seguir){
      // Enviar menu
      sprintf(message, "\e[1;1H\e[2JBuenos dias, ¿Que desea generar?\n 1) Un nombre de usuario\n 2) Una contraseña\n 3) Salir\n\nOpcion: ");
      send(clientSocket, message, strlen(message), 0);

      // Recibir opcion del menu
      recvSize = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
      if (recvSize == SOCKET_ERROR) {
         printf("Fallo al recibir el mensaje\n");
      } else {
         printf("Mensaje: %s\n", buffer);
         // Convertir la cadena recibida a un entero
         opc = atoi(buffer);

         if(opc == 1){
            //Opcion 1 generar nombre de usuario
            sprintf(message, "\nElija la cantidad de caracteres para su nombre de usuario (Entre 5 y 15)\n\nCaracteres: ");
            send(clientSocket, message, strlen(message), 0);
            
            // Recibir cantidad de caracteres
            recvSize = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
            
            if (recvSize == SOCKET_ERROR) {
               printf("Fallo al recibir el mensaje\n");
            } else {
               // Convertir la cadena recibida a un entero
               opc = atoi(buffer);
               printf("Caracteres: %d\n", opc);
               sprintf(message, "\nError cantidad de caracteres incorrecta\n\n");
               if(opc >= 5 && opc <= 15){
                  char usuario[opc + 1]; // +1 para el carácter nulo
                  generarUsuario(opc, usuario);
                  sprintf(message, "\nSu usuario es: %s", usuario);
               }
               send(clientSocket, message, strlen(message), 0);
            }


         }else if(opc == 2){
            //Opcion 2 generar contraseña
            sprintf(message, "\nElija la cantidad de caracteres para su contraseña (Entre 8 y 49)\n\nCaracteres: ");
            send(clientSocket, message, strlen(message), 0);
            
            // Recibir cantidad de caracteres
            recvSize = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
            
            if (recvSize == SOCKET_ERROR) {
               printf("Fallo al recibir el mensaje\n");
            } else {
               // Convertir la cadena recibida a un entero
               opc = atoi(buffer);
               printf("Caracteres: %d\n", opc);
               sprintf(message, "\nError cantidad de caracteres incorrecta\n\n");
               if(opc >= 8 && opc < 50){
                  char contra[opc + 1]; // +1 para el carácter nulo
                  generarContra(opc, contra);
                  sprintf(message, "\nSu contraseña es: %s", contra);
               }
               send(clientSocket, message, strlen(message), 0);
            }
         }else if(opc == 3){
            //SALIR
            sprintf(message, "SALIR");
            send(clientSocket, message, strlen(message), 0);
            closesocket(clientSocket);
            seguir = FALSE;
         }else{
            sprintf(message, "\nError numero de opcion incorrecto precione cualquier tecla para continuar\n\n");
            send(clientSocket, message, strlen(message), 0);
         }
         fflush(stdin);
         recvSize = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
         if (recvSize == SOCKET_ERROR) {
            printf("Fallo al recibir el mensaje\n");
         }
      }
   }

   // Cerrar sockets
   closesocket(clientSocket);
   closesocket(serverSocket);
   WSACleanup();

   return 0;
}

