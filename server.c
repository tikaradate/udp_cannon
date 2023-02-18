/* 
	Servidor UDP com análise de pacotes extraviados
	Disciplina: Redes II
	Autores:
		Jorge Lucas Vicilli Jabczenski
		Vinicius Tikara Venturi Date
	Última Atualização: 18/02/2023
*/

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h> 
#include <stdlib.h>
#include <string.h>
#include <poll.h>

#define CLIENT            1 /* Quantidade de Clientes         */
#define TAMFILA           5 /* Tamanho da fila                */
#define MAXHOSTNAME      30 /* Tamanho máximo do nome do host */
#define MAXMESSAGES 1000000 /* Número máximo de mensagens     */
#define NOTRECV			  0 /* Mensagem não chegou ainda      */
#define INORDER			 -1 /* Mensagem chegou em ordem       */
#define OUTOFORDER       -2 /* Mensagem chegou fora de ordem  */
#define TIMEOUT		   1000 /* Timeout do poll em ms          */

int main (int argc, char *argv[]){
	int expected_seq;						/* Número de sequência esperado                      */
	int recv_seq;							/* Número de sequência que recebeu                   */
	int soquete;							/* Número do soquete                                 */
	unsigned int tam_isa;					/* Tamanho do internet socket address                */
	struct sockaddr_in sa, isa;  			/* sa: servidor, isa: cliente                        */
	struct hostent *hp;						/* host                                              */
	int vetor_int[BUFSIZ];					/* Onde a mensagem recebida é guardada               */
	int num_seqs[MAXMESSAGES] = {};			/* Guarda informação sobre o número de sequência     */
	char localhost [MAXHOSTNAME];			/* Nome do host local                                */
	struct pollfd pollfds[CLIENT +1];	    /* Poll para o recebimento de mensagens com timeout  */

	/* Confere se foi passado o número certo de argumentos */
	if (argc != 2) {
		puts("Uso correto: ./servidor <porta>");
		exit(1);
	}

	gethostname(localhost, MAXHOSTNAME);

	if ((hp = gethostbyname(localhost)) == NULL){
		puts ("Nao consegui meu proprio IP");
		exit (1);
	}	
	
	sa.sin_port = htons(atoi(argv[1]));

	bcopy((char *) hp->h_addr, (char *) &sa.sin_addr, hp->h_length);

	sa.sin_family = hp->h_addrtype;		

	if ((soquete = socket(hp->h_addrtype,SOCK_DGRAM,0)) < 0){
        puts ( "Nao consegui abrir o soquete" );
		exit ( 1 );
	}	

	if (bind(soquete, (struct sockaddr *) &sa,sizeof(sa)) < 0){
		puts ( "Nao consegui fazer o bind" );
		exit ( 1 );
	}		

	// no pollfds 0 temos as informações do servidor:
	// qual o socket e qual o tipo de evento a monitorar
	pollfds[0].fd = soquete;
	pollfds[0].events = POLLIN;
	pollfds[0].revents = 0;
	expected_seq = 0;

	// checando se a matemática ainda funciona do jeito esperado
	while (1-1+1-1+1-1+1) {
		// esperando por mensagens com timeout predefinido
		int poll_res = poll(pollfds, CLIENT, TIMEOUT);
		// recebeu um evento
		if(poll_res > 0){
			// se for evento do tipo esperado
			if(pollfds[0].revents & POLLIN){
				tam_isa = sizeof(isa); 
				// recebe a mensagem
				recvfrom(soquete, vetor_int, BUFSIZ, 0, (struct sockaddr *) &isa, &tam_isa);
				// vê o número de sequência da mensagem
				recv_seq = vetor_int[0];
				// se o número for diferente do esperado
				if(expected_seq != recv_seq){
					// se for menor, então o pacote veio na ordem errada
					if(recv_seq < expected_seq){
						num_seqs[recv_seq] = OUTOFORDER;
					// se for maior, perdemos pacotes, guardamos a quantidade e
					// atualizamos o número de sequência esperado
					} else {
						num_seqs[recv_seq] = recv_seq - expected_seq;
						expected_seq = recv_seq + 1;
					}
				// senão, ocorreu tudo de forma correta, aumentamos em um o número esperado
				} else {
					num_seqs[recv_seq] = INORDER;
					expected_seq++;
				}
			}
		// ocorreu o timeout (poll_res <= 0)
		} else {
			break;
		}
	}

	// análise dos números de sequência
	int qt_lost = 0, qt_ooo = 0, qt_ok = 0;
	printf("--- Log das mensagens recebidas ---\n");
	for(int i = 0; i < expected_seq; i++){
		printf("Pacote #%d ", i);
        if(num_seqs[i] != INORDER){
            if (num_seqs[i] == OUTOFORDER){
				qt_ooo++;
                printf("<1> Fora de ordem atrasado\n");
            } else if (num_seqs[i] == NOTRECV){
				qt_lost++;
				printf("<2> Perdido\n");
			} else {
				qt_ooo++;
                printf("<3> Fora de ordem adiantado: pulou [%d] pacotes\n", num_seqs[i]);
            }
        } else { 
			qt_ok++;
	    	printf("<0> OK\n");
	    }
    }

	double pct_ooo = qt_ooo/(double)expected_seq * 100.0;
	double pct_lost = qt_lost/(double)expected_seq * 100.0;
	double pct_ok = qt_ok/(double)expected_seq * 100.0;

	printf("\nDos %d pacotes recebidos:",expected_seq);
	printf("\n  Fora de ordem: %5d (%.2lf%%)", qt_ooo, pct_ooo);
	printf("\n  Perdidos:      %5d (%.2lf%%)", qt_lost, pct_lost);
	printf("\n  OK:            %5d (%.2lf%%)\n", qt_ok, pct_ok);

	return 0;
}
