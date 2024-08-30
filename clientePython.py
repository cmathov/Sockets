import socket

SERVER_IP = "127.0.0.1"
PORT = 8080
BUFSIZE = 1024

def main():
    try:
        # Crear el socket del cliente
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    except socket.error as e:
        print(f"No se pudo crear el socket: {e}")
        return

    try:
        # Conectar al servidor
        client_socket.connect((SERVER_IP, PORT))
    except socket.error as e:
        print(f"Conexión fallida: {e}")
        client_socket.close()
        return

    print("Esta conectado al servidor.")

    try:
        while True:
            # Recibir mensaje del servidor
            try:
                data = client_socket.recv(BUFSIZE).decode()
                if not data:
                    print("Esta desconectado del servidor.")
                    break
            except socket.error as e:
                print(f"Error al recibir el mensaje del servidor: {e}")
                break

            # Mostrar mensaje del servidor
            print(data)

            # Leer input del usuario
            user_input = input()
            
            # Si el input está vacío cambiarlo a "vacio"
            if not user_input.strip():
                user_input = "vacio"

            # Enviar mensaje al servidor
            try:
                client_socket.send(user_input.encode('utf-8'))
            except socket.error as e:
                print(f"Error al enviar el mensaje al servidor: {e}")
                break

    finally:
        client_socket.close()

if __name__ == "__main__":
    main()
