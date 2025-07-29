#include <math.h>
#include <stdlib.h>

int count_digits(const int num) {
  if (num < 0)
    return -1;
  if (num == 0)
    return 1;
  return (int)ceil(log10(abs(num))) + 1;
}