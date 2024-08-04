#include "s21_string.h"

int s21_sprintf(char *to_print, const char *format, ...) {
  setlocale(LC_ALL, "en_US.UTF-8");
  va_list args;
  va_start(args, format);
  int i = 0, j = 0, flag = 0, error = 0;
  while (format[i] != '\0') {
    flag = 0;
    flags current_flags = {0};
    if (format[i] == '%') {
      if (format[i + 1] == '%') {
        to_print[j] = '%';
        i += 2;
        j++;
        continue;
      }
      char current_spec = process_format(format, &current_flags, &i, &error);
      if (error) {
        printf("Error: Invalid format specifier\n");
        va_end(args);
        exit(1);
      }
      choose_format_to_print(current_spec, &args, &current_flags, &j, to_print);
      flag = 1;
    } else
      to_print[j] = format[i];
    i++;
    if (!flag) j++;
  }
  to_print[j] = '\0';
  va_end(args);
  return j;
}

char process_format(const char *format, flags *flags, int *j, int *error) {
  int i = *j + 1, flag = 0, no_numbers_left = 0, flag_width_asterics = 0,
      cycle_should_end = 0;
  char format_specifiers[] = "cdieEfgGosuxXpn";
  char *formatspec = NULL;
  while ((formatspec = search_one_of(format[i], format_specifiers)) == NULL) {
    if (format[i] == '\0' || no_numbers_left || flag_width_asterics ||
        cycle_should_end) {  // после того , как загребли числа , у нас должен
                             // пойти спецификатор формата, значит мы должны
                             // выйти из цикла
      *error = 1;
      return '\0';
    }
    int precision_or_width = 0;
    if (format[i] == '*') flags->width_asterisk = 1;
    if (flags->width_asterisk && !flag_width_asterics) {
      flag_width_asterics = 1;
      if (search_one_of(format[i + 1], format_specifiers) == NULL) {
        i++;
        flag = 1;
      }
    }
    if (format[i] == '.') {
      flags->flag_precision = 1;
      if (format[i + 1] == '*') {
        flags->precision_asterisk = 1;
        i += 2;
        flag = 1;
      } else {
        precision_or_width = 1;
        flag = define_precision_or_width(format, flags, &i, precision_or_width);
      }
      no_numbers_left = 1;
      flag_width_asterics = 0;
    }
    if (format[i] >= 49 && format[i] <= 57 && !no_numbers_left &&
        !flag_width_asterics) {
      flag = define_precision_or_width(format, flags, &i, precision_or_width);
      if (format[i] != '.') no_numbers_left = 1;
    }
    if (format[i] == '0') flags->zero += 1;
    if (format[i] == '-') flags->minus += 1;
    if (format[i] == '+') flags->plus += 1;
    if (format[i] == ' ') flags->space += 1;
    if (format[i] == '#') flags->prefis += 1;
    if (format[i] == '*') flags->width_asterisk = 1;

    if (format[i] == 'h') {
      flags->h = 1;
      flag = 0;
      flag_width_asterics = 0;
      cycle_should_end = 1;
    }
    if (format[i] == 'l') {
      flags->l = 1;
      flag = 0;
      flag_width_asterics = 0;
      cycle_should_end = 1;
    }
    if (format[i] == 'L') {
      flags->L = 1;
      flag = 0;
      flag_width_asterics = 0;
      cycle_should_end = 1;
    }
    if (!flag) i++;
  }
  *j = i;

  return *formatspec;
}

char *search_one_of(const char symb, char *format_specifiers) {
  char *search = NULL;
  int i = 0;
  while (format_specifiers[i] != '\0') {
    if (symb == format_specifiers[i]) {
      search = &format_specifiers[i];
    }
    i++;
  }
  return search;
}

int define_precision_or_width(const char *format, flags *flags, int *i,
                              int precision_or_width) {
  int lower_boundary = 48, j = *i, index = 0;
  if (precision_or_width) {
    lower_boundary = 47;
    j += 1;
  }
  char t_buffer[SIZE] = {'\0'};
  while (format[j] >= lower_boundary && format[j] <= 57)
    t_buffer[index++] = format[j++];
  if (precision_or_width)
    flags->precision = atoi(t_buffer);
  else
    flags->width = atoi(t_buffer);
  *i = j;
  return 1;
}

