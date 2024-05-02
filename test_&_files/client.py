import socket

clients = []

def connect_to_server(host, port, i):
    try:
        # Create a socket object
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        
        # Connect to the server
        client_socket.connect((host, port))
        
        print(f"Connected to {host} on port {port}")
        
        # Send data to the server
        message = f"pass 1\nuser x{i} x x x\nnick user{i}\njoin #ch{i}\n"
        # message = f"nick user{i}\r\n"
        client_socket.sendall(message.encode())
        print("Message sent to server:", message)
        
        # Receive data from the server
        response = client_socket.recv(512)
        print("Received:", response.decode())
        
        # Close the connection
        clients.append(client_socket)
        # client_socket.close()
        
    except ConnectionRefusedError:
        print("Connection refused. Make sure the server is running and the port is correct.")
    except Exception as e:
        print("An error occurred:", e)

if __name__ == "__main__":
    host = 'localhost'  # Change this to your server's IP address
    port = 6667  # Change this to the port your server is listening on
    for i in range(4):
        connect_to_server(host, port, i)
    a = input()
    for i in range(4):
        clients[i].close()
