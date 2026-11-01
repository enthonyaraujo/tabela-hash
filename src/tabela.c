#include "tabela.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Converte os bytes da palavra em inteiro usando base 131. */
static uint64_t chave_para_inteiro(const char *palavra)
{
    uint64_t numero = 0;

    for (size_t indice = 0; palavra[indice] != '\0'; indice++) {
        unsigned char byte = (unsigned char)palavra[indice];
        numero = numero * 131 + byte;
    }

    return numero; /* Inteiro sem sinal: aritmética módulo 2^64. */
}

/* H4: soma grupos de quatro dígitos e calcula o resto pela capacidade. */
size_t hash_indice(const char *palavra, size_t capacidade)
{
    uint64_t numero = chave_para_inteiro(palavra);
    uint64_t soma = 0;

    while (numero > 0) {
        uint64_t grupo = numero % 10000;
        soma = soma + grupo;
        numero = numero / 10000;
    }

    return (size_t)(soma % capacidade);
}

/* Reserva o vetor uma vez. Todos os estados começam como VAZIO. */
int tabela_criar(Tabela *tabela, size_t capacidade)
{
    *tabela = (Tabela){0}; /* Zera os campos antes de reservar memória. */

    if (capacidade == 0) {
        return 0;
    }

    tabela->posicoes = calloc(capacidade, sizeof(Posicao));
    if (tabela->posicoes == NULL) {
        return 0;
    }

    tabela->capacidade = capacidade;
    return 1;
}

/* Libera as linhas de cada palavra e depois o vetor da tabela. */
void tabela_destruir(Tabela *tabela)
{
    for (size_t indice = 0; indice < tabela->capacidade; indice++) {
        free(tabela->posicoes[indice].palavra.linhas);
    }

    free(tabela->posicoes);
    *tabela = (Tabela){0};
}

/* Retorna a posição da chave, ou capacidade se a chave não existir. */
static size_t localizar(Tabela *tabela, const char *chave)
{
    size_t indice_inicial = hash_indice(chave, tabela->capacidade);

    for (size_t tentativa = 0; tentativa < tabela->capacidade; tentativa++) {
        size_t indice = (indice_inicial + tentativa) % tabela->capacidade;
        Posicao *posicao = &tabela->posicoes[indice];

        if (posicao->estado == VAZIO) {
            break;
        }

        /* Em REMOVIDO, seguimos procurando. Só comparamos posições ocupadas. */
        if (posicao->estado == OCUPADO) {
            if (strcmp(posicao->palavra.chave, chave) == 0) {
                return indice;
            }
        }
    }

    return tabela->capacidade;
}

/* Rejeita duplicata e procura uma posição vazia ou removida. */
Resultado tabela_inserir(Tabela *tabela, const char *chave, int frequencia)
{
    size_t indice_existente = localizar(tabela, chave);
    if (indice_existente != tabela->capacidade) {
        return RES_DUPLICADA;
    }

    size_t indice_inicial = hash_indice(chave, tabela->capacidade);
    for (size_t tentativa = 0; tentativa < tabela->capacidade; tentativa++) {
        size_t indice = (indice_inicial + tentativa) % tabela->capacidade;
        Posicao *posicao = &tabela->posicoes[indice];

        if (posicao->estado == OCUPADO) {
            continue;
        }

        /* Conta uma colisão se a posição inicial já tinha outra palavra. */
        if (tabela->posicoes[indice_inicial].estado == OCUPADO) {
            tabela->colisoes++;
        }

        posicao->palavra = (Palavra){0};
        strcpy(posicao->palavra.chave, chave);
        posicao->palavra.frequencia = frequencia;
        posicao->estado = OCUPADO;
        tabela->quantidade++;
        return RES_OK;
    }

    return RES_CHEIA;
}

/* Devolve o registro interno, ou NULL quando a palavra está ausente. */
Palavra *tabela_buscar(Tabela *tabela, const char *chave)
{
    size_t indice = localizar(tabela, chave);

    if (indice == tabela->capacidade) {
        return NULL;
    }

    return &tabela->posicoes[indice].palavra;
}

