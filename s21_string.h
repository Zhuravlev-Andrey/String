#ifndef S21_STRING
#define S21_STRING

#include <limits.h>
#include <locale.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#define SIZE 10000
#define s21_NULL (void *)0
#define INIT_SIZE 512

typedef unsigned long long s21_size_t;

typedef struct flags {
  unsigned short minus;
  unsigned short plus;
  unsigned short space;
  unsigned short prefis;  // #
  unsigned short zero;
  int width;
  _Bool width_asterisk;
  int precision;
  _Bool precision_asterisk;
  _Bool h;
  _Bool l;
  _Bool L;
  _Bool flag_precision;
} flags;

typedef struct {
  int minus;
  int plus;
  int space;
  int asterisk;
  int width;
  char length;
  char specifier;
  void *adress;
  int base;
  int float_stop_rez;
  int flag_for_i;
  int fail;
  int is_width;
  int is_znak;
} sscanf_format;

int s21_sscanf(const char *src, const char *format, ...);
s21_size_t string_to_number(const char *start, int number_of_symbols);
const char *sscanf_length(const char *format, sscanf_format *arguments);
const char *sscanf_width(const char *format, sscanf_format *arguments);
const char *sscanf_flags(const char *format, sscanf_format *arguments);

void write_string(char **string, sscanf_format *arguments, int *res);
void write_character(char **string, sscanf_format *arguments, int *res);
void write_integer(char **string, sscanf_format *arguments, int *res);
void write_i_integer(char **buf, sscanf_format *arguments, int *res);
void write_float(char **buf, sscanf_format *arguments, int *rez);
void write_to_memory(char **buf, sscanf_format *arguments, int *rez,
                     char *start);
void write_unsigned(char **buf, sscanf_format *arguments, int *res);
long long int write_8_16(char **buf, sscanf_format *arguments, int *res);
long double sscanf_atof(char **str, sscanf_format *arguments);
long long int mod_atoi(char **str, sscanf_format *arguments);

void skip_blank_spaces(char **string);
int is_space_enter_tab(char ch);
void sscanf_format_zero(sscanf_format *arguments);

void *s21_memchr(const void *str, int c, s21_size_t n);
int s21_memcmp(const void *str1, const void *str2, s21_size_t n);
void *s21_memcpy(void *dest, const void *src, s21_size_t n);
void *s21_memset(void *str, int c, s21_size_t n);
char *s21_strncat(char *dest, const char *src, s21_size_t n);
char *s21_strchr(const char *str, int c);
int s21_strncmp(const char *str1, const char *str2, s21_size_t n);
char *s21_strncpy(char *dest, const char *src, s21_size_t n);
s21_size_t s21_strcspn(const char *str1, const char *str2);
char *s21_strerror(int errnum);
s21_size_t s21_strlen(const char *str);
char *s21_strpbrk(const char *str1, const char *str2);
char *s21_strrchr(const char *str, int c);
char *s21_strstr(const char *haystack, const char *needle);
char *s21_strtok(char *str, const char *delim);

// bonus 5
void *s21_insert(const char *src, const char *str, s21_size_t start_index);
void *s21_trim(const char *src, const char *trim_chars);
void *s21_to_upper(const char *string);
void *s21_to_lower(const char *string);

////////////////////////////////////////////////////////////////
char *s21_strcpy(char *destination, const char *source);
char *s21_strcat(char *dest, const char *src);
////////////////////////////////////////////////////////////////

// SPRINTF
char *search_one_of(const char symb, char *format_specifiers);
int s21_sprintf(char *to_print, const char *format, ...);
int s_format(va_list *args, char *to_print, flags flags);
int c_format(va_list *args, char *to_print, flags flags);
int d_format(va_list *args, char *buffer, flags flags);
void spec_d(long int n, char *str);
int u_format(va_list *args, char *to_print, flags flags);
void spec_u(unsigned long int n, char *str);
int f_format(va_list *args, char *to_print, flags flags);
void spec_f(long double n, char *str, int kolvo_posle_tochki);
char process_format(const char *format, flags *flags, int *j, int *error);
int choose_format_to_print(char format_spec, va_list *args, flags *flags,
                           int *j, char *to_print);
int define_precision_or_width(const char *format, flags *flags, int *i,
                              int precision_or_width);
void process_flags(char format_spec, va_list *args, char *buff, char *to_print,
                   int *i, flags flags, int if_null_term);
int Ee_format(va_list *args, char *to_print, flags flags, char format_spec);
void spec_Ee(long double n, char *str, int precision, char format_spec);
void Ee_record_conditional(char *tmp, int *base_precision, int *flag);
int o_format(int *j, va_list *args, char *to_print, flags flags);
void spec_o(unsigned long int n, char *str);
int p_format(va_list *args, char *to_print);
void spec_p(void *ptr, char *str);
int Xx_format(int *j, va_list *args, char *to_print, flags flags,
              char format_spec);
void spec_Xx(unsigned long int num, char *str, char format_spec);
int n_format(int *j, va_list *args);
void reverse_str(char *str, int length);
void append_tmp_to_output(char *tmp, char *output);
void to_print_in_number(char *to_print, char *buffer, int *j,
                        int counter_for_printing_form_buff);
double custom_round(long double num, int precision);
int round_up_and_print(char *to_print, char *buff, flags *flags,
                       char format_spec);
int flags_ignor_check(char format_spec, flags flags);
int flags_format_check(char format_spec, flags flags);
int flags_repeat_check(flags flags);
int flags_check(char format_spec, flags flags);

int Gg_format(va_list *args, char *to_print, flags flags, char format_spec);
void spec_Gg(long double n, char *str, int precision, char format_spec);
void remove_zeros_Gg(char *str);
s21_size_t s21_strspn(const char *str1, const char *str2);
#endif
