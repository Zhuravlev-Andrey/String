#include "s21_string.h"

char *s21_strtok(char *str, const char *delim) {
  static char *olds = s21_NULL;
  char *token = s21_NULL;
  if (str != s21_NULL) {
    olds = str;
  }
  if (olds != s21_NULL) {
    while (*olds != '\0' && s21_strchr(delim, *olds) != s21_NULL) {
      olds++;
    }
    if (*olds != '\0') {
      token = olds;
      while (*olds != '\0' && s21_strchr(delim, *olds) == s21_NULL) {
        olds++;
      }
      if (*olds != '\0') {
        *olds = '\0';
        olds++;
      } else {
        olds = s21_NULL;
      }
    }
  }
  return token;
}