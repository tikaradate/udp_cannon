/* 
	Cliente UDP que manda mensagens com seu número de sequência
	Disciplina: CI1061-Redes II
	Autores:
		Jorge Lucas Vicilli Jabczenski
		Vinicius Tikara Venturi Date
	Última Atualização: 18/02/2023
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h> 
#include <stdlib.h>
#include <string.h>

int 
main(int argc, char *argv[])
{  
	int nr_porta;		   // Porta a ser usada pelo socket
	int sockdescr;         // Descritor do socket
	struct sockaddr_in sa; // Endereço do socket
	struct hostent *hp;    // Guarda informações sobre o host
	char *host;            // Nome do host

	unsigned int nr_mensagens;

	// Confere o número de argumentos da linha de comando
	if(argc != 4) {
		puts("Uso correto: ./cliente <nome-servidor> <porta> <nr_mensagens>");
		exit(1);
	}

	host = argv[1];               // Recupera o nome do host
	nr_mensagens = atoi(argv[3]); // Recupera a quantidade de mensagens que vão ser enviadas

	printf("Obtendo o IP da máquina do host...\n");	
	if((hp = gethostbyname(host)) == NULL){
		puts("Nao consegui obter endereco IP do servidor.");
		exit(1);
	}
	printf("Obtido com sucesso!\n");	
	
	bcopy((char *)hp->h_addr, (char *)&sa.sin_addr, hp->h_length);
	sa.sin_family = hp->h_addrtype;

	nr_porta = atoi(argv[2]);
	sa.sin_port = htons(nr_porta);
	printf("Abrindo socket na porta %d\n", nr_porta);
	if((sockdescr = socket(hp->h_addrtype, SOCK_DGRAM, 0)) < 0) {
		puts("Nao consegui abrir o socket.");
		exit(1);
	}
	printf("Socket aberto com sucesso!\n");	

	// vetor que guarda o número de sequência da mensagem atual
	int vetor_de_uma_posicao[1];
	printf("Começando o envio de %d mensagens.", nr_mensagens);
	for (int i = 0; i < nr_mensagens; ++i){
		vetor_de_uma_posicao[0] = i;
		if(sendto(sockdescr, vetor_de_uma_posicao, sizeof(vetor_de_uma_posicao), 0, (struct
			sockaddr *) &sa, sizeof sa) != sizeof(vetor_de_uma_posicao)){
			puts("Nao consegui mandar os dados"); 
			exit(1);
		}
	}
	printf("Enviei todas as mensagens, irei fechar o socket\n");

	close(sockdescr);
	
	return(0);
}
