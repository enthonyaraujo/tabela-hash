# Testes de correção pelo menu — configuração 40

Compile o aplicativo conforme o README e execute `./hashapp 2`.
Digite os números no campo Opção e as palavras/frequências quando forem pedidas.
Execute na ordem abaixo, sem reiniciar entre os casos. A coluna Obtido registra
a conferência do menu nesta versão; a dupla deve repetir e conferir na apresentação.

| Caso | Entradas no menu | Esperado | Obtido na conferência | Situação |
|---|---|---|---|---|
| Tabela vazia | 2; palavra `hash` | Chave não encontrada | Chave não encontrada | OK |
| Inserção e busca | 1; `hash`; frequência 1. Depois 2; `hash` | OK; frequência 1 | OK; `hash: frequência=1; linhas=[]` | OK |
| Chave inexistente | 2; palavra `ausente` | Chave não encontrada | Chave não encontrada | OK |
| Duplicata | 1; `hash`; frequência 9. Depois 2; `hash` | Chave duplicada; mantém frequência 1 | Chave duplicada; frequência 1 | OK |
| Colisão | 1; `tabela`; frequência 2. Depois 6 | Índices iniciais 0 e 0; posições 0 e 1; uma colisão | `[0] h=0 hash`; `[1] h=0 tabela`; colisões=1 | OK |
| Tabela cheia | 1; `extra`; frequência 1 | Tabela cheia | Tabela cheia | OK |
| Remoção em colisão | 4; `hash`. Depois 2; `tabela`. Depois 2; `hash`. Depois 6 | `tabela` presente; `hash` ausente; posição 0 REMOVIDO | Frequência de tabela=2; hash não encontrada; `[0] REMOVIDO` | OK |
| Atualização | 3; `tabela`; frequência 7. Depois 2; `tabela`. Depois 5 | Mesma chave, frequência 7; uma palavra na lista | `tabela: frequência=7; linhas=[]`; uma palavra distinta | OK |

Escolha 0 para sair. Linhas vazias são esperadas em inserções manuais.
Redimensionamento não se aplica à estrutura fixa E1.1.

Para conferir a aplicação A4 com frequência e linhas, abra `./hashapp 64`:

1. Escolha 7 e informe `dados/texto_exemplo.txt`.
2. Confira 55 ocorrências, 41 palavras distintas e 0 rejeitadas.
3. Escolha 2 e informe `Hash`: frequência 3, linhas 1, 2 e 5.
4. Escolha 5 para listar e 6 para visualizar a estrutura.
5. Escolha 0 para sair.
