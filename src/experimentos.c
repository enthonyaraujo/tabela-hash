/* Mede três volumes com cinco repetições. Não executa testes de correção. */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "tabela.h"

int main(void)
{
    size_t tamanhos[] = {100, 1000, 10000};
    int repeticoes = 5;

    puts("Médias de 5 repetições; tempos de CPU por lote, em ms.\n");
    puts("| n | m | alfa | Colisões | Inserção ms | Busca ok ms | Busca ausente ms |");
    puts("|---:|---:|---:|---:|---:|---:|---:|");

    for (int cenario = 0; cenario < 3; cenario++) {
        size_t quantidade_registros = tamanhos[cenario];
        size_t capacidade = 2 * quantidade_registros;

        /* Vetor de strings: metade será inserida, metade será consultada como ausente. */
        char (*chaves)[32] = malloc(2 * quantidade_registros * sizeof *chaves);
        if (chaves == NULL) {
            return 1;
        }
        for (size_t indice = 0; indice < 2 * quantidade_registros; indice++) {
            snprintf(chaves[indice], 32, "palavra%zu", indice);
        }

        double tempo_insercao = 0;
        double tempo_busca_presente = 0;
        double tempo_busca_ausente = 0;
        double total_colisoes = 0;

        for (int repeticao = 0; repeticao < repeticoes; repeticao++) {
            Tabela tabela;
            if (tabela_criar(&tabela, capacidade) == 0) {
                free(chaves);
                return 1;
            }

            /* Cada marca de tempo separa um lote de operações do seguinte. */
            clock_t inicio_insercao = clock();
            for (size_t indice = 0; indice < quantidade_registros; indice++) {
                tabela_inserir(&tabela, chaves[indice], 1);
            }
            clock_t fim_insercao = clock();

            for (size_t indice = 0; indice < quantidade_registros; indice++) {
                tabela_buscar(&tabela, chaves[indice]);
            }
            clock_t fim_busca_presente = clock();

            for (size_t indice = quantidade_registros; indice < 2 * quantidade_registros; indice++) {
                tabela_buscar(&tabela, chaves[indice]);
            }
            clock_t fim_busca_ausente = clock();

            /* Converte a diferença de ticks em milissegundos e soma as repetições. */
            tempo_insercao += 1000.0 * (fim_insercao - inicio_insercao) / CLOCKS_PER_SEC;
            tempo_busca_presente += 1000.0 * (fim_busca_presente - fim_insercao) / CLOCKS_PER_SEC;
            tempo_busca_ausente += 1000.0 * (fim_busca_ausente - fim_busca_presente) / CLOCKS_PER_SEC;
            total_colisoes += tabela.colisoes;
            tabela_destruir(&tabela);
        }

        double media_colisoes = total_colisoes / repeticoes;
        double media_insercao = tempo_insercao / repeticoes;
        double media_busca_presente = tempo_busca_presente / repeticoes;
        double media_busca_ausente = tempo_busca_ausente / repeticoes;

        printf("| %zu | %zu | 0.50 | %.0f | %.3f | %.3f | %.3f |\n",
               quantidade_registros, capacidade, media_colisoes, media_insercao,
               media_busca_presente, media_busca_ausente);
        free(chaves);
    }

    return 0;
}
