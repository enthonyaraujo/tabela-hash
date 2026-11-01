/* Tipos e operações da configuração 40. */
#ifndef TABELA_H
#define TABELA_H

#include <stddef.h>
#define TAM_PALAVRA 256

/* REMOVIDO mantém o caminho de busca depois de uma exclusão. */
typedef enum { VAZIO, OCUPADO, REMOVIDO } Estado;
typedef enum { RES_OK, RES_DUPLICADA, RES_AUSENTE, RES_CHEIA } Resultado;

typedef struct {
    char chave[TAM_PALAVRA];
    int frequencia;
    int *linhas; /* Endereço do vetor de linhas distintas da palavra. */
    size_t quantidade_linhas;
} Palavra;

typedef struct {
    Estado estado;
    Palavra palavra;
} Posicao;

typedef struct {
    Posicao *posicoes; /* Endereço do vetor de posições. */
    size_t capacidade; /* m: total fixo de posições. */
    size_t quantidade; /* n: palavras distintas armazenadas. */
    size_t colisoes;   /* Histórico das colisões nas inserções aceitas. */
} Tabela;

/* As chaves recebidas devem estar normalizadas e ter até 255 bytes. */
size_t hash_indice(const char *palavra, size_t capacidade);
int tabela_criar(Tabela *tabela, size_t capacidade);
void tabela_destruir(Tabela *tabela);
Resultado tabela_inserir(Tabela *tabela, const char *chave, int frequencia);
Palavra *tabela_buscar(Tabela *tabela, const char *chave);
Resultado tabela_atualizar(Tabela *tabela, const char *chave, int frequencia);
Resultado tabela_remover(Tabela *tabela, const char *chave);
void tabela_adicionar_linha(Palavra *palavra, int linha);
void tabela_listar(const Tabela *tabela);
void tabela_exibir_estrutura(const Tabela *tabela);
const char *resultado_texto(Resultado resultado);
#endif
