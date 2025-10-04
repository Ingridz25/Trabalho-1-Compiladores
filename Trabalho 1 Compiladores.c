#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

#define MAXENTRADA 100
#define MAXTOKEN   20

// Prototypes
char *verificarAFD(const char *entrada);
char  getNextChar(const char *entrada);
void  ungetNextChar(void);

// Posição da cabeça de leitura na entrada
static int posEntrada = 0;
// Posição de escrita no token de saída
static int posToken   = 0;

int main(void) {
  char entrada[MAXENTRADA];
  char *saida;
  int opcao;

  do {
    printf("Insira a entrada: ");
    if (!fgets(entrada, sizeof(entrada), stdin)) {
      // EOF ou erro de leitura
      break;
    }

    // Remover o '\n' do final (se houver)
    size_t n = strlen(entrada);
    if (n > 0 && entrada[n - 1] == '\n') entrada[n - 1] = '\0';

    saida = verificarAFD(entrada);

    if (saida[0] == '\0') {
      printf("Nenhum token foi reconhecido\n");
    } else {
      printf("-> %s\n", saida);
    }

    printf("Continuar? (1 - Sim, 0 - Nao): ");
    if (scanf("%d", &opcao) != 1) break;

    // Consumir resto da linha do scanf
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {}
  } while (opcao == 1);

  return 0;
}

// Retorna o caracter que a cabeça de leitura aponta e depois a avança.
char getNextChar(const char *entrada) {
  return entrada[posEntrada++];
}

// Move a cabeça de leitura uma posição à esquerda
void ungetNextChar(void) {
  if (posEntrada > 0) posEntrada--;
}

char *verificarAFD(const char *entrada) {
  static char saida[MAXTOKEN];

  int estado = 0;
  int save   = TRUE;

  posEntrada = 0;
  posToken   = 0;
  saida[0]   = '\0';

  // Busca o primeiro token válido: -?d+(,d+)?
  while (estado != 5) {
    char atual = getNextChar(entrada);

    // Fim da string: só aceita se terminou em estado final (2 ou 4)
    if (atual == '\0') {
      if (estado == 2 || estado == 4) {
        break; // token já está em 'saida'
      } else {
        // estados não-finais (0,1,3) -> descarta
        saida[0] = '\0';
        posToken = 0;
        break;
      }
    }

    save = TRUE;

    switch (estado) {
      case 0: {
        if (atual == '-') {
          estado = 1;
          save = FALSE;                 // NÃO salva '-' até confirmar dígito
        } else if (isdigit((unsigned char)atual)) {
          // Só inicia número se o caractere anterior NÃO for vírgula
          int prevIdx = posEntrada - 2; // posição do char anterior a 'atual'
          char prev   = (prevIdx >= 0) ? entrada[prevIdx] : '\0';
          if (prev == ',') {
            // Ex.: ",5" -> não iniciar token aqui
            save = FALSE;               // ignora esse dígito e segue varrendo
          } else {
            estado = 2;
          }
        } else {
          // ignora lixo (inclusive vírgula isolada)
          save = FALSE;
        }
        break;
      }

      case 1: // sinal lido; precisa de dígito para começar
        if (isdigit((unsigned char)atual)) {
          // agora sim: salva '-' e o primeiro dígito
          if (posToken < MAXTOKEN - 1) saida[posToken++] = '-';
          if (posToken < MAXTOKEN - 1) { saida[posToken++] = atual; saida[posToken] = '\0'; }
          save = FALSE;                 // já salvamos 'atual' manualmente
          estado = 2;
        } else {
          // sinal isolado inválido: descarta e recomeça
          save = FALSE;
          posToken = 0; saida[0] = '\0';
          estado = 0;
          ungetNextChar();              // reprocessa 'atual' no estado 0
        }
        break;

      case 2: // parte inteira
        if (isdigit((unsigned char)atual)) {
          estado = 2;
        } else if (atual == ',') {
          estado = 3;
          save = FALSE;                 // segura a vírgula até confirmar 1º dígito
        } else {
          ungetNextChar();
          save = FALSE;
          estado = 5;                   // finaliza inteiro
        }
        break;

      case 3: // após vírgula, exige 1º dígito fracionário
        if (isdigit((unsigned char)atual)) {
          if (posToken < MAXTOKEN - 1) saida[posToken++] = ',';
          if (posToken < MAXTOKEN - 1) { saida[posToken++] = atual; saida[posToken] = '\0'; }
          save = FALSE;                 // já salvamos manualmente
          estado = 4;
        } else {
          // vírgula sem dígito: descarta token parcial e recomeça
          save = FALSE;
          posToken = 0; saida[0] = '\0';
          estado = 0;
          ungetNextChar();
        }
        break;

      case 4: // parte fracionária
        if (isdigit((unsigned char)atual)) {
          estado = 4;
        } else {
          ungetNextChar();
          save = FALSE;
          estado = 5;                   // finaliza real
        }
        break;

      default:
        estado = 0;
        save = FALSE;
        break;
    }

    if (save && estado != 0 && estado != 5 && posToken < (MAXTOKEN - 1)) {
      saida[posToken++] = atual;
      saida[posToken]   = '\0';
    }
  }

  return saida;
}
