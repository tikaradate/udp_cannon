/* 
	Cliente UDP que manda mensagens com seu número de sequência
	Disciplina: Redes II
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
	int sockdescr;

	struct sockaddr_in sa;
	struct hostent *hp;
	char *host;

	unsigned int i;

	if(argc != 3) {
		puts("Uso correto: ./cliente <nome-servidor> <porta>");
		exit(1);
	}

	host = argv[1];

	if((hp = gethostbyname(host)) == NULL){
		puts("Nao consegui obter endereco IP do servidor.");
		exit(1);
	}

	bcopy((char *)hp->h_addr, (char *)&sa.sin_addr, hp->h_length);
	sa.sin_family = hp->h_addrtype;

	sa.sin_port = htons(atoi(argv[2]));

	if((sockdescr=socket(hp->h_addrtype, SOCK_DGRAM, 0)) < 0) {
		puts("Nao consegui abrir o socket.");
		exit(1);
	}

	int vetor_de_uma_posicao[1];
    // int seqs[10] = {0, 1, 2, 5, 4, 3, 6, 8, 10, 9};
    int seqs[3] = {0, 2, 3};
    // enviar 3 mensagens em ordem
	for (i = 0; i < 3; ++i){
		vetor_de_uma_posicao[0] = seqs[i];
		if(sendto(sockdescr, vetor_de_uma_posicao, sizeof(vetor_de_uma_posicao), 0, (struct
			sockaddr *) &sa, sizeof sa) != sizeof(vetor_de_uma_posicao)){
			puts("Nao consegui mandar os dados"); 
			exit(1);
		}
	}

	close(sockdescr);
	
	return(0);
}
