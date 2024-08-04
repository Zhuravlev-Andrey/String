#include "s21_string.h"

int s21_strncmp(const char *str1, const char *str2, s21_size_t n) {
  char c1, c2;
  int flag = 0;
  while (n && flag == 0) {
    c1 = *str1++;
    c2 = *str2++;
    if (c1 != c2) {
      flag = c1 - c2;
    }
    n--;
  }
  return flag;
}