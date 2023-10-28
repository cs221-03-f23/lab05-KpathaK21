#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scan.h"

void scan_table_init(struct scan_table_st *tt) {
  // initialize the count to zero, like we did for the map in lab01
  tt->head = NULL;
  tt->len = 0;
}

struct scan_token_st *scan_table_new_token(struct scan_table_st *tt) {
  struct scan_token_st *tp = calloc(1, sizeof(struct scan_token_st));
  if (!tt->head) {
    tt->head = tp;
  } else {
    struct scan_token_st *walk = tt->head;
    while (1) {
      if (!walk->next) {
        walk->next = tp;
        break;
      } else {
        walk = walk->next;
      }
    }
  }

  // increment the length of reserved tokens
  tt->len += 1;
  return tp;
}

// scan_read_token is a helper function to generalize populating a token
char *scan_read_token(struct scan_token_st *tp, char *p, int len,
                      enum scan_token_enum id) {

  // fill in the ID of the new token
  tp->id = id;

  // fill in the name of the new token for 'len' bytes
  int i;
  for (i = 0; i < len; i++) {
    tp->name[i] = *p;
    p += 1;
  }
  tp->name[i] = '\0'; // null-terminate the name string
  return p;
}

bool scan_is_whitespace(char c) { return c == ' ' || c == '\t'; }

char *scan_whitespace(char *p, char *end) {
  while (scan_is_whitespace(*p) && p < end) {
    p += 1;
  }
  return p;
}

bool scan_is_digit(char c) { return c >= '0' && c <= '9'; }

char *scan_intlit(struct scan_token_st *tp, char *p, int len,
                  enum scan_token_enum id) {
  // TODO: fill this in, like scan_read_token except just for integers

  int length = 0;
  while (p + len < p + strlen(p) &&
         (scan_is_digit(p[length]) || (p[length] == '-' && len == 0))) {
    length++;
  }

  return scan_read_token(tp, p, length, TK_INTLIT);
}

char *scan_binlit(struct scan_token_st *tp, char *p, int len,
                  enum scan_token_enum id) {

  int length = 0;

  while (p + len < p + strlen(p) && (p[0] == '0' || p[length] == '1' ||
                                     (p[1] == 'b' && length == 1) ||
                                     (p[1] == 'B' && length == 1))) {
    length++;
  }

  return scan_read_token(tp, p, length, TK_BINLIT);
}

char *scan_hexlit(struct scan_token_st *tp, char *p, int len,
                  enum scan_token_enum id) {

  int length = 0;

  while (p + len < p + strlen(p) &&
         (isxdigit(p[length]) || (p[1] == 'x' && length == 1) ||
          (p[1] == 'X' && length == 1))) {
    length++;
  }

  return scan_read_token(tp, p, length, TK_HEXLIT);
}

char *scan_token(struct scan_token_st *tp, char *p, char *end) {

  /* TODO
      add cases for binlit and hexlit
      add cases for the other symbols
  */
  if (p == end) {
    p = scan_read_token(tp, p, 0, TK_EOT);
  } else if (scan_is_whitespace(*p)) {
    // skip over the whitespace
    p = scan_whitespace(p, end);
    // recurse to get the next token
    p = scan_token(tp, p, end);
  } else if (scan_is_digit(*p)) {
    p = scan_intlit(tp, p, 1, TK_INTLIT);
  } else if (*p == '+') {
    p = scan_read_token(tp, p, 1, TK_PLUS);
  } else if (*p == '-') {
    p = scan_read_token(tp, p, 1, TK_MINUS);
  } else if (*p == '*') {
    p = scan_read_token(tp, p, 1, TK_MULT);
  } else if (*p == '/') {
    p = scan_read_token(tp, p, 1, TK_DIV);
  } else if (*p == '(') {
    p = scan_read_token(tp, p, 1, TK_LPAREN);
  } else if (*p == ')') {
    p = scan_read_token(tp, p, 1, TK_RPAREN);
  } else if (*p == '0' && (p[1] == 'b' || p[1] == 'B')) {
    p = scan_binlit(tp, p, 1, TK_BINLIT);
  } else if (*p == '0' && (p[1] == 'x' || p[1] == 'X')) {
    p = scan_hexlit(tp, p, 1, TK_HEXLIT);
  }
  return p;
}

void scan_table_scan(struct scan_table_st *tt, char *p) {
  char *end = p + strlen(p);

  // loop over the input text, getting one token each time through the loop
  while (true) {
    struct scan_token_st *tp = scan_table_new_token(tt);
    p = scan_token(tp, p, end);
    if (tp->id == TK_EOT) {
      break;
    }
  }
}

void scan_table_print(struct scan_table_st *tt) {

  // define a variable containing the list of token ID names
  char *id_names[NUM_TOKENS] = SCAN_STRINGS;

  struct scan_token_st *tp = tt->head;
  while (tp) {
    // print the ID and name of that token
    printf("%s(\"%s\")\n", id_names[tp->id], tp->name);
    tp = tp->next;
  }
}

int main(int argc, char **argv) {
  struct scan_table_st table;

  if (argc == 1) {
    printf("Usage: lab05 <expression>");
    return -1;
  }

  scan_table_init(&table);
  scan_table_scan(&table, argv[1]);
  scan_table_print(&table);

  return 0;
}
