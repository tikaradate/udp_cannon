import sys
import pickle
import socket

#main do cliente
if __name__ == "__main__":
	#verifica o numero de argumentos
	if len(sys.argv) != 4:
		print(f"Erro:	Número errado de argumentos. Tente:\npython cliente-udp.py <nome-servidor> <porta> <num_msg>")
		exit(1)

	#pega o nome do server, a porta e o número de mensagens que serao enviadas
	hostname	= sys.argv[1]
	porta		= int(sys.argv[2])
	num_msg		= int(sys.argv[3])

	#pega o endereço IP do servidor baseado no seu nome
	hostIP = socket.gethostbyname(hostname)
	if (hostIP  == 0):
		print(f"Erro:	Não foi possível encontrar o endereço IP do servidor")
		exit (1)
	
	#abre o socket
	sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

	#manda o numero de mensagens
	mensagem_bytes = pickle.dumps(num_msg)
	sock.sendto(mensagem_bytes, (hostIP, porta))

	#manda num_msg mensagens
	for i in range(0, num_msg):
		mensagem_bytes = pickle.dumps(i)
		sock.sendto(mensagem_bytes, (hostIP, porta))

	exit(1)