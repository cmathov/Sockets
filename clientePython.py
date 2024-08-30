import socket

SERVER_IP = "127.0.0.1"
PORT = 8080
BUFSIZE = 1024

if __name__ == "__main__":
    # Crear el socket del cliente
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    try:
        # Conectar al servidor
        client_socket.connect((SERVER_IP, PORT))
        print("Esta conectado al servidor.")

        while True:
            # Recibir mensaje del servidor
            try:
                data = client_socket.recv(BUFSIZE).decode()
                if not data:
                    print("Esta desconectado del servidor.")
                    break
                print(data)
            except socket.error as e:
                print(f"Error al recibir el mensaje del servidor: {e}")
                break

            # Leer input del usuario
            user_input = input()
            
            # Si el input está vacío, cambiarlo a "vacio"
            if not user_input.strip():
                user_input = "vacio"
            
            # Enviar mensaje al servidor
            try:
                client_socket.sendall(user_input.encode())
            except socket.error as e:
                print(f"Error al enviar el mensaje al servidor: {e}")
                break

    finally:
        client_socket.close()
