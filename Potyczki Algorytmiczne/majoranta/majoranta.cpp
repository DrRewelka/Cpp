#include "majoranta.h"
#include <iostream>
using namespace std;

int main() {
  long long N = LiczbaElementow();
  // Faza pierwsza.
  long long obecna_liczba, ile_wystapien;
  obecna_liczba = 0;
  ile_wystapien = 0;
  for (long long i = 0; i < N; ++i) {
    long long kolejna = DajElement(i);
    if (kolejna == obecna_liczba) {
      ile_wystapien += 1;
    } else {
      if (ile_wystapien > 0) {
        ile_wystapien -= 1;
      } else {
        obecna_liczba = kolejna;
        ile_wystapien = 1;
      }
    }
  }
  // Faza druga
  ile_wystapien = 0;
  for (long long i = 0; i < N; ++i) {
    if (DajElement(i) == obecna_liczba) ile_wystapien++;
  }
  if (ile_wystapien * 2 > N) {
    cout << obecna_liczba << endl;
  } else {
    cout << "Nie ma majoranty" << endl;
  }
  return 0;
}
