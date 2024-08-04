#include "s21_string.h"

void *s21_insert(const char *src, const char *str, s21_size_t start_index) {
  char *new_str = s21_NULL;
  if (src != s21_NULL && str != s21_NULL) {
    s21_size_t str_len = s21_strlen(str);
    s21_size_t src_len = s21_strlen(src);
    if (src_len >= start_index) {
      s21_size_t length = str_len + src_len + 1;
      new_str = calloc(length, sizeof(char));
      for (size_t i = 0; i < start_index && src[i] != '\0'; i++) {
        new_str[i] = src[i];
      }
      for (size_t i = 0; i < str_len; i++) {
        new_str[start_index + i] = str[i];
      }
      for (size_t i = start_index + str_len; i < length - 1; i++) {
        new_str[i] = src[i - str_len];
      }

      new_str[length - 1] = '\0';
    }
  }
  return new_str;
}