#include "s21_string.h"

void *s21_to_upper(const char *str) {
  char *new_str = s21_NULL;
  if (str != s21_NULL) {
    s21_size_t length = s21_strlen(str) + 1;
    new_str = calloc(length + 1, sizeof(char));
    for (s21_size_t i = 0; i < length; i++) {
      if (str[i] >= 'a' && str[i] <= 'z')
        new_str[i] = str[i] - ('a' - 'A');
      else
        new_str[i] = str[i];
    }
    new_str[length] = '\0';
  }
  return new_str;
}
