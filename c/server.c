/* 
	Servidor UDP com análise de pacotes extraviados
	Disciplina: CI1061-Redes II
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
#include <sys/stat.h>
#include "relatorios.h"
#include "consts.h"

#define CLIENT            1  /* Quantidade de Clientes         */
#define TAMFILA           5  /* Tamanho da fila                */
#define MAXHOSTNAME      30  /* Tamanho máximo do nome do host */
#define NOTRECV			  0  /* Mensagem não chegou ainda      */
#define INORDER			 -1  /* Mensagem chegou em ordem       */
#define OUTOFORDER       -2  /* Mensagem chegou fora de ordem  */
#define TIMEOUT		   1000  /* Timeout do poll em ms          */
#define TRUE 			  1
#define FALSE         	  0

relatorio_t analisar_dados(int expected_seq, int *num_seqs);

int main (int argc, char *argv[]){
	int expected_seq;                       /* Número de sequência esperado                      */
	int recv_seq;                           /* Número de sequência que recebeu                   */
	int soquete;                            /* Número do soquete                                 */
	unsigned int tam_isa;                   /* Tamanho do internet socket address                */
	struct sockaddr_in sa, isa;             /* sa: servidor, isa: cliente                        */
	struct hostent *hp;                     /* host                                              */
	int vetor_int[BUFSIZ];                  /* Onde a mensagem recebida é guardada               */
	int *num_seqs; 							/* Guarda informação sobre o número de sequência     */
	char localhost [MAXHOSTNAME];           /* Nome do host local                                */
	struct pollfd pollfds[CLIENT +1];       /* Poll para o recebimento de mensagens com timeout  */
	int nr_porta;                           /* Número da porta                                   */
	int nr_relatorio = 0;                   /* Número do relatório gerado                        */

	/* Confere se foi passado o número certo de argumentos */
	if (argc != 2) {
		puts("Uso correto: ./servidor <porta>");
		exit(1);
	}
	printf("Obtendo o endereco IP do host...\n");
	gethostname(localhost, MAXHOSTNAME);
	if ((hp = gethostbyname(localhost)) == NULL){
		puts ("Nao consegui meu proprio IP");
		exit (1);
	}
	printf("Endereco IP obtido com sucesso!\n");
	
	nr_porta = atoi(argv[1]);
	sa.sin_port = htons(nr_porta);

	bcopy((char *) hp->h_addr, (char *) &sa.sin_addr, hp->h_length);

	sa.sin_family = hp->h_addrtype;		
	printf("Abrindo socket na porta %d\n", nr_porta);
	if ((soquete = socket(hp->h_addrtype, SOCK_DGRAM,0)) < 0){
        puts ("Nao consegui abrir o soquete");
		exit (1);
	}
	printf("Socket aberto com sucesso!\n");	

	printf("Realizando o bind...\n");	
	if (bind(soquete, (struct sockaddr *) &sa,sizeof(sa)) < 0){
		puts ("Nao consegui fazer o bind");
		exit (1);
	}
	printf("Bind realizado com sucesso!\n");

	printf("Criando diretorios para armazenar os relatorios...\n");
	// Criando diretório "dados" para armazenar os relatórios
	struct stat st = {0};
	if(stat("../dados", &st) == -1){
		mkdir("../dados", S_IRWXU);
	}
	if(stat("../dados/c", &st) == -1){
		mkdir("../dados/c", S_IRWXU);
	}
	printf("Diretorios criados com sucesso!\n");

	num_seqs = calloc(MAXMESSAGES, sizeof(int));
	// no pollfds 0 temos as informações do servidor:
	// qual o socket e qual o tipo de evento a monitorar
	pollfds[0].fd = soquete;
	pollfds[0].events = POLLIN;
	pollfds[0].revents = 0;
	expected_seq = 0;
	while (TRUE) {
		// esperando por mensagens com timeout predefinido
		int poll_res = poll(pollfds, CLIENT, TIMEOUT);
		// recebeu um evento
		if(poll_res > 0){
			// se for evento do tipo esperado
			if(pollfds[0].revents & POLLIN){
				tam_isa = sizeof(isa); 
				recvfrom(soquete, vetor_int, BUFSIZ, 0, (struct sockaddr *) &isa, &tam_isa);
				// vê o número de sequência da mensagem
				recv_seq = vetor_int[0];
				if(expected_seq != recv_seq){
					if(recv_seq < expected_seq){
						num_seqs[recv_seq] = OUTOFORDER;
					// se for maior, perdemos pacotes, guardamos a quantidade e
					// atualizamos o número de sequência esperado
					} else {
						num_seqs[recv_seq] = recv_seq - expected_seq;
						expected_seq = recv_seq + 1;
					}
				} else {
					num_seqs[recv_seq] = INORDER;
					expected_seq++;
				}
			}
		// ocorreu o timeout (poll_res <= 0)
		} else {
			printf(". "); // sinaliza o timeout
			fflush(stdout);
			relatorio_t infos = analisar_dados(expected_seq, num_seqs);
			// se dados foram recebidos, criamos os relatórios
			if (infos.qtd_recebidos != 0){
				printf("Recebi até %d pacotes!\n", expected_seq);
				printf("Gerando relatorios...\n");
				char human_report_filename[64], machine_report_filename[64];
				sprintf(human_report_filename,   "../dados/c/human_report_%d_%d.txt"   , nr_porta, nr_relatorio);
				printf("Gerando relatorio humano...\n");
				gerar_relatorio_humano(infos, human_report_filename);
				printf("Relatorio humano gerado com sucesso!\n");
				sprintf(machine_report_filename, "../dados/c/machine_report_%d_%d.json", nr_porta, nr_relatorio);
				printf("Gerando relatorio em JSON...\n");
				gerar_relatorio_json(infos, machine_report_filename);
				printf("Relatorio em JSON gerado com sucesso!\n");
				nr_relatorio++;
				expected_seq = 0;
                memset(num_seqs, 0, MAXMESSAGES*sizeof(int));
				printf("Voltando a esperar mensagem...\n");
			}
		}
	}

	return 0;
}


relatorio_t
analisar_dados(int expected_seq, int *num_seqs)
{
	relatorio_t infos;

	infos.qtd_ok        = 0;
	infos.qtd_ooo       = 0;
	infos.qtd_lost      = 0;
	infos.qtd_recebidos = expected_seq;
	for(int i = 0; i < expected_seq; ++i) infos.num_seqs[i] = num_seqs[i];

	// percorre o vetor de mensagens recebidas, classificando-as
	for(int i = 0; i < expected_seq; i++){
        if(num_seqs[i] != INORDER){
            if (num_seqs[i] == OUTOFORDER){
				infos.qtd_ooo++;
            } else if (num_seqs[i] == NOTRECV){
				infos.qtd_lost++;
			} else {
				infos.qtd_ooo++;
            }
        } else { 
			infos.qtd_ok++;
	    }
    }
	return infos;
}
