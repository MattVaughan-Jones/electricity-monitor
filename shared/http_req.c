#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *build_headers(char *response, const int len)
{
  char *header_main =
      "HTTP/1.1 200 OK\r\n"
      "Content-Type: text/plain\r\n"
      "Connection: close\r\n"
      "Content-Length:";
  int header_main_len = strlen(header_main);
  int copy_len = len;
  int count_chars_in_len = count_digits(copy_len);
  char *full_msg = malloc(header_main_len + count_chars_in_len + 5 +
                          len); // +5 for \r\n and \0
  char len_as_str[count_chars_in_len];
  sprintf(len_as_str, "%d", len);
  strcat(full_msg, header_main);
  strcat(full_msg, len_as_str);
  strcat(full_msg, "\r\n\r\n");
  strcat(full_msg, response);

  return full_msg;
}
