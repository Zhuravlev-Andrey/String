#include "s21_string.h"

char *s21_strstr(const char *haystack, const char *needle) {
  char *result = s21_NULL;

  if (!*needle) result = (char *)haystack;

  s21_size_t needle_len = s21_strlen(needle);

  for (; *haystack; haystack++) {
    if (*haystack == *needle &&
        s21_memcmp((const void *)haystack, needle, needle_len) == 0)
      result = (char *)haystack;
  }
  return result;
}
