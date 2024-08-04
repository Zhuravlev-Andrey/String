#include "s21_string.h"

char *s21_strpbrk(const char *str1, const char *str2) {
  const char *result = s21_NULL;
  if (str1 == s21_NULL || str2 == s21_NULL) {
    result = s21_NULL;
  }
  int f = 0;
  for (s21_size_t i = 0; str1[i] != '\0' && f == 0; i++) {
    for (s21_size_t j = 0; str2[j] != '\0'; j++) {
      if (str1[i] == str2[j]) {
        result = (char *)&str1[i];
      }
    }
    if (result != s21_NULL) {
      f = 1;
    }
  }
  return (char *)result;
}
