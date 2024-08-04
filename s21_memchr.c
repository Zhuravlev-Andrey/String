#include "s21_string.h"

void *s21_memchr(const void *str, int c, s21_size_t n) {
  unsigned char *byte = (unsigned char *)str;
  unsigned char *sucsess = s21_NULL;
  while (n > 0 && sucsess == s21_NULL) {
    if (*byte == c) {
      sucsess = byte;
    }
    byte++;
    n--;
  }
  return sucsess;
}