/* cliente.c
   Jorge Tikara Vicilli Date
   Vinicius Lucas Venturi Jabczenski
   última att: 06-02-2023
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
	unsigned int nr_mensagens;

	// if(argc != 4) {
	// 	puts("Uso correto: ./cliente <nome-servidor> <porta> <dados>");
	// 	exit(1);
	// }

	if(argc != 4) {
		puts("Uso correto: ./cliente <nome-servidor> <porta> <nr_mensagens>");
		exit(1);
	}

	host = argv[1];
	nr_mensagens = atoi(argv[3]);

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

	for (i  = 0; i < nr_mensagens; ++i){
		vetor_de_uma_posicao[0] = i;
		if(sendto(sockdescr, vetor_de_uma_posicao, sizeof(vetor_de_uma_posicao), 0, (struct
			sockaddr *) &sa, sizeof sa) != sizeof(vetor_de_uma_posicao)){
			puts("Nao consegui mandar os dados"); 
			exit(1);
		}
	}
	/* end while }*/

	vetor_de_uma_posicao[0] = -1;
	if(sendto(sockdescr, vetor_de_uma_posicao, sizeof(vetor_de_uma_posicao), 0, (struct
		sockaddr *) &sa, sizeof sa) != sizeof(vetor_de_uma_posicao)){
		puts("Nao consegui mandar os dados"); 
		exit(1);
	}

	// "deixa comentado, a gente ve o que faz depois" - tikara
	// recvfrom(sockdescr, buf, BUFSIZ, 0, (struct sockaddr *) &sa, &i);

	// printf("Sou o cliente, recebi: %s\n", buf);

	close(sockdescr);
	
	return(0);
}
