#include "aplicacao.h"
#include <ctype.h>

/* Converte o segundo byte dos acentos UTF-8 tratados para a letra sem acento. */
static char sem_acento(unsigned char segundo_byte)
{
    segundo_byte = segundo_byte | 0x20; /* Reúne as formas maiúscula e minúscula. */

    if (segundo_byte >= 0xA0 && segundo_byte <= 0xA5) {
        return 'a';
    }
    if (segundo_byte == 0xA7) {
        return 'c';
    }
    if (segundo_byte >= 0xA8 && segundo_byte <= 0xAB) {
        return 'e';
    }
    if (segundo_byte >= 0xAC && segundo_byte <= 0xAF) {
        return 'i';
    }
    if (segundo_byte == 0xB1) {
        return 'n';
    }
    if (segundo_byte >= 0xB2 && segundo_byte <= 0xB6) {
        return 'o';
    }
    if (segundo_byte >= 0xB9 && segundo_byte <= 0xBC) {
        return 'u';
    }

    return 0; /* Não há uma conversão prevista para esse caractere. */
}

/* Produz a chave normalizada; retorna 0 se ficar vazia ou não couber na saída. */
int normalizar_palavra(const char *entrada, char *saida, size_t tamanho_saida)
{
    size_t indice_entrada = 0;
    size_t quantidade_escrita = 0;

    while (entrada[indice_entrada] != '\0') {
        unsigned char byte = (unsigned char)entrada[indice_entrada];
        char letra = 0;

        if (byte == 0xC3 && entrada[indice_entrada + 1] != '\0') {
            /* Os acentos mapeados ocupam dois bytes na entrada. */
            letra = sem_acento((unsigned char)entrada[indice_entrada + 1]);
            indice_entrada = indice_entrada + 2;
        } else if (byte >= 0x80) {
            /* Descarta outros bytes não ASCII, conforme a regra do projeto. */
            indice_entrada++;
            continue;
        } else {
            if (isalnum(byte)) {
                letra = (char)tolower(byte);
            }
            indice_entrada++;
        }

        if (letra == 0) {
            continue;
        }
        if (quantidade_escrita + 1 >= tamanho_saida) {
            return 0; /* Precisa sobrar espaço para o terminador '\0'. */
        }

        saida[quantidade_escrita] = letra;
        quantidade_escrita++;
    }

    saida[quantidade_escrita] = '\0';
    if (quantidade_escrita == 0) {
        return 0;
    }
    return 1;
}

/* Separa as palavras do arquivo e acumula frequência e linhas na tabela. */
size_t aplicacao_indexar_texto(Tabela *tabela, FILE *arquivo, size_t *rejeitadas)
{
    char token[TAM_PALAVRA];
    char chave[TAM_PALAVRA];
    size_t tamanho_token = 0;
    size_t ocorrencias_aceitas = 0;
    int numero_linha = 1;
    int palavra_longa = 0;
    int caractere;
    *rejeitadas = 0;

    do {
        caractere = fgetc(arquivo); /* int também permite representar EOF. */
        if (caractere != EOF) {
            if (isalnum((unsigned char)caractere) || (unsigned char)caractere >= 128) {
                if (tamanho_token < sizeof token - 1) {
                    token[tamanho_token] = (char)caractere;
                    tamanho_token++;
                } else {
                    palavra_longa = 1;
                }
                continue;
            }
        }

        /* Espaço, pontuação ou fim do arquivo encerra o token atual. */
        if (palavra_longa == 1) {
            (*rejeitadas)++;
        } else if (tamanho_token > 0) {
            token[tamanho_token] = '\0';
            int chave_valida = normalizar_palavra(token, chave, sizeof chave);

            if (chave_valida == 1) {
                Palavra *palavra = tabela_buscar(tabela, chave);

                if (palavra == NULL) {
                    Resultado resultado = tabela_inserir(tabela, chave, 0);
                    if (resultado == RES_CHEIA) {
                        (*rejeitadas)++;
                    } else {
                        palavra = tabela_buscar(tabela, chave);
                    }
                }

                /* Também acumula palavras existentes quando a tabela está cheia. */
                if (palavra != NULL) {
                    palavra->frequencia++;
                    tabela_adicionar_linha(palavra, numero_linha);
                    ocorrencias_aceitas++;
                }
            }
        }

        tamanho_token = 0;
        palavra_longa = 0;
        if (caractere == '\n') {
            numero_linha++;
        }
    } while (caractere != EOF);

    return ocorrencias_aceitas;
}
