#ifndef __RELATORIOS__
#define __RELATORIOS__

#include <stdio.h>
#include <stdlib.h>
#include "consts.h"

typedef struct relatorio_s {
    int qtd_recebidos;         // Quantidade total de pacotes recebidos
    int qtd_ok;                // Quantidade de pacotes ok
    int qtd_ooo;               // Quantidade de pacotes fora de ordem
    int qtd_lost;              // Quantidade de pacotes perdidos
    int num_seqs[MAXMESSAGES]; // Número máximo de mensagens
    char nome_arquivo[64];     // Nome do arquivo que vai ser criado
} relatorio_t;


/* Gera um relatório amigável para olhos humanos */
void gerar_relatorio_humano(relatorio_t infos, char filename[]);

/* Gera um relatório para as máquinas sem coração */
void gerar_relatorio_json(relatorio_t infos, char filename[]);

void imprime_relat(relatorio_t r);

#endif