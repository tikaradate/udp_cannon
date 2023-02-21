import socket
import sys
import os
from enum import Enum

MAXMESSAGES = 1000000
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

if not os.path.isdir('../dados'):
    os.mkdir('../dados', 0o700)
if not os.path.isdir('../dados/python'):
    os.mkdir('../dados/python', 0o700)

port = int(sys.argv[1])
bufferSize  = 1024

# Create a datagram socket
server_socket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)

server_socket.bind((host, port))

num_seqs = [Type.NOTRECV]*MAXMESSAGES
expected_seq = 0
nr_relatorio = 0
server_socket.settimeout(2)
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
        received = expected_seq
        if(received == 0):
            continue
        ooo = 0
        lost = 0
        ok = 0
        
        human_content = ''
        json_content = ''
        for i in range(received):
            human_content += ("Pacote #%d " % (i))
            json_content +=  "\t\t{\n"
            json_content += ("\t\t\t\"id\":%d,\n" % (i))

            if(num_seqs[i] != Type.INORDER):
                if(num_seqs[i] == Type.OUTOFORDER):
                    human_content += "<1> Fora de ordem atrasado\n"
                    json_content += "\t\t\t\"estado\": 1\n"
                    ooo += 1
                elif(num_seqs[i] == Type.NOTRECV):
                    human_content += "<2> Perdido\n"
                    json_content += "\t\t\t\"estado\": 2\n"
                    lost += 1
                else:
                    human_content +=  ("<3> Fora de ordem adiantado: pulou [%d] pacotes\n" % (num_seqs[i]))
                    json_content += "\t\t\t\"estado\": 3\n"
                    ooo += 1
            else:
                human_content += "<0> OK\n"
                json_content += "\t\t\t\"estado\": 0\n"
                ok += 1

            json_content += ("\t\t}%c\n" % (',' if (i < received-1) else ' '))
	    
        json_content += "\t]\n"
        json_content += "}"

        human_header =  ('\nDos %d pacotes recebidos:' % (received))
        human_header += ('\n  OK:            %5d (%6.2lf%%)' % (ok, ok/received * 100.0))
        human_header += ('\n  Fora de ordem: %5d (%6.2lf%%)' % (ooo, ooo/received * 100.0))
        human_header += ('\n  Perdidos:      %5d (%6.2lf%%)\n' % (lost, lost/received * 100.0))
        
        json_header =   "{\n"
        json_header += ("\t\"total_pacotes\":%d,\n" % (received))
        json_header += ("\t\"pacotes_ok\":%d,\n" % (ok))
        json_header += ("\t\"pacotes_fora_de_ordem\":%d,\n" % (ooo))
        json_header += ("\t\"pacotes_perdidos\":%d,\n" % (lost))
        json_header +=  "\t\"pacotes\": [\n"

        human_path = "../dados/python/human_report_" + str(port) + "_" + str(nr_relatorio) + ".txt"
        json_path  = "../dados/python/machine_report_" + str(port) + "_" + str(nr_relatorio) + ".json"
       
        with open(human_path, "w") as f:
            f.write(human_header)
            f.write(human_content)
        
        with open(json_path, "w") as f:
            f.write(json_header)
            f.write(json_content)
        nr_relatorio += 1
        expected_seq = 0
        for i in range (expected_seq):
            num_seqs[i] = Type.NOTRECV
            