int choose_format_to_print(char format_spec, va_list *args, flags *flags,
                           int *j, char *to_print) {
  int error = 0;
  char buffer[SIZE] = {"\0"};

  if (flags->width_asterisk) flags->width = va_arg(*args, int);
  if (flags->precision_asterisk) flags->precision = va_arg(*args, int);
  int input = 1;
  switch (format_spec) {
    case 'c':
      error = c_format(args, buffer, *flags);
      break;
    case 's':
      error = s_format(args, buffer, *flags);
      break;
    case 'i':
    case 'd':
      error = d_format(args, buffer, *flags);
      break;
    case 'u':
      error = u_format(args, buffer, *flags);
      break;
    case 'f':
      error = f_format(args, buffer, *flags);
      break;
    case 'e':
    case 'E':
      error = Ee_format(args, buffer, *flags, format_spec);
      break;
    case 'g':
    case 'G':
      error = Gg_format(args, buffer, *flags, format_spec);
      break;
    case 'o':
      error = o_format(&input, args, buffer, *flags);
      break;
    case 'p':
      error = p_format(args, buffer);
      break;
    case 'n':
      error = n_format(j, args);
      break;
    case 'x':
    case 'X':
      error = Xx_format(&input, args, buffer, *flags, format_spec);
      break;
    default:
      printf("Poshel nax idi formati delay chtobi ih vvodit!\n");
      break;
  }
  if (!input) flags->prefis = 0;
  process_flags(format_spec, args, buffer, to_print, j, *flags, error);
  return error;
}

int c_format(va_list *args, char *buffer, flags flags) {
  int err = 0, cnt = 0, arg = 0;
  if (flags.l) {
    arg = (wchar_t)va_arg(*args, int);
    wctomb(buffer, arg);
  } else {
    arg = (char)va_arg(*args, int);
    buffer[cnt] = arg;
  }
  if (arg == '\0') err = 1;
  return err;
}

int s_format(va_list *args, char *buffer, flags flags) {
  int err = 0, cnt = 0;
  void *arg = NULL;
  if (flags.l) {
    arg = (wchar_t *)va_arg(*args, wchar_t *);
    wcstombs(buffer, arg, SIZE);
  } else {
    arg = (char *)va_arg(*args, char *);
    char *to_be_printed = (char *)arg;
    while (to_be_printed[cnt] != '\0') {
      buffer[cnt] = to_be_printed[cnt];
      cnt++;
    }
  }

  if (arg == NULL) {
    err = 1;
    return err;
  }
  return err;
}

int d_format(va_list *args, char *buffer, flags flags) {
  char tmp[SIZE];
  int err = 0;
  int cnt = 0;
  long int arg = va_arg(*args, long int);
  if (flags.h)
    arg = (short int)arg;
  else if (!flags.l)
    arg = (int)arg;
  spec_d(arg, tmp);
  if (tmp[0] == '0' && flags.flag_precision && !flags.precision) tmp[0] = '\0';
  while (tmp[cnt] != '\0') {
    buffer[cnt] = tmp[cnt];
    cnt++;
  }
  return err;
}

void spec_d(long int n, char *str) {
  int i = 0;
  long int chislo;
  chislo = n;
  if (n < 0) n = -n;

  do {
    str[i++] = n % 10 + '0';
  } while ((n /= 10) > 0);

  if (chislo < 0) str[i++] = '-';
  str[i] = '\0';

  int j = 0;
  int k = i - 1;
  while (k > j) {
    char tmp = str[j];
    str[j] = str[k];
    str[k] = tmp;
    j++;
    k--;
  }
}

int u_format(va_list *args, char *buffer, flags flags) {
  char tmp[SIZE];
  int err = 0;
  int cnt = 0;
  unsigned long int arg = va_arg(*args, unsigned long int);
  if (flags.h) arg = (unsigned short int)arg;
  if (!flags.l) arg = (unsigned int)arg;
  spec_u(arg, tmp);
  while (tmp[cnt] != '\0') {
    buffer[cnt] = tmp[cnt];
    cnt++;
  }
  return err;
}

void spec_u(unsigned long int n, char *str) {
  int i = 0;
  do {
    str[i++] = n % 10 + '0';
  } while ((n /= 10) > 0);
  str[i] = '\0';

  int j = 0;
  int k = i - 1;
  while (k > j) {
    char tmp = str[j];
    str[j] = str[k];
    str[k] = tmp;
    j++;
    k--;
  }
}

int f_format(va_list *args, char *buffer, flags flags) {
  char tmp[SIZE];
  int err = 0;
  int cnt = 0;
  long double arg = 0;
  int base_precision = 6;
  if (flags.L)
    arg = va_arg(*args, long double);
  else {
    arg = va_arg(*args, double);
    arg = (double)arg;
  }
  if (flags.flag_precision) {
    arg = custom_round(arg, flags.precision);
    base_precision = flags.precision;
  } else
    arg = custom_round(arg, base_precision);
  spec_f(arg, tmp, base_precision);
  while (tmp[cnt] != '\0') {
    buffer[cnt] = tmp[cnt];
    cnt++;
  }
  return err;
}

double custom_round(long double num, int precision) {
  double factor = pow(10, precision);
  return (round(num * factor) / factor);
}
void spec_f(long double n, char *str, int kolvo_posle_tochki) {
  int k;
  if ((n == 0.0) && (k = signbit(n)) && k != 0) {
    s21_strcpy(str, "-0.");
    for (int i = 0; i < kolvo_posle_tochki; i++) {
      s21_strcat(str, "0");
    }
    return;
  }

  int if_negativ = 0;
  if (n < 0) {
    if_negativ = 1;
    n = -n;
  }

  if ((n == 0) && (k = signbit(n)) && k != 0) {
    if_negativ = 1;
    n = -n;
  }
  double okrugl = 0.1;
  for (int i = 0; i < kolvo_posle_tochki; i++) {
    okrugl /= 10.0;
  }
  n += okrugl;

  long long int do_tochki = (long long int)n;
  long double posle_tochki = n - do_tochki;

  char int_str[100];
  spec_d(do_tochki, int_str);

  int i = 0;
  if (if_negativ == 1) {
    str[i++] = '-';
  }

  int j = 0;
  while (int_str[j] != '\0') {
    str[i] = int_str[j];
    i++;
    j++;
  }

  str[i++] = '.';

  for (int p = 0; p < kolvo_posle_tochki; p++) {
    posle_tochki *= 10;
    long long int chislo = (long long int)posle_tochki;
    str[i++] = chislo + '0';
    posle_tochki -= chislo;
  }

  str[i] = '\0';
}

int Ee_format(va_list *args, char *buffer, flags flags, char format_spec) {
  char tmp[SIZE];
  int err = 0;
  int cnt = 0;
  long double arg = 0;
  int base_precision = 6;
  int add_precision = 2;
  if (flags.L)
    arg = va_arg(*args, long double);
  else {
    arg = va_arg(*args, double);
    arg = (double)arg;
  }

  if (flags.flag_precision) base_precision = flags.precision;
  spec_Ee(arg, tmp, base_precision + add_precision, format_spec);
  int flag = 0;
  if (tmp[base_precision + add_precision] >= '5' &&
      tmp[base_precision + add_precision] <= '9') {
    while (!flag) {
      Ee_record_conditional(tmp, &base_precision, &flag);
    }
  }
  while (tmp[cnt] != '\0') {
    buffer[cnt] = tmp[cnt];
    cnt++;
  }
  return err;
}

void spec_Ee(long double n, char *str, int precision, char format_spec) {
  int exponent = 0;
  char sign = '+';
  char buffer[SIZE];
  int i = 0, j = 0;

  if (n == 0.0) {
    str[i++] = '0';
    str[i++] = '.';
    while (j < precision - 1) {
      str[i++] = '0';
      j++;
    }
    str[i++] = format_spec == 'e' ? 'e' : 'E';
    str[i++] = '+';
    str[i++] = '0';
    str[i++] = '0';
    str[i] = '\0';
    return;
  }

  if (n < 0) {
    n = -n;
    str[i++] = '-';
  }

  while (n >= 10.0) {
    n /= 10.0;
    exponent++;
  }
  while (n < 1.0 && n != 0.0) {
    n *= 10.0;
    exponent--;
  }

  if (exponent < 0) {
    sign = '-';
    exponent = -exponent;
  }

  int ip = (int)n;
  do {
    buffer[j++] = ip % 10 + '0';
  } while ((ip /= 10) > 0);

  for (int k = 0; k < j / 2; k++) {
    char tmp = buffer[k];
    buffer[k] = buffer[j - k - 1];
    buffer[j - k - 1] = tmp;
  }

  for (int k = 0; k < j; k++) {
    str[i++] = buffer[k];
  }
  str[i++] = '.';

  n -= (int)n;  // Remove the integer part
  j = 0;        // Reset buffer index for fractional part
  for (int k = 0; k < precision; k++) {
    n *= 10;
    int digit = (int)n % 10;
    buffer[j++] = digit + '0';
    n -= digit;  // Remove the processed digit
  }

  for (int k = 0; k < j; k++) {
    str[i++] = buffer[k];
  }

  str[i++] = format_spec == 'e' ? 'e' : 'E';
  str[i++] = sign;
  str[i++] = (exponent / 10) + '0';
  str[i++] = (exponent % 10) + '0';
  str[i] = '\0';
}

char *s21_strcpy(char *destination, const char *source) {
  char *current = destination;
  while (*source != '\0') {
    *current = *source;

    source++;
    current++;
  }
  *current = '\0';
  return destination;
}

char *s21_strcat(char *dest, const char *src) {
  s21_strcpy(dest + s21_strlen(dest), src);
  return dest;
}

void Ee_record_conditional(char *tmp, int *base_precision, int *flag) {
  if (tmp[*base_precision + 1] != '9') {
    tmp[*base_precision + 1] += 1;
    *flag = 1;
  } else {
    while (tmp[*base_precision + 1] == '9') {
      tmp[*base_precision + 1] = '0';
      *base_precision -= 1;
    }
    if (tmp[*base_precision + 1] != '.')
      tmp[*base_precision + 1] += 1;
    else if (tmp[*base_precision] != '9' && tmp[*base_precision != '.'])
      tmp[*base_precision] += 1;
    else {
      int cnt = 0;
      tmp[*base_precision] = '1';
      while (tmp[cnt] != '+' && tmp[cnt] != '-') cnt++;
      if (tmp[cnt + 2] != '9')
        tmp[cnt + 2] += 1;
      else {
        if (tmp[cnt + 1] == '0')
          tmp[cnt + 1] = '1';
        else
          tmp[cnt + 1] += 1;
      }
    }
    *flag = 1;
  }
}

int Gg_format(va_list *args, char *buffer, flags flags, char format_spec) {
  char tmp[SIZE];
  int err = 0;
  long double arg = 0;
  int base_precision = 4;
  if (flags.L)
    arg = va_arg(*args, long double);
  else {
    arg = va_arg(*args, long double);
    arg = (double)arg;
  }
  if (flags.flag_precision) base_precision = flags.precision;
  if ((arg > 0 && arg <= 0.0001) || (arg >= -0.0001 && arg < 0) ||
      arg >= 100000 || arg <= -100000) {
    spec_Gg(arg, tmp, base_precision, format_spec);
  } else {
    spec_f(arg, tmp, base_precision);
    remove_zeros_Gg(tmp);
  }
  append_tmp_to_output(tmp, buffer);
  return err;
}

void spec_Gg(long double n, char *str, int precision, char format_spec) {
  int exponent = 0;
  char sign = '+';
  char buffer[SIZE];
  int i = 0;

  if (n < 0) {
    n = -n;
    *str++ = '-';
  }

  while (n < 1.0) {
    n *= 10.0;
    exponent--;
  }

  if (exponent < 0) {
    sign = '-';
    exponent = -exponent;
  }

  spec_f(n, buffer, precision);
  remove_zeros_Gg(buffer);
  i = 0;
  while (buffer[i] != '\0') {
    str[i] = buffer[i];
    i++;
  }

  if (format_spec == 'g') {
    str[i++] = 'e';
  } else if (format_spec == 'G') {
    str[i++] = 'E';
  }
  str[i++] = sign;

  str[i++] = (exponent / 10) + '0';
  str[i++] = (exponent % 10) + '0';

  str[i] = '\0';
}

void remove_zeros_Gg(char *str) {
  int len = s21_strlen(str);

  while (len > 0 && str[len - 1] == '0') {
    len--;
  }

  if (len > 0 && str[len - 1] == '.') {
    len--;
  }

  str[len] = '\0';
}

void append_tmp_to_output(char *tmp, char *output) {
  int cnt = 0;
  while (tmp[cnt] != '\0') {
    output[cnt] = tmp[cnt];
    cnt++;
  }
}

int o_format(int *j, va_list *args, char *buffer, flags flags) {
  int err = 0;
  char tmp[SIZE];
  int cnt = 0;
  unsigned long int arg = va_arg(*args, unsigned long int);
  if (flags.h)
    arg = (unsigned short int)arg;
  else if (!flags.l)
    arg = (unsigned int)arg;
  *j = arg;
  spec_o(arg, tmp);
  while (tmp[cnt] != '\0') {
    buffer[cnt] = tmp[cnt];
    cnt++;
  }
  return err;
}

void spec_o(unsigned long int n, char *str) {
  int i = 0;
  if (n == 0) {
    str[i++] = '0';
  } else {
    while (n != 0) {
      long int chislo = n % 8;
      str[i++] = chislo + '0';
      n /= 8;
    }
  }
  str[i] = '\0';

  int j = 0;
  int k = i - 1;
  while (k > j) {
    char tmp = str[j];
    str[j] = str[k];
    str[k] = tmp;
    j++;
    k--;
  }
}

int p_format(va_list *args, char *buffer) {
  int err = 0;
  char tmp[SIZE];
  int cnt = 0;
  void *arg = va_arg(*args, void *);
  spec_p(arg, tmp);
  while (tmp[cnt] != '\0') {
    buffer[cnt] = tmp[cnt];
    cnt++;
  }
  return err;
}

void spec_p(void *ptr, char *str) {
  unsigned long long address = (unsigned long long)ptr;
  int i = 0;

  do {
    int ch = address % 16;
    if (ch < 10) {
      str[i++] = ch + '0';
    } else {
      str[i++] = ch - 10 + 'a';
    }
    address /= 16;
  } while (address != 0);

  str[i++] = 'x';
  str[i++] = '0';

  int j = 0;
  int k = i - 1;
  while (k > j) {
    char tmp = str[j];
    str[j] = str[k];
    str[k] = tmp;
    j++;
    k--;
  }

  str[i] = '\0';
}

int n_format(int *j, va_list *args) {
  int err = 0;
  int chars_printed = *j;
  *j = chars_printed;
  *va_arg(*args, int *) = chars_printed;
  return err;
}

int Xx_format(int *j, va_list *args, char *buffer, flags flags,
              char format_spec) {
  char tmp[SIZE];
  int err = 0;
  unsigned long int arg = va_arg(*args, unsigned long int);
  if (flags.h)
    arg = (unsigned short int)arg;
  else if (!flags.l)
    arg = (unsigned int)arg;
  if (arg == 0) {
    *j = arg;
    buffer[0] = '0';
  } else {
    spec_Xx(arg, tmp, format_spec);

    append_tmp_to_output(tmp, buffer);
  }
  return err;
}

void spec_Xx(unsigned long int num, char *str, char format_spec) {
  int i = 0;
  while (num > 0) {
    long int ostatok = num % 16;
    if (ostatok < 10) {
      str[i++] = ostatok + '0';
    } else if (format_spec == 'X' && ostatok >= 10) {
      str[i++] = ostatok - 10 + 'A';
    } else if (format_spec == 'x' && ostatok >= 10) {
      str[i++] = ostatok - 10 + 'a';
    }
    num /= 16;
  }
  str[i] = '\0';
  reverse_str(str, i);
}

void reverse_str(char *str, int length) {
  int j = 0;
  int k = length - 1;
  while (k > j) {
    char tmp = str[j];
    str[j] = str[k];
    str[k] = tmp;
    j++;
    k--;
  }
}

void process_flags(char format_spec, va_list *args, char *buff, char *to_print,
                   int *i, flags flags, int if_null_term) {
  char buffer[SIZE] = {'\0'};
  if (flags_check(format_spec, flags)) {
    va_end(*args);
    exit(1);
  }
  round_up_and_print(buffer, buff, &flags, format_spec);
  int if_positive_or_negative = 1, counter_for_printing_from_buf = 0,
      substract_to_found_witdth = s21_strlen(buffer), j = *i;
  char fill_in = ' ';
  int counter_of_width = flags.width - substract_to_found_witdth;
  if (buff[0] == '-') if_positive_or_negative = 0;
  if (format_spec == 'c' && if_null_term) {
    to_print[j++] = '\0';
    counter_of_width -= 1;
  }
  if (counter_of_width > 1 && !flags.zero && !flags.minus) {
    while (counter_of_width > 1) {
      to_print[j++] = fill_in;
      counter_of_width -= 1;
    }
  }
  if (flags.plus || flags.space) {
    if (flags.plus) flags.space = 0;
    char fill_in = '+';
    if (flags.space) fill_in = ' ';
    if (if_positive_or_negative) {
      to_print[j++] = fill_in;
      counter_of_width -= 1;
    }
  }
  if (flags.zero) {
    if (!if_positive_or_negative) {
      to_print[j++] = '-';
      counter_for_printing_from_buf += 1;
    }
  }
  if (counter_of_width > 0 && !flags.minus) {
    if (flags.zero) fill_in = '0';
    while (counter_of_width > 0) {
      to_print[j++] = fill_in;
      counter_of_width -= 1;
    }
  }
  to_print_in_number(to_print, buffer, &j, counter_for_printing_from_buf);
  if (counter_of_width > 0 && flags.minus && !flags.zero) {
    while (counter_of_width > 0) {
      to_print[j++] = fill_in;
      counter_of_width -= 1;
    }
  }
  to_print[j] = '\0';
  *i = j;
}

int flags_check(char format_spec, flags flags) {
  int ignor = flags_ignor_check(format_spec, flags);
  if (ignor) printf("Error: ignor flags");
  int conflict = flags_format_check(format_spec, flags);
  if (conflict && ignor)
    printf("\nError: flags and format conflict");
  else if (conflict && !ignor)
    printf("Error: flags and format conflict");
  int repeat = flags_repeat_check(flags);
  if (ignor + conflict > 0 && repeat)
    printf("\nError: repeated flags");
  else if (ignor + conflict <= 0 && repeat)
    printf("Error: repeated flags");
  return ignor + conflict + repeat;
}

int flags_ignor_check(char format_spec, flags flags) {
  int ignor = 0;

  char *format_zero_ignor = "diouxX";
  ;
  if ((flags.plus && flags.space) || (flags.minus && flags.zero) ||
      ((flags.flag_precision &&
        search_one_of(format_spec, format_zero_ignor)) &&
       flags.zero) ||
      (flags.h && (flags.l || flags.L)) || (flags.l && (flags.h || flags.L)) ||
      (flags.L && (flags.h || flags.l))) {
    ignor = 1;
  }
  return ignor;
}

int flags_format_check(char format_spec, flags flags) {
  int conflict = 0;
  char unavailable_format_type1[] = "cosuxXpn";
  char unavailable_format_type2[] = "n";
  char unavailable_format_type3[] = "cdisupn";
  char unavailable_format_type4[] = "cspn";
  char unavailable_format_type5[] = "cpn";
  char unavailable_format_type6[] = "ceEfgGsp";
  char unavailable_format_type7[] = "p";
  if (((flags.plus || flags.space) &&
       search_one_of(format_spec, unavailable_format_type1)) ||
      ((flags.minus || flags.width) &&
       search_one_of(format_spec, unavailable_format_type2)) ||
      (flags.prefis && search_one_of(format_spec, unavailable_format_type3)) ||
      ((flags.zero || flags.L) &&
       search_one_of(format_spec, unavailable_format_type4)) ||
      (flags.precision &&
       search_one_of(format_spec, unavailable_format_type5)) ||
      (flags.h && search_one_of(format_spec, unavailable_format_type6)) ||
      (flags.l && search_one_of(format_spec, unavailable_format_type7))) {
    conflict = 1;
  }
  return conflict;
}