/* Altera apenas a frequência. A chave e as linhas continuam iguais. */
Resultado tabela_atualizar(Tabela *tabela, const char *chave, int frequencia)
{
    size_t indice = localizar(tabela, chave);

    if (indice == tabela->capacidade) {
        return RES_AUSENTE;
    }

    tabela->posicoes[indice].palavra.frequencia = frequencia;
    return RES_OK;
}

/* REMOVIDO mantém acessíveis as palavras deslocadas pela colisão. */
Resultado tabela_remover(Tabela *tabela, const char *chave)
{
    size_t indice = localizar(tabela, chave);

    if (indice == tabela->capacidade) {
        return RES_AUSENTE;
    }

    free(tabela->posicoes[indice].palavra.linhas);
    tabela->posicoes[indice].palavra = (Palavra){0};
    tabela->posicoes[indice].estado = REMOVIDO;
    tabela->quantidade--;
    return RES_OK;
}

/* Durante a leitura sequencial, registra cada linha uma única vez. */
void tabela_adicionar_linha(Palavra *palavra, int linha)
{
    if (palavra->quantidade_linhas > 0) {
        size_t ultima_posicao = palavra->quantidade_linhas - 1;
        if (palavra->linhas[ultima_posicao] == linha) {
            return;
        }
    }

    /* Só este vetor cresce. A capacidade da tabela hash permanece fixa. */
    size_t nova_quantidade = palavra->quantidade_linhas + 1;
    int *novas_linhas = realloc(palavra->linhas, nova_quantidade * sizeof(int));
    if (novas_linhas == NULL) {
        fputs("Memória insuficiente.\n", stderr);
        exit(EXIT_FAILURE);
    }

    palavra->linhas = novas_linhas;
    palavra->linhas[palavra->quantidade_linhas] = linha;
    palavra->quantidade_linhas++;
}

/* Exibe frequência e linhas de uma palavra. */
static void imprimir(const Palavra *palavra)
{
    printf("%s: frequência=%d; linhas=[", palavra->chave, palavra->frequencia);

    for (size_t indice = 0; indice < palavra->quantidade_linhas; indice++) {
        if (indice > 0) {
            printf(", ");
        }
        printf("%d", palavra->linhas[indice]);
    }

    puts("]");
}

/* Lista apenas as palavras que ainda estão na tabela. */
void tabela_listar(const Tabela *tabela)
{
    printf("%zu palavras distintas\n", tabela->quantidade);

    for (size_t indice = 0; indice < tabela->capacidade; indice++) {
        const Posicao *posicao = &tabela->posicoes[indice];
        if (posicao->estado == OCUPADO) {
            imprimir(&posicao->palavra);
        }
    }
}

/* Mostra contadores e todas as posições, inclusive vazias e removidas. */
void tabela_exibir_estrutura(const Tabela *tabela)
{
    double fator_carga = (double)tabela->quantidade / tabela->capacidade;
    printf("m=%zu; n=%zu; alfa=%.3f; colisões=%zu\n",
           tabela->capacidade, tabela->quantidade, fator_carga, tabela->colisoes);

    for (size_t indice = 0; indice < tabela->capacidade; indice++) {
        const Posicao *posicao = &tabela->posicoes[indice];
        printf("[%zu] ", indice);

        if (posicao->estado == VAZIO) {
            puts("VAZIO");
        } else if (posicao->estado == REMOVIDO) {
            puts("REMOVIDO");
        } else {
            size_t indice_inicial = hash_indice(posicao->palavra.chave, tabela->capacidade);
            printf("h=%zu; ", indice_inicial);
            imprimir(&posicao->palavra);
        }
    }
}

/* Converte o resultado da operação em uma mensagem do menu. */
const char *resultado_texto(Resultado resultado)
{
    switch (resultado) {
        case RES_OK:
            return "OK";
        case RES_DUPLICADA:
            return "chave duplicada";
        case RES_AUSENTE:
            return "chave não encontrada";
        case RES_CHEIA:
            return "tabela cheia";
    }

    return "erro";
}
