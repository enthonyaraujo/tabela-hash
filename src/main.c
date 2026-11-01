#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aplicacao.h"

/* Lê uma linha e remove a quebra. Retorna 0 quando acaba a entrada. */
static int ler_linha(const char *rotulo, char *texto, size_t tamanho)
{
    printf("%s: ", rotulo);
    if (fgets(texto, (int)tamanho, stdin) == NULL) {
        return 0;
    }

    size_t fim_linha = strcspn(texto, "\r\n");
    if (texto[fim_linha] == '\0' && !feof(stdin)) {
        /* Descarta o excesso para não afetar a próxima resposta do usuário. */
        int caractere;
        do {
            caractere = getchar();
        } while (caractere != '\n' && caractere != EOF);

        texto[0] = '\0';
        puts("Entrada longa demais.");
    } else {
        texto[fim_linha] = '\0';
    }

    return 1;
}

/* A capacidade é opcional: ./hashapp usa 64; ./hashapp 2 usa duas posições. */
int main(int quantidade_argumentos, char **argumentos)
{
    char *fim_numero; /* Indica se sobraram caracteres depois do número. */
    unsigned long capacidade = 64;

    if (quantidade_argumentos > 2) {
        fputs("Uso: ./hashapp [capacidade]\n", stderr);
        return 1;
    }
    if (quantidade_argumentos == 2) {
        capacidade = strtoul(argumentos[1], &fim_numero, 10);
        if (argumentos[1][0] == '\0' || *fim_numero != '\0' || capacidade < 1 || capacidade > 1000000) {
            fputs("Capacidade deve ser de 1 a 1000000.\n", stderr);
            return 1;
        }
    }

    Tabela tabela;
    if (tabela_criar(&tabela, capacidade) == 0) {
        fputs("Não foi possível criar a tabela.\n", stderr);
        return 1;
    }

    puts("Configuração 40: E1.1 / H4 / C2 / A4");
    char entrada[512];
    char chave[TAM_PALAVRA];

    while (1) {
        puts("\n1 Inserir | 2 Buscar | 3 Atualizar frequência | 4 Remover");
        puts("5 Listar | 6 Exibir estrutura | 7 Indexar arquivo | 0 Sair");

        if (ler_linha("Opção", entrada, sizeof entrada) == 0) {
            break;
        }
        long opcao = strtol(entrada, &fim_numero, 10);
        if (entrada[0] == '\0' || *fim_numero != '\0' || opcao < 0 || opcao > 7) {
            puts("Opção inválida.");
            continue;
        }

        if (opcao == 0) {
            break;
        }
        if (opcao == 5) {
            tabela_listar(&tabela);
            continue;
        }
        if (opcao == 6) {
            tabela_exibir_estrutura(&tabela);
            continue;
        }
        if (opcao == 7) {
            if (ler_linha("Arquivo", entrada, sizeof entrada) == 0) {
                break;
            }
            FILE *arquivo = fopen(entrada, "r");
            if (arquivo == NULL) {
                puts("Não foi possível abrir o arquivo.");
                continue;
            }

            size_t rejeitadas;
            size_t aceitas = aplicacao_indexar_texto(&tabela, arquivo, &rejeitadas);
            int erro_leitura = ferror(arquivo);
            fclose(arquivo);

            printf("%zu ocorrências indexadas; %zu palavras distintas; %zu rejeitadas (tabela cheia ou palavra longa).\n",
                   aceitas, tabela.quantidade, rejeitadas);
            if (erro_leitura != 0) {
                puts("Erro de leitura: o arquivo foi indexado parcialmente.");
            }
            continue;
        }

        /* As demais opções recebem uma palavra e usam a mesma normalização. */
        if (ler_linha("Palavra", entrada, sizeof entrada) == 0) {
            break;
        }
        if (normalizar_palavra(entrada, chave, sizeof chave) == 0) {
            puts("Palavra inválida.");
            continue;
        }

        if (opcao == 2) {
            Palavra *palavra = tabela_buscar(&tabela, chave);
            if (palavra == NULL) {
                puts("Chave não encontrada.");
            } else {
                printf("%s: frequência=%d; linhas=[", chave, palavra->frequencia);
                for (size_t indice = 0; indice < palavra->quantidade_linhas; indice++) {
                    if (indice > 0) {
                        printf(", ");
                    }
                    printf("%d", palavra->linhas[indice]);
                }
                puts("]");
            }
        } else if (opcao == 4) {
            Resultado resultado = tabela_remover(&tabela, chave);
            puts(resultado_texto(resultado));
        } else {
            /* Só restam inserir (1) e atualizar (3), que recebem uma frequência. */
            if (ler_linha("Frequência", entrada, sizeof entrada) == 0) {
                break;
            }
            long frequencia = strtol(entrada, &fim_numero, 10);
            if (entrada[0] == '\0' || *fim_numero != '\0' || frequencia < 1 || frequencia > 1000000) {
                puts("Frequência deve ser de 1 a 1000000.");
                continue;
            }

            Resultado resultado;
            if (opcao == 1) {
                resultado = tabela_inserir(&tabela, chave, (int)frequencia);
            } else {
                resultado = tabela_atualizar(&tabela, chave, (int)frequencia);
            }
            puts(resultado_texto(resultado));
        }
    }

    tabela_destruir(&tabela);
    return 0;
}
