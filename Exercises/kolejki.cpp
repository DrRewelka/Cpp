#include <iostream>
#include <stdlib.h>

using namespace std;

// Definicja elementu stosu i kolejki
struct elem
{
    int dane;
    elem *nast;
};

void add(elem* &pocz_kolejki, elem* &kon_kolejki, int x)
{
    elem* temp = new elem;
    temp->dane = x;
    if(pocz_kolejki == NULL)
    {
        pocz_kolejki = kon_kolejki = temp;
    }
    else
    {
        kon_kolejki->nast = temp;
        kon_kolejki = kon_kolejki->nast;
    }
}

int next(elem* &pocz_kolejki, elem* &kon_kolejki)
{
    if(pocz_kolejki != NULL)
    {
        elem* last = pocz_kolejki;
        int retValue = pocz_kolejki->dane;
        if(pocz_kolejki == kon_kolejki)
            kon_kolejki = NULL;
        pocz_kolejki = pocz_kolejki->nast;
        delete last;
        return retValue;
    }
    else
        return 0;
}

int firstEl(elem* pocz_kolejki)
{
    if(pocz_kolejki != NULL)
    {
        return pocz_kolejki->dane;
    }
}

bool isEmptyKol(elem* pocz_kolejki)
{
    elem* temp = new elem;
    int checker = 0;
    if(pocz_kolejki == NULL)
    {
        temp = pocz_kolejki;
        while(temp != NULL)
        {
            temp = temp->nast;
            checker++;
        }
        if(checker != 0)
            return false;
        else
            return true;
    }
    else
        return false;
}

int main()
{
    cout << "Kolejka" << endl;
    elem* pocz_kolejki = NULL;
    elem* kon_kolejki = NULL;
    add(pocz_kolejki, kon_kolejki, 1);
    add(pocz_kolejki, kon_kolejki, 5);
    add(pocz_kolejki, kon_kolejki, 8);
    cout << "Poczatek: " << pocz_kolejki->dane << "\nKolejka: " << pocz_kolejki->dane << " - " << pocz_kolejki->nast->dane << " - " << pocz_kolejki->nast->nast->dane << endl;
    cout << next(pocz_kolejki, kon_kolejki) << endl;
    cout << next(pocz_kolejki, kon_kolejki) << endl;
    cout << "Poczatek kolejki: " << firstEl(pocz_kolejki) << endl;
    cout << "Czy jest pusta? " << isEmptyKol(pocz_kolejki) << endl;
    return 0;
}
