import socket
# Create a socket
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# Connect to the server
client_socket.connect(("10.9.216.247", 12345)) # Replace with the server's IP
# Send and receive data
client_socket.send("Hello from the client!".encode())
data = client_socket.recv(1024).decode()
print(f"Server says: {data}")
# Close the connection
client_socket.close()