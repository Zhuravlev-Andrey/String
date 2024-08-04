#include "s21_string.h"

char *s21_strchr(const char *str, int c) {
  char *success = s21_NULL;
  while (*str != '\0') {
    if (*str == (char)c) {
      success = (char *)str;
    }
    str++;
  }
  if (c == '\0') {
    success = (char *)str;
  }
  return success;
}
