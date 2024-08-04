#include "s21_string.h"

void get_start_index(const char *src, const char *trim_chars,
                     s21_size_t *index_start, s21_size_t length,
                     s21_size_t chars_length) {
  int f = 0;
  for (s21_size_t idx = 0; idx < length && f == 0; idx++) {
    s21_size_t is_found = 0;
    for (s21_size_t j = 0; j < chars_length && is_found == 0; j++) {
      if (src[idx] == trim_chars[j]) {
        is_found = 1;
      }
    }
    *index_start = idx;
    if (is_found == 0) f = 1;
  }
}

void get_end_index(const char *src, const char *trim_chars,
                   s21_size_t *index_end, s21_size_t length,
                   s21_size_t chars_length) {
  int f = 0;
  for (int idx = length - 1; idx > 0 && f == 0; idx--) {
    s21_size_t is_found = 0;
    for (s21_size_t j = 0; j < chars_length && is_found == 0; j++) {
      if (src[idx] == trim_chars[j]) {
        is_found = 1;
      }
    }
    *index_end = idx;
    if (is_found == 0) f = 1;
  }
}

void *s21_trim(const char *src, const char *trim_chars) {
  char *new_str = s21_NULL;
  if (src != s21_NULL) {
    s21_size_t length = s21_strlen(src);
    if (trim_chars == NULL) trim_chars = " \t\n";
    s21_size_t chars_length = s21_strlen(trim_chars);
    s21_size_t index_start = 0;
    s21_size_t index_end = (length > 0) ? length - 1 : 0;
    get_start_index(src, trim_chars, &index_start, length, chars_length);
    get_end_index(src, trim_chars, &index_end, length, chars_length);
    if (index_start <= index_end) {
      s21_size_t new_length = index_end - index_start + 1 + 1;
      new_str = calloc(new_length + 1, sizeof(char));
      for (size_t i = 0, j = index_start; i < new_length - 1; i++, j++) {
        new_str[i] = src[j];
      }
      new_str[new_length] = '\0';
    } else if (index_start > index_end) {
      new_str = calloc(1, sizeof(char));
      new_str[0] = '\0';
    }
  }
  return new_str;
}