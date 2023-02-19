#include "relatorios.h"
void 
gerar_relatorio_humano(relatorio_t infos, char filename[])
{
    fflush(stdout);
    FILE *human_report;
    human_report   = fopen(filename  , "w");
	double pct_ooo  = infos.qtd_ooo /(double)infos.qtd_recebidos * 100.0;
	double pct_lost = infos.qtd_lost/(double)infos.qtd_recebidos * 100.0;
	double pct_ok   = infos.qtd_ok  /(double)infos.qtd_recebidos * 100.0;

    fprintf(human_report, "\nDos %d pacotes recebidos:",infos.qtd_recebidos);
	fprintf(human_report, "\n  OK:            %5d (%6.2lf%%)", infos.qtd_ok, pct_ok);
	fprintf(human_report, "\n  Fora de ordem: %5d (%6.2lf%%)", infos.qtd_ooo, pct_ooo);
	fprintf(human_report, "\n  Perdidos:      %5d (%6.2lf%%)\n", infos.qtd_lost, pct_lost);

    fprintf(human_report, "--- Log das mensagens recebidas ---\n");
	for(int i = 0; i < infos.qtd_recebidos; i++){
		fprintf(human_report, "Pacote #%d ", i);
        if(infos.num_seqs[i] != INORDER){
            if (infos.num_seqs[i] == OUTOFORDER){
                fprintf(human_report, "<1> Fora de ordem atrasado\n");
            } else if (infos.num_seqs[i] == NOTRECV){
				fprintf(human_report, "<2> Perdido\n");
			} else {
                fprintf(human_report, "<3> Fora de ordem adiantado: pulou [%d] pacotes\n", infos.num_seqs[i]);
            }
        } else { 
	    	fprintf(human_report, "<0> OK\n");
	    }
    }

    fclose(human_report);
}

void 
gerar_relatorio_json(relatorio_t infos, char filename[])
{
    FILE *machine_report;
    machine_report   = fopen(filename  , "w");

    fprintf(machine_report, "{\n");
    fprintf(machine_report, "\t\"total_pacotes\":%d,\n", infos.qtd_recebidos);
    fprintf(machine_report, "\t\"pacotes_ok\":%d,\n", infos.qtd_ok);
	fprintf(machine_report, "\t\"pacotes_fora_de_ordem\":%d,\n", infos.qtd_ooo);
	fprintf(machine_report, "\t\"pacotes_perdidos\":%d,\n", infos.qtd_lost);
	fprintf(machine_report, "\t\"pacotes\": [\n");

	for(int i = 0; i < infos.qtd_recebidos; i++){
		fprintf(machine_report, "\t\t{\n");
		fprintf(machine_report, "\t\t\t\"id\":%d,\n", i);
        if(infos.num_seqs[i] != INORDER){
            if (infos.num_seqs[i] == OUTOFORDER){
				fprintf(machine_report, "\t\t\t\"estado\": 1\n");
            } else if (infos.num_seqs[i] == NOTRECV){
				fprintf(machine_report, "\t\t\t\"estado\": 2\n");
			} else {
				fprintf(machine_report, "\t\t\t\"estado\": 3\n");
            }
        } else { 
			fprintf(machine_report, "\t\t\t\"estado\": 0\n");
	    }
		fprintf(machine_report, "\t\t}%c\n", i < infos.qtd_recebidos-1 ? ',' : ' ');
    }
	fprintf(machine_report, "\t]\n");
	fprintf(machine_report, "}");

    fclose(machine_report);
}