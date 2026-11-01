# Tabela hash — ED II

**Enthony e Douglas · Prof. Kennedy Lopes**

Configuração **40: E1.1 / H4 / C2 / A4**: vetor de capacidade fixa,
hash por dobra, sondagem linear e índice de palavras. Linguagem C11,
sem tabela hash pronta ou bibliotecas externas.

## Execução

Na pasta do projeto, compile diretamente com GCC:

```bash
gcc -std=c11 -Wall -Wextra -O2 src/tabela.c src/aplicacao.c src/main.c -o hashapp
gcc -std=c11 -Wall -Wextra -O2 src/tabela.c src/experimentos.c -o experimentos
```

O primeiro comando gera o aplicativo; o segundo gera somente o programa de medições.
Os testes de correção são feitos pelo menu do `hashapp`, sem testes automatizados.
Recompile depois de alterar os fontes.

```bash
./hashapp                 # capacidade fixa de 64 posições
./hashapp 32              # capacidade escolhida antes do uso
./experimentos           # três volumes, cinco repetições por volume
```

O roteiro com entradas, resultados esperados e obtidos está em
`resultados/testes_manuais.md`. Para salvar as medições:

```bash
mkdir -p resultados
./experimentos > resultados/experimentos_config40.md
```

O menu permite inserir, consultar, atualizar frequência, remover, listar,
exibir a estrutura e indexar um arquivo. Inserções manuais começam sem linhas;
a indexação registra frequência e linhas distintas. Atualizar mantém chave e linhas.

## Decisões obrigatórias

- **Chave:** palavra em minúsculas, sem pontuação e sem os acentos portugueses
  mapeados no código. Espaços/pontuação separam palavras no arquivo.
  O menu recebe uma palavra por vez. Limite: 255 bytes; tokens maiores são rejeitados.
- **Conversão:** sobre os bytes normalizados, `numero = numero * 131 + byte`,
  usando inteiro sem sinal de 64 bits (módulo 2⁶⁴).
- **H4:** somar grupos de quatro dígitos decimais e aplicar `soma % m`.
  Exemplo: `hash` → 235481250 → `2+3548+1250=4800` → índice 0 com m=2.
- **C2:** procurar `(h+i)%m`, até m posições. `hash` e `tabela` colidem com m=2.
- **E1.1:** cada posição contém estado e palavra com frequência/linhas. O vetor
  é alocado uma vez e não cresce. Somente o vetor de linhas de cada palavra cresce.
- **Remoção:** deixa `REMOVIDO`, preservando as buscas após colisões. Inserções
  reutilizam essa posição; buscas só param em `VAZIO`, na chave ou após m posições.
- **Duplicata manual:** rejeitada. Palavra repetida na indexação incrementa frequência.
  Ausência retorna NULL; tabela cheia rejeita novas palavras com mensagem.
- **Contadores:** `quantidade` é n, a ocupação atual; `capacidade` é m; alfa=n/m. Colisão é contada uma vez por
  inserção aceita de chave nova cuja posição inicial já esteja ocupada por outra.
  Atualizações não contam; remoções não diminuem o histórico de colisões.

## Arquivos e verificação

| Arquivo | Função |
|---|---|
| `src/tabela.h`, `src/tabela.c` | Tipos, conversão, dobra, vetor fixo e operações |
| `src/aplicacao.h`, `src/aplicacao.c` | Normalização, leitura, frequência e linhas |
| `src/main.c` | Menu |
| `src/experimentos.c` | Medições da análise experimental |
| `resultados/testes_manuais.md` | Roteiro e registro dos testes pelo menu |
| `dados/` | Texto e entradas das demonstrações |
| `resultados/` | Saídas dos testes, médias e análise |
| `.gitignore` | Executáveis e objetos gerados que não entram no Git |
| `slides/apresentacao.tex`, `slides/apresentacao.pdf` | Fonte LaTeX Beamer e apresentação final |

O roteiro manual cobre os oito casos aplicáveis, com tabela vazia e cheia
separadas. Redimensionamento não se aplica a E1.1. Use a opção 6 para evidenciar
colisões e remoções. Registre entrada, esperado, obtido e aprovação ao repetir os testes.

Os experimentos usam n=100, 1.000 e 10.000, m=2n, alfa=0,5 e cinco repetições.
As chaves sintéticas são `palavra0`, `palavra1` etc. As primeiras n são inseridas,
as próximas n são ausentes. Mede-se tempo de CPU por lote, em ms, sem geração
das chaves, leitura de arquivo ou normalização. Para tempo por operação, dividir por n.
A mesma sequência e a mesma regra de medição são usadas em cada repetição.
A tabela não redimensiona. Tempos dependem da máquina e da resolução de `clock()`.
Resultados e interpretação estão em `resultados/`.

## Entrega

Repositório GitHub com código, README, dados/testes, resultados e slides finais.
A apresentação Beamer está em `slides/apresentacao.pdf`, com fonte editável em
`slides/apresentacao.tex`. São 27 slides, com explicações e demonstrações finais.
A divisão das falas e a recompilação estão em `docs/ROTEIRO_APRESENTACAO.md`.
Data e forma de apresentação: conferir SIGAA.
