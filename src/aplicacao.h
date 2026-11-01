/* Normalização e indexação de palavras (A4). */
#ifndef APLICACAO_H
#define APLICACAO_H
#include <stdio.h>
#include "tabela.h"
/* saida deve ter tamanho_saida > 0 bytes; devolve 1 se a chave for válida. */
int normalizar_palavra(const char *entrada, char *saida, size_t tamanho_saida);
/* Recebe arquivo aberto; devolve ocorrências aceitas e escreve as rejeitadas. */
size_t aplicacao_indexar_texto(Tabela *tabela, FILE *arquivo, size_t *rejeitadas);
#endif
