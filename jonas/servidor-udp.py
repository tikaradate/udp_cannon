import sys
import socket
import pickle

BUFFERSIZE = 1024

#main do servidor
if __name__ == "__main__":
	if len(sys.argv) != 2:
		print(f"Erro:	Número errado de argumentos. Tente:\npython servidor-udp.py <porta>")
		exit(1)

	#pega a porta dos parametros da chamada
	porta		=  int(sys.argv[1])

	#pega o nome do server
	hostname	= socket.gethostname()
	print(f"Servidor ligado :)\nOlá\nMeu nome é {hostname}\nBom te ver, usuário\n\nAguardando mensagens...\n");

	#consegue o endreço IP do server
	hostIP = socket.gethostbyname(hostname)
	if (hostIP  == 0):
		print(f"Erro:	Não foi possível encontrar o endereço IP do servidor")
		exit (1)

	#pega o socket e dá bind
	sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	sock.bind((hostIP, porta))

	while (True):
		#pega a primeira mensagem que possui o número de mensagens
		sock.settimeout(None)
		mensagem_bytes, addr	= sock.recvfrom(BUFFERSIZE)
		n_msg					= pickle.loads(mensagem_bytes)

		#inicia variaveis usadas no recebimento de mensagens
		num_anterior	= -1
		chegou			= [0] * n_msg
		desord			= list()

		#coloca o timeout como 1 segundo
		sock.settimeout(1)

		#recebe as mensagens até dar timeout
		while (True):
			try:
				mensagem_bytes, addr	= sock.recvfrom(BUFFERSIZE)
				num_recebido			= pickle.loads(mensagem_bytes)

				#detecta se a mensagem está fora de ordem
				if (num_recebido < num_anterior):
					desord.append(num_recebido)

				#registra que a mensagem chegou
				chegou[num_recebido]	= 1

				num_anterior			= num_recebido

			except socket.timeout:
				break

		#a partir daqui, no vetor "chegou", os indices com 0 não chegaram
		#e o vetor desord guarda quem não chegou depois do numero anterior

		#cria as strings que darao nome aos arquivos de logs
		nom_arq_nchegou = "py-nchegou-"
		nom_arq_desorde = "py-desorde-"
		nom_arq_nchegou = nom_arq_nchegou + str(n_msg) 
		nom_arq_desorde = nom_arq_desorde + str(n_msg)
		nom_arq_nchegou = nom_arq_nchegou + ".txt" 
		nom_arq_desorde = nom_arq_desorde + ".txt"

		#cria os arquivos
		f_nchegou = open(nom_arq_nchegou,"w")
		f_desorde = open(nom_arq_desorde,"w")

		print(f"O recebimento de {n_msg} mensagens acabou, esperando mais...")

		#escreveremos nos arquivos de log
		for i in range(0, n_msg):
			if chegou[num_recebido] == 0:
				f_nchegou.write(f"A mensagem de número {i} não chegou\n")

		for i in range(0, len(desord)):
			f_desorde.write(f"A mensagem de número {i} chegou fora de ordem\n")

		#fecha os arquivos
		f_nchegou.close()
		f_desorde.close()

	exit(1)

