#include "s21_string.h"

int s21_sscanf(const char *src, const char *format, ...) {
  sscanf_format arguments = {0};
  va_list va;
  char *string = (char *)src;
  va_start(va, format);
  int rez = 0;
  char *start = string;
  arguments.float_stop_rez = 0;
  arguments.flag_for_i = 0;
  while (*format) {
    if (is_space_enter_tab(*format)) {
      while (*(format + 1) != '\0' && is_space_enter_tab(*string)) {
        ++string;
      }
      format++;
      continue;
    }
    if (*format != '%') {
      if (*string == *format) {
        string++;
        format++;
        continue;
      } else {
        break;
      }
    } else {
      format++;
      if (*format == '%') {
        if (*string == '%') {
          string++;
          format++;
          continue;
        } else {
          break;
        }
      }
    }
    format = sscanf_flags(format, &arguments);  // считываем флаги
    format = sscanf_width(format, &arguments);  // считываем ширину
    format = sscanf_length(format, &arguments);  // считываем длину
    arguments.specifier = *format;  // записываем спецификатор

    if (*string == '\0' && arguments.specifier != 'n') {
      arguments.fail = 1;
      break;
    }

    if (arguments.asterisk != -1) arguments.adress = va_arg(va, void *);
    if ((arguments.adress != 0 || arguments.asterisk != -1) &&
        !arguments.fail) {
      write_to_memory(&string, &arguments, &rez, start);
      sscanf_format_zero(&arguments);
    }
    format++;
  }
  va_end(va);
  if (arguments.fail && rez == 0) rez--;
  return rez;
}

void write_string(char **string, sscanf_format *arguments, int *res) {
  char buffer[SIZE] = {'\0'};
  int i = 0;
  while (**string && !is_space_enter_tab(**string) && !arguments->fail) {
    buffer[i] = **string;
    (*string)++;
    i++;
    if ((arguments->width && i >= arguments->width) ||
        is_space_enter_tab(**string))
      break;
  }

  if ((arguments->asterisk) != -1) {
    s21_strcpy((char *)arguments->adress, buffer);
    (*res)++;
  }
}

void write_character(char **string, sscanf_format *arguments, int *res) {
  char symbol;
  symbol = **string;
  if (arguments->width > 0) {
    for (int i = 0; i < arguments->width; i++) {
      (*string)++;
    }
  } else {
    (*string)++;
  }
  if ((arguments->asterisk) != -1) {
    *(char *)arguments->adress = symbol;
    (*res)++;
  }
}

void write_integer(char **string, sscanf_format *arguments, int *res) {
  long long int result = 0;
  int stop_i = 0;
  if (s21_strspn(*string, "0x") == 2) stop_i = 1;
  if (s21_strspn(*string, "0123456789+-")) {
    if ((s21_strspn(*string, "+-") < 2)) {
      result = mod_atoi(string, arguments);
      if (arguments->asterisk != -1) {
        if (arguments->length == 'h') {
          *(short int *)arguments->adress = (short int)result;
        } else if (arguments->length == 'l') {
          *(long int *)arguments->adress = (long int)result;
        } else {
          *(int *)arguments->adress = (int)result;
        }
      }

      if ((arguments->asterisk) != -1 && arguments->flag_for_i != 1 &&
          !arguments->fail) {
        (*res)++;
        if (stop_i == 1) arguments->flag_for_i = 1;
      }
    }
  } else {
    arguments->fail = 1;
  }
}

void write_i_integer(char **string, sscanf_format *arguments, int *res) {
  long long int result = 0;
  int negative = 0;
  if (**string == '-') {
    negative = 1;
    (*string)++;
    arguments->width--;
  } else if (**string == '+') {
    (*string)++;
    arguments->width--;
  }
  if (((s21_strncmp(*string, "0x", 2) == 0) ||
       (s21_strncmp(*string, "0X", 2) == 0)) &&
      (!arguments->is_width || (arguments->width > 1))) {
    arguments->base = 16;
    (*string) += 2;
    arguments->width -= 2;
  } else if (s21_strspn(*string, "0") == 1) {
    arguments->base = 8;
    (*string)++;
    arguments->width--;
  } else if (s21_strspn(*string, "0123456789")) {
    arguments->base = 10;
  }

  if (s21_strchr("xXp", arguments->specifier)) arguments->base = 16;
  if (arguments->specifier == 'o') arguments->base = 8;

  if (arguments->base && **string &&
      s21_strspn(*string, "0123456789abcdefABCDEFxX")) {
    result = write_8_16(string, arguments, res);
    result = negative ? -result : result;
    if (arguments->asterisk != -1) {
      if (arguments->length == 'h') {
        *(short int *)arguments->adress = (short int)result;
      } else if (arguments->length == 'l') {
        *(long int *)arguments->adress = (long int)result;
      } else if (arguments->length == 'L') {
        *(long long int *)arguments->adress = (long long int)result;
      } else {
        *(int *)arguments->adress = (int)result;
      }
    }
  } else
    arguments->fail = 1;
}

