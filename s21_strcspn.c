#include "s21_string.h"

s21_size_t s21_strcspn(const char *str1, const char *str2) {
  const char *tmp1 = str1;
  s21_size_t count1 = 0;
  s21_size_t count2 = s21_strlen(str1);

  for (; *str2 != '\0';) {
    int f = 0;
    for (; *str1 != '\0' && f == 0;) {
      if (*str1 != *str2) {
        str1++;
        count1++;
      } else {
        f = 1;
      }
    }
    str2++;
    if (count1 <= count2) {
      count2 = count1;
      count1 = 0;
    }
    count1 = 0;
    str1 = tmp1;
  }
  return count2;
}