#include "s21_string.h"

s21_size_t s21_strspn(const char *str1, const char *str2) {
  s21_size_t len = 0;
  while (*str1 && s21_strchr(str2, *str1++)) {
    len++;
  }
  return len;
}
