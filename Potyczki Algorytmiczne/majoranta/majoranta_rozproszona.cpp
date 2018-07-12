#include "majoranta.h"
#include "message.h"
#include <iostream>
using namespace std;

int main() {
  long long N = LiczbaElementow();
  // Faza pierwsza.
  long long obecna_liczba, ile_wystapien;
  obecna_liczba = 0;
  ile_wystapien = 0;
  long long poczatek = (MyNodeId() * N) / NumberOfNodes();
  long long koniec = ((MyNodeId() + 1) * N) / NumberOfNodes();
  for (long long i = poczatek; i < koniec; ++i) {
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

 if (MyNodeId() > 0) {
    PutLL(0, obecna_liczba);
    PutLL(0, ile_wystapien);
    Send(0);
  } else {
    for (int i = 1; i < NumberOfNodes(); ++i) {
      int instancja = Receive(-1);
      long long otrzymana_liczba = GetLL(instancja);
      long long otrzymane_wystapienia = GetLL(instancja);
      if (otrzymana_liczba == obecna_liczba) {
        ile_wystapien += otrzymane_wystapienia;
      } else {
        if (ile_wystapien > otrzymane_wystapienia) {
          ile_wystapien -= otrzymane_wystapienia;
        } else {
          obecna_liczba = otrzymana_liczba;
          ile_wystapien = otrzymane_wystapienia - ile_wystapien;
        }
      }
    }
  }

  if (MyNodeId() == 0) {
    for (int instancja = 1; instancja < NumberOfNodes(); ++instancja) {
      PutLL(instancja, obecna_liczba);
      Send(instancja);
    }
  } else {
    Receive(0);
    obecna_liczba = GetLL(0);
  }
  ile_wystapien = 0;
  for (long long i = poczatek; i < koniec; ++i) {
    if (DajElement(i) == obecna_liczba) ile_wystapien++;
  }
  if (MyNodeId() > 0) {
    PutLL(0, ile_wystapien);
    Send(0);
  } else {
    for (int i = 1; i < NumberOfNodes(); ++i) {
      int instancja = Receive(-1);
      ile_wystapien += GetLL(instancja);
    }
    if (ile_wystapien * 2 > N) {
      cout << obecna_liczba << endl;
    } else {
      cout << "Nie ma majoranty" << endl;
    }
  }
  return 0;
}
