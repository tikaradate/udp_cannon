# 
#	Cliente UDP em Python que manda mensagens com seu número de sequência
#	Disciplina: CI1061-Redes II
#	Autores:
#		Jorge Lucas Vicilli Jabczenski
#		Vinicius Tikara Venturi Date
#	Última Atualização: 18/02/2023
#
import socket
import sys

if len(sys.argv) != 4:
    print("Uso: python client.py <host> <porta> <nr_messagens>")
    exit(1)
    
try:
    host = socket.gethostbyname(sys.argv[1])
except socket.gaierror:
    print("Não foi possível obter endereço IP do servidor")
    exit(1)

port = int(sys.argv[2])
nr_msgs = int(sys.argv[3])

address = (host, port)
bufferSize = 1024

client_socket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
for i in range(nr_msgs):
    client_socket.sendto(bytearray(str(i), 'utf-8'), address)