void write_float(char **string, sscanf_format *arguments, int *rez) {
  float result_f = 0;
  double result_d = 0;
  long double result_ld = 0;

  if ((s21_strncmp(*string, "inf", 3) == 0) ||
      (s21_strncmp(*string, "INF", 3) == 0)) {
    result_f = INFINITY;
    *(float *)arguments->adress = (float)result_f;
    (*string) += 3;
    *rez += 1;

  } else if ((s21_strncmp(*string, "nan", 3) == 0) ||
             (s21_strncmp(*string, "NAN", 3) == 0)) {
    result_f = NAN;
    *(float *)arguments->adress = (float)result_f;
    (*string) += 3;
    (*rez)++;

  } else if (s21_strspn(*string, ".0123456789+-")) {
    if (arguments->length == 'l') {
      result_d = sscanf_atof(string, arguments);
      if ((arguments->asterisk) != -1 && arguments->float_stop_rez < 2) {
        *(double *)arguments->adress = (double)result_d;
      }
    } else if (arguments->length == 'L') {
      result_ld = sscanf_atof(string, arguments);
      if ((arguments->asterisk) != -1 && arguments->float_stop_rez < 2) {
        *(long double *)arguments->adress = (long double)result_ld;
      }
    } else {
      result_f = sscanf_atof(string, arguments);
      if ((arguments->asterisk) != -1 && arguments->float_stop_rez < 2) {
        *(float *)arguments->adress = (float)result_f;
      }
    }

    if ((arguments->asterisk) != -1 && (arguments->float_stop_rez) != 2) {
      (*rez)++;
    }
  } else {
    arguments->fail = 1;
  }
}

void write_to_memory(char **string, sscanf_format *arguments, int *rez,
                     char *start) {
  switch (arguments->specifier) {
    case 's':
      skip_blank_spaces(string);
      write_string(string, arguments, rez);
      break;

    case 'c':
      write_character(string, arguments, rez);
      break;

    case 'd':
      skip_blank_spaces(string);
      write_integer(string, arguments, rez);
      break;

    case 'i':
      skip_blank_spaces(string);
      write_i_integer(string, arguments, rez);
      break;

    case 'e':
    case 'E':
    case 'g':
    case 'G':
    case 'f':
      skip_blank_spaces(string);
      write_float(string, arguments, rez);
      break;

    case 'u':
      skip_blank_spaces(string);
      write_unsigned(string, arguments, rez);
      break;
    case 'x':
    case 'X':
    case 'p':
      skip_blank_spaces(string);
      write_i_integer(string, arguments, rez);
      break;
    case 'o':
      skip_blank_spaces(string);
      write_i_integer(string, arguments, rez);
      break;
    case 'n':
      *((int *)arguments->adress) = (*string) - start;
      break;
    default:
      arguments->fail = 1;
  }
}

void write_unsigned(char **string, sscanf_format *arguments, int *res) {
  long long int result = 0;
  int stop_i = 0;
  if (s21_strspn(*string, "0x") == 2) stop_i = 1;
  if (s21_strspn(*string, "0123456789+-")) {
    if ((s21_strspn(*string, "+-") < 2)) {
      result = mod_atoi(string, arguments);
      if (arguments->asterisk != -1) {
        if (arguments->length == 'h') {
          *(unsigned short int *)arguments->adress = (short int)result;
        } else if (arguments->length == 'l') {
          *(unsigned long int *)arguments->adress = (long int)result;
        } else {
          *(unsigned int *)arguments->adress = (int)result;
        }
      }

      if ((arguments->asterisk) != -1 && arguments->flag_for_i != 1 &&
          !arguments->fail) {
        (*res)++;
        if (stop_i == 1) arguments->flag_for_i = 1;
      }
    }
  } else
    arguments->fail = 1;
}

