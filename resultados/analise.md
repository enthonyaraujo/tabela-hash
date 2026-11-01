# Análise experimental — configuração 40

A tabela em `experimentos_config40.md` contém médias de cinco repetições para
100, 1.000 e 10.000 registros. Usamos m=2n: capacidade fixa suficiente e alfa=0,5.
Não há redimensionamento. Em cada repetição, uma tabela nova recebe as mesmas
chaves sintéticas `palavra0` até `palavra(n-1)`; as próximas n são buscas ausentes.
São medidos tempos de CPU por lote e colisões de inserção.

Mesmo mantendo alfa=0,5, o número de colisões varia com o conjunto de chaves e a
capacidade. A ocupação global não garante índices uniformes: a dobra pode concentrar
chaves, e a sondagem linear forma blocos. Buscas ausentes precisam alcançar uma
posição vazia, enquanto buscas presentes param ao encontrar a palavra.

Os tempos medidos estão na tabela e devem ser conferidos na máquina da apresentação.
Não incluem leitura do texto, normalização nem armazenamento das linhas. Lotes pequenos
têm precisão limitada pela resolução do relógio. Alfa foi mantido constante:
estes resultados não demonstram uma curva de desempenho ao variar a ocupação.
A configuração atende às operações, mas pode visitar até m posições por busca.
