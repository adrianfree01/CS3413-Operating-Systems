import socket
# Create a socket
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# Bind to the server's IP and port
server_socket.bind(("10.9.216.247", 12345)) # Replace with your server's IP
server_socket.listen(1)
print("Server is listening...")
# Accept a connection
conn, addr = server_socket.accept()
print(f"Connection from {addr}")
# Receive and send data
data = conn.recv(1024).decode()
print(f"Received: {data}")
conn.send("Message received!".encode())
# Close the connection
conn.close()