import socket
import sys
from enum import Enum
MAXMESSAGES = 100000
class Type(Enum):
    NOTRECV = 0
    INORDER = -1
    OUTOFORDER = -2

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

num_seqs = [Type.NOTRECV]*MAXMESSAGES
expected_seq = 0
server_socket.settimeout(1)
while(True != False):
    try:
        bytesAddressPair = server_socket.recvfrom(bufferSize)
        message = bytesAddressPair[0]
        recv_seq = int(message.decode('utf-8'))
        if(expected_seq != recv_seq):
            if(recv_seq < expected_seq):
                num_seqs[recv_seq] = Type.OUTOFORDER
            else:
                num_seqs[recv_seq] = recv_seq - expected_seq
                expected_seq = recv_seq + 1
        else:
            num_seqs[recv_seq] = Type.INORDER
            expected_seq += 1

    except socket.timeout:
        print("timeout")
        break

ooo = 0
lost = 0
ok = 0

for i in range(expected_seq):
    if(num_seqs[i] != Type.INORDER):
        if(num_seqs[i] == Type.OUTOFORDER):
            ooo += 1
        elif(num_seqs[i] == Type.NOTRECV):
            lost += 1
        else:
            ooo += 1
    else:
        ok += 1

print("Total: %d" % (expected_seq))
print("OK: %d" % (ok))
print("Out of order: %d" % (ooo))
print("Lost: %d" % (lost))