int flags_repeat_check(flags flags) {
  int repeat = 0;
  if ((flags.minus > 1) || (flags.plus > 1) || (flags.space > 1) ||
      (flags.prefis > 1) || (flags.zero > 1))
    repeat = 1;

  return repeat;
}

int round_up_and_print(char *to_print, char *buff, flags *flags,
                       char format_spec) {
  int counter_for_printing_from_buf = 0, j = 0,
      init_precision = flags->precision, flag_precision = 0, comp = 0,
      flag_for_print_precision = 0, flag_point = 0;
  int workout_integral = 0, length_of_integral = 0, if_integral_negative = 0,
      worked_out = 0, cut_str = __INT32_MAX__;
  if (flags->prefis &&
      (format_spec == 'o' || format_spec == 'x' || format_spec == 'X')) {
    to_print[j++] = '0';
    if (format_spec == 'x') to_print[j++] = 'x';
    if (format_spec == 'X') to_print[j++] = 'X';
  }
  if (format_spec == 'e' || format_spec == 'E') comp = -1;
  if (flags->flag_precision &&
      (format_spec == 'f' || format_spec == 'e' || format_spec == 'E'))
    flag_precision = 1;
  if (!init_precision && flag_precision) comp += 1;
  if (!flag_precision && flags->flag_precision) {
    length_of_integral = s21_strlen(buff);
    if (buff[0] == '-') {
      length_of_integral -= 1;
      if_integral_negative = 1;
    }
    workout_integral = 1;
  }
  if (format_spec == 's' && flags->flag_precision) cut_str = flags->precision;
  if (format_spec == 'G' || format_spec == 'g') {
    if (!flags->flag_precision) {
      cut_str = 6;
    } else
      cut_str = flags->precision;

    if (buff[0] == '-') cut_str += 1;
    if (buff[0] == '0' || (buff[0] == '-' && buff[1] == '0')) cut_str += 1;
  }
  if ((format_spec == 'e' || format_spec == 'E') && !flags->flag_precision) {
    flags->precision = 6;
    flag_precision = 1;
  }

  while (buff[counter_for_printing_from_buf] != '\0' &&
         flag_for_print_precision != 1 &&
         counter_for_printing_from_buf < cut_str) {
    if (workout_integral && !worked_out) {
      if (if_integral_negative) {
        to_print[j++] = '-';
        counter_for_printing_from_buf += 1;
      }
      while (flags->precision > length_of_integral) {
        to_print[j++] = '0';
        flags->precision -= 1;
      }
      worked_out = 1;
    }
    if ((format_spec == 'g' || format_spec == 'G') &&
        buff[counter_for_printing_from_buf] == '.')
      cut_str += 1;
    if (buff[counter_for_printing_from_buf] == '.' && flag_precision) {
      if (format_spec == 'f' && !init_precision && flags->flag_precision &&
          !flags->prefis)
        flag_point = 1;
      while (flags->precision > comp &&
             buff[counter_for_printing_from_buf + 1] != '\0') {
        to_print[j] = buff[counter_for_printing_from_buf];
        counter_for_printing_from_buf += 1;
        j += 1;
        flags->precision -= 1;
      }
      flag_for_print_precision = 1;
    }
    if (flag_for_print_precision &&
        (format_spec == 'e' || format_spec == 'E')) {
      char while_search;
      if (format_spec == 'e') while_search = 'e';
      if (format_spec == 'E') while_search = 'E';
      if (flags->prefis && !search_one_of('.', to_print)) to_print[j++] = '.';
      while (buff[counter_for_printing_from_buf] != while_search &&
             buff[counter_for_printing_from_buf + 1] != '\0')
        counter_for_printing_from_buf++;
      flag_for_print_precision = 0;
    }
    if (!flag_point) to_print[j] = buff[counter_for_printing_from_buf];
    counter_for_printing_from_buf++;
    j++;
  }
  return flag_precision;
}

void to_print_in_number(char *to_print, char *buffer, int *j,
                        int counter_for_printing_form_buff) {
  int i = *j;
  while (buffer[counter_for_printing_form_buff] != '\0') {
    to_print[i++] = buffer[counter_for_printing_form_buff++];
  }
  *j = i;
}