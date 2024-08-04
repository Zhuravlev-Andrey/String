#include "s21_string.h"

int s21_memcmp(const void *str1, const void *str2, s21_size_t n) {
  unsigned char *ptr1 = (unsigned char *)str1;
  unsigned char *ptr2 = (unsigned char *)str2;
  int f = 0;
  for (s21_size_t i = 0; i < n && f == 0; i++) {
    if (ptr1[i] != ptr2[i]) f = ptr1[i] - ptr2[i];
  }
  return f;
}