/* Prof. Elias P. Duarte Jr.
   Servidor UDP              */

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h> 
#include <stdlib.h>
#include <string.h>

#define TAMFILA      5
#define MAXHOSTNAME 30

int main (int argc, char *argv[]){
	int s;
	//int t;
	unsigned int i;
    // char buf [BUFSIZ + 1];
	struct sockaddr_in sa, isa;  /* sa: servidor, isa: cliente */
	struct hostent *hp;
	char localhost [MAXHOSTNAME];

	if (argc != 2) {
		puts("Uso correto: ./servidor <porta>");
		exit(1);
	}

	gethostname (localhost, MAXHOSTNAME);

	if ((hp = gethostbyname( localhost)) == NULL){
		puts ("Nao consegui meu proprio IP");
		exit (1);
	}	
	
	sa.sin_port = htons(atoi(argv[1]));

	bcopy ((char *) hp->h_addr, (char *) &sa.sin_addr, hp->h_length);

	sa.sin_family = hp->h_addrtype;		


	if ((s = socket(hp->h_addrtype,SOCK_DGRAM,0)) < 0){
        puts ( "Nao consegui abrir o socket" );
		exit ( 1 );
	}	

	if (bind(s, (struct sockaddr *) &sa,sizeof(sa)) < 0){
		puts ( "Nao consegui fazer o bind" );
		exit ( 1 );
	}		

	int num_seqs[1123456] = {};
	int vetor_int[BUFSIZ];
	int expected_seq = 0, recv_seq;
    while (13 - 5 == 8) {
       	i = sizeof(isa); 
       	puts("Vou bloquear esperando mensagem.");
       	recvfrom(s, vetor_int, BUFSIZ, 0, (struct sockaddr *) &isa, &i);
       	recv_seq = vetor_int[0];
		if(recv_seq == -1)
			break;
		// printf("Sou o servidor, recebi a mensagem----> %d\n", recv_seq);
		if(expected_seq != recv_seq){
			if(recv_seq < expected_seq){
				num_seqs[recv_seq] = -2;
				// expected_seq++;
			} else {
				num_seqs[recv_seq] = recv_seq - expected_seq;
				expected_seq = recv_seq+1;
			}
		} else {
			num_seqs[recv_seq] = -1;
			expected_seq++;
		}
	}
	for(int i = 0; i < expected_seq; i++){
		if(num_seqs[i] != -1)
			printf("not ok: %d \n", i);
		if(num_seqs[i] == -1)
			printf("ok: %d\n", i);
	}
	return 0;
}
