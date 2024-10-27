#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <util/setbaud.h>

int uart_putchar(char, FILE *);
int uart_getchar(FILE *);
void uart_init(void);

FILE uart_output = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
FILE uart_input = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);

#define CMD_SIZE  2048

unsigned char *PROMPT = ". ";

unsigned char *cmd, *cptr, *dptr = 0;

void brainfuck(void);

int main(void)
{
  uart_init();

  stdin = &uart_input;
  stdout = &uart_output;

  cmd = malloc(CMD_SIZE);
  memset(cmd, 0, CMD_SIZE);
  cptr = cmd;

  printf(PROMPT);

  while(1) {
    *cptr++ = getchar();

    if (cptr > cmd + CMD_SIZE) {
      memset(cmd, 0, CMD_SIZE);
      cptr = cmd;
    }

    putc(*(cptr - 1), stdout);

    if (*(cptr - 1) == '\r') {
      putc('\n', stdout);

      brainfuck();
      printf(PROMPT);
    }
  }
}

int uart_putchar(char c, FILE *stream)
{
  if (c == '\n') {
    uart_putchar('\r', stream);
  }

  loop_until_bit_is_set(UCSR0A, UDRE0);

  UDR0 = c;
}

int uart_getchar(FILE *stream)
{
  loop_until_bit_is_set(UCSR0A, RXC0);

  return UDR0;
}

void uart_init(void)
{
  UBRR0H = UBRRH_VALUE;
  UBRR0L = UBRRL_VALUE;

#if USE_2X
  UCSR0A |= _BV(U2X0);
#else
  UCSR0A &= ~(_BV(U2X0));
#endif

  UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
  UCSR0B = _BV(RXEN0) | _BV(TXEN0);
}

void brainfuck(void)
{
  cptr = cmd;

  while (*cptr != '\r') {
    switch (*cptr) {
      case '>':
        ++dptr;
        break;

      case '<':
        --dptr;
        break;

      case '+':
        *dptr = *dptr + 1;
        break;

      case '-':
        *dptr = *dptr - 1;
        break;

      case '.':
        putc(*dptr, stdout);
        break;

      case ',':
        *dptr = getchar();
        break;

      case '[':
        if (!*dptr) {
          while (*dptr++ != ']');
          continue;
        }

        break;

      case ']':
        while (*dptr && *--dptr != '[');
        break;

      default:
        break;
    }

    ++cptr;
  }

  memset(cmd, 0, CMD_SIZE);
  cptr = cmd;
  dptr = 0;
}