long double sscanf_atof(char **string, sscanf_format *arguments) {
  long double result = 0.0;  // записываем реузльтат
  double scale = 1;          // для записи дробной части
  double sign = 1;           // для отрицательного числа
  double sign_of_exp = 1;
  double exp_calc = 0;
  double result_with_exp = 0;

  while (**string == ' ') {
    ++(*string);
  }

  char *start_string = *string;
  if ((!arguments->is_width ||
       (arguments->is_width && ((*string - start_string) < arguments->width))))
    if (**string == '-') {
      sign = -1;
      ++(*string);
    }
  if ((!arguments->is_width ||
       (arguments->is_width && ((*string - start_string) < arguments->width))))
    if (**string == '+') {
      ++(*string);
    }
  if (**string == '.' && arguments->float_stop_rez < 2) {
    arguments->float_stop_rez = 1;
  }
  while (**string && **string >= '0' && **string <= '9' &&
         (!arguments->is_width ||
          (arguments->is_width &&
           ((*string - start_string) < arguments->width)))) {
    result = result * 10 + (**string - '0');
    ++(*string);
  }
  // записываем правую часть от точки
  if ((!arguments->is_width ||
       (arguments->is_width && ((*string - start_string) < arguments->width))))
    if (**string == '.') {
      (*string)++;
      if (s21_strspn(*string, "0123456789") && arguments->float_stop_rez < 2) {
        arguments->float_stop_rez = 0;
      } else if (arguments->float_stop_rez == 1) {
        arguments->float_stop_rez = 2;
      }
      while (**string && **string >= '0' && **string <= '9' &&
             (!arguments->is_width ||
              (arguments->is_width &&
               ((*string - start_string) < arguments->width)))) {
        scale /= 10;
        result = result + (**string - '0') * scale;
        (*string)++;
      }
    }
  if ((!arguments->is_width ||
       (arguments->is_width &&
        ((*string - start_string) < arguments->width)))) {
    if (**string == 'e' || **string == 'E') {
      (*string)++;
      if (!arguments->is_width ||
          (arguments->is_width &&
           ((*string - start_string) < arguments->width))) {
        if (**string == '+') {
          (*string)++;
        } else if (**string == '-') {
          sign_of_exp = -1;
          (*string)++;
        }
      }
      while (**string >= '0' && **string <= '9' &&
             (!arguments->is_width ||
              (arguments->is_width &&
               ((*string - start_string) < arguments->width)))) {
        result_with_exp = result_with_exp * 10 + (**string - '0');
        ++(*string);
      }
      exp_calc += result_with_exp * sign_of_exp;
      while (exp_calc > 0) {
        result *= 10;
        --exp_calc;
      }
      while (exp_calc < 0) {
        result /= 10;
        ++exp_calc;
      }
    }
  }
  return result * sign;
}

long long int mod_atoi(char **string, sscanf_format *arguments) {
  long long int result = 0;
  int sign = 1;
  if ((arguments->width < 0) && arguments->is_width) arguments->fail = 1;
  while (**string == ' ') (*string)++;
  if (**string == '-') {
    (*string)++;
    sign = -1;
    arguments->width--;
  }
  if (**string == '+') {
    (*string)++;
    arguments->width--;
  }
  if (arguments->is_width) {
    if (arguments->width > 0) {
      for (int i = 0;
           (i < arguments->width) && (**string >= '0' && **string <= '9');
           i++) {
        result = result * 10 + (**string - '0');
        (*string)++;
      }
    }
  } else {
    while (**string && **string >= '0' && **string <= '9') {
      result = result * 10 + (**string - '0');
      (*string)++;
    }
  }
  result *= sign;
  return result;
}

int is_space_enter_tab(char symbol) {
  int flag = 0;
  if (symbol == ' ' || symbol == '\n' || symbol == '\t') flag = 1;
  return flag;
}

long long int write_8_16(char **string, sscanf_format *arguments, int *res) {
  unsigned long long int result = 0;
  int var, size = 0;
  char dict[] = "0123456789ABCDEF";
  char symb = 0;
  size = s21_strspn(*string, "0123456789abcdefABCDEF");
  if (size) {
    if (!arguments->is_width) arguments->width = size;
    while (arguments->width > 0) {
      if (**string >= 'a' && **string <= 'f')
        symb = **string - 32;
      else
        symb = **string;
      (*string)++;
      var = (int)(s21_strchr(dict, symb) - dict);
      result = result * arguments->base + var;
      arguments->width--;
    }
  } else {
    arguments->fail = 1;
  }
  (*res)++;
  return result;
}

const char *sscanf_length(const char *format, sscanf_format *arguments) {
  switch (*format) {
    case 'h':
      arguments->length = 'h';
      ++format;
      break;
    case 'l':
      arguments->length = 'l';
      ++format;
      break;
    case 'L':
      arguments->length = 'L';
      ++format;
      break;
  }
  return format;
}

const char *sscanf_width(const char *format, sscanf_format *arguments) {
  if (*format >= '0' && *format <= '9') {
    int long_width = s21_strspn(format, "1234567890");
    if (long_width) arguments->is_width = 1;
    arguments->width = string_to_number(format, long_width);
    format += long_width;
  }
  return format;
}

const char *sscanf_flags(const char *format, sscanf_format *arguments) {
  if (*format == ' ' || *format == '+' || *format == '-' || *format == '*') {
    if (*format == '*') arguments->asterisk = -1;
    ++format;
  }
  return format;
}

void sscanf_format_zero(sscanf_format *arguments) {
  arguments->width = 0;
  arguments->specifier = 0;
  arguments->length = 0;
  arguments->minus = 0;
  arguments->asterisk = 0;
  arguments->plus = 0;
  arguments->space = 0;
  arguments->is_width = 0;
  arguments->base = 0;
  arguments->flag_for_i = 0;
  arguments->is_znak = 0;
  arguments->adress = 0;
  arguments->float_stop_rez = 0;
}

void skip_blank_spaces(char **string) {
  while (is_space_enter_tab(**string)) {
    ++(*string);
  }
}

s21_size_t string_to_number(const char *start, int number_of_symbols) {
  s21_size_t res = 0;
  start += number_of_symbols - 1;
  int i = 0;
  while (number_of_symbols-- > 0) {
    res += (s21_size_t)pow(10, i) * (int)(*start - 48);
    i++;
    start--;
  }
  return res;
}
