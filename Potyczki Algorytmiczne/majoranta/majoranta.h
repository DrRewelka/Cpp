#ifndef MAJORANTA_H
#define MAJORANTA_H

#include <stdio.h>

long long LiczbaElementow() {
  static long long liczba_elementow = -1;
  if (liczba_elementow < 0)
    scanf("%lld", &liczba_elementow);
  return liczba_elementow;
}

long long DajElement(long long k) {
  if (k < LiczbaElementow() / 2)
    return k;
  else
    return LiczbaElementow() / 2;
}

#endif
