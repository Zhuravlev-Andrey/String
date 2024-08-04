#include "s21_string.h"

char *s21_strrchr(const char *str, int c) {
  const char *result = str + s21_strlen(str);

  while (result != str && *result != c) {
    result--;
  }
  if (result == str && *result != c) {
    result = s21_NULL;
  }
  return (char *)result;
}
