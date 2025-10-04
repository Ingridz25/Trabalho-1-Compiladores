# Implementação de AFD — Scanner de Números (Compiladores)

Projeto de compiladores que implementa um **AFD no estilo “Solução/Tipo 2”** (estado corrente + `switch`) para reconhecer **o primeiro token numérico** em uma linha de entrada, no formato:

```
-? d+ ( , d+ )?
```

Ou seja: **inteiros** (ex.: `-42`, `007`) e **reais com vírgula** (ex.: `3,14`).
O programa percorre a linha da esquerda para a direita e imprime **apenas o primeiro token válido** encontrado.

---

## 🎯 O que o programa reconhece

* Inteiros: `0`, `7`, `007`, `-42`
* Reais com vírgula: `3,14`, `0,0`, `-12,30`
* **Não** reconhece:

  * `123,` (vírgula sem dígito depois)
  * `,5` (não há parte inteira antes da vírgula)
  * `-` sozinho (sinal sem dígito)
  * Reais com **ponto** decimal (`3.14` → reconhece apenas `3`)

---

## 🧠 Como funciona (visão geral do AFD)

Estados (aceitação em **negrito**):

* `0` — início (ignorando “lixo”); se ler `-` → `1`; se ler dígito → `2`
* `1` — leu sinal; exige **dígito** (se não houver, descarta e volta ao `0`)
* **`2`** — parte inteira; dígito continua em `2`; `,` vai para `3`; outro char → **finaliza inteiro**
* `3` — leu vírgula; exige **primeiro dígito fracionário** (senão descarta tudo e volta ao `0`)
* **`4`** — parte fracionária; dígito continua em `4`; outro char → **finaliza real**
* `5` — estado técnico de **finalização** (sai do laço e imprime o token)

Técnicas usadas:

* **Lookahead/retrocesso** com `getNextChar` e `ungetNextChar`
* **Não salva** `-` nem `,` até confirmar que o próximo símbolo é válido
* **Varredura resiliente**: se um começo falso aparecer (ex.: `-x`), descarta o parcial e segue procurando o próximo token válido

---

## 🗂️ Estrutura do repositório

```
.
├─ main.c          // código-fonte do scanner (AFD)
├─ README.md       // este arquivo
└─ docs/           // (opcional) material de apoio do trabalho
```

> Se tiver arquivos de enunciado/relatório, coloque-os em `docs/`.

---

## 🧩 Requisitos

* Compilador C (C11 ou C99 servem)
* Linux/Mac com `gcc` ou Windows (MinGW ou WSL)

---

## 🔧 Como compilar

```bash
gcc -Wall -Wextra -O2 main.c -o afd
```

No Windows (MinGW):

```bash
gcc -Wall -Wextra -O2 main.c -o afd.exe
```

---

## ▶️ Como executar

Rode e digite uma linha por vez:

```bash
./afd
Insira a entrada: var := 3,14
-> 3,14
Continuar? (1 - Sim, 0 - Nao): 0
```

---

## ✅ Exemplos de entrada → saída esperada

> O programa **imprime só o primeiro token válido** da linha.

| Entrada             | Saída                          | Observação                         |
| ------------------- | ------------------------------ | ---------------------------------- |
| `var := 3,14`       | `-> 3,14`                      | real com vírgula                   |
| `var := 3.14`       | `-> 3`                         | ponto não é decimal                |
| `-42 é inteiro`     | `-> -42`                       | inteiro negativo                   |
| `-x 12,30 e -7`     | `-> 12,30`                     | ignora início inválido             |
| `   007 itens`      | `-> 007`                       | zeros à esquerda                   |
| `0,0 ok`            | `-> 0,0`                       | real válido                        |
| `3,14.15`           | `-> 3,14`                      | para no ponto                      |
| `+77 ganhou`        | `-> 77`                        | `+` é ignorado; começa em `77`     |
| `-,5 depois tem 10` | `-> 10`                        | `-,5` é inválido; depois pega `10` |
| `- ,5   10`         | `-> 5`                         | espaço “quebra” e vira inteiro `5` |
| `123,`              | `Nenhum token foi reconhecido` | vírgula sem dígito                 |
| `-3,`               | `Nenhum token foi reconhecido` | idem                               |
| `,5`                | `Nenhum token foi reconhecido` | falta parte inteira                |
| `somente -`         | `Nenhum token foi reconhecido` | sinal isolado                      |
| `abc def`           | `Nenhum token foi reconhecido` | sem números válidos                |

---

## 📌 Decisões e comportamentos importantes

* **Apenas vírgula** como separador decimal (ponto não vale).
* **Sinal `-`** só é aceito se **imediatamente** seguido de dígito.
* Dígito **logo após vírgula** *só* vale se a vírgula veio após **parte inteira** (impede `,5`).
* Se o token parcial se mostrar inválido (ex.: `-x` ou `123,`), ele é **descartado** e a varredura continua.
* O programa retorna **o primeiro token válido** e para (não lista todos).

---

## 🧪 Dicas de teste rápido (sem interagir)

No Linux/Mac, você pode “injetar” entradas:

```bash
printf "var := 3,14\n0\n" | ./afd
printf "var := 3.14\n0\n" | ./afd
printf ",5\n0\n" | ./afd
printf "somente -\n0\n" | ./afd
```

---

## 🚧 Limitações conhecidas

* Não lista **todos** os números da mesma linha (só o primeiro).
* Não aceita **ponto** como separador decimal.
* Tamanho do token limitado a `MAXTOKEN` (padrão: 20 chars).
* Não há testes automatizados (somente testes manuais/visuais).

---

## 🔮 Extensões sugeridas

* **Listar todos os tokens** da linha (armazenar em um vetor e imprimir todos).
* Aceitar `+` explícito como sinal inicial.
* Aceitar **ponto** como separador (ou ambos `,`/`.` com regra regional).
* Suportar expoente (ex.: `1,23e-4`) — isso vira outro AFD.
* Testes automatizados com `CTest`/`Makefile` ou um script de comparação (`diff`).

---

## 🛠️ Solução de problemas (FAQ)

* **Erro de compilação “expected declaration or statement at end of input”**
  Quase sempre é chave `}` faltando. Use este `main.c` do repositório **sem editar** trechos do `switch`/`while`.

* **O programa aceitou `123,` ou `-` sozinho**
  Atualize para a versão atual de `main.c` (não salva `,`/`-` antes de confirmar o próximo dígito) e recompile.

* **Em `,5` imprimiu `5`**
  Versões antigas faziam isso. A atual impede iniciar número **imediatamente após** uma vírgula solta.

---

## 🧾 Licença

 **MIT**

---

## 👤 Autoria

* Seu nome / matrícula / disciplina
* Período e professor(a)
* (Opcional) Links para materiais de apoio do trabalho

---
