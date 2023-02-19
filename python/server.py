import socket
import sys
if len(sys.argv) != 2:
    print("Uso: python server.py <porta>")
    exit(1)

try:
    host_name = socket.gethostname()
except socket.gaierror:
    print("Não foi possível obter nome da máquina")
    exit(1)

try:    
    host = socket.gethostbyname(host_name)
except socket.gaierror:
    print("Não foi possível obter IP da máquina")
    exit(1)

port = int(sys.argv[1])
bufferSize  = 1024

# Create a datagram socket
server_socket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)

server_socket.bind((host, port))

server_socket.settimeout(1)
while(True):
    try:
        bytesAddressPair = server_socket.recvfrom(bufferSize)
        message = bytesAddressPair[0]
        print(message)
    except socket.timeout:
        print("timeout")
        break