#include <iostream>
#include <stdlib.h>

using namespace std;

// Definicja elementu stosu i kolejki
struct elem
{
    int dane;
    elem *nast;
};

// Zad 1 a)

void push(elem* &stos, int x)
{
    elem *temp = new elem;
    temp->dane = x;
    temp->nast = stos;
    stos = temp;
}

// Zad 1 b)

int pop(elem* &stos)
{
    if(stos != NULL)
    {
        elem *last = stos;
        stos = stos->nast;
        return last->dane;
    }
}

// Zad 1 c)

int topEl(elem* stos)
{
    if(stos != NULL)
    {
        return stos->dane;
    }
}

// Zad 1 d)

bool isEmpty(elem* stos)
{
    return stos == NULL;
}

// Zad 2 a)

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

// Zad 3 = SYEUQTSAONIE
// Zad 4 = EA*S*Y** -> ASYE
// Zad 5 - wstawianie: push na pierwszy stos, zdejmowanie: wszystkie elementy z 1 stosu oprócz ostatniego do 2 stosu, zwrócenie
// ostatniego elementu 1 stosu i zdjêcie elementów 2 stosu na 1 stos


int main()
{
    elem* stos = NULL;
    push(stos, 5);
    push(stos, 2);
    push(stos, 1);
    cout << "Stos" << endl;
    cout << stos->dane << endl;
    cout << stos->nast->dane << endl;
    cout << stos->nast->nast->dane << endl;
    cout << pop(stos) << endl;
    cout << topEl(stos) << endl;
    cout << isEmpty(stos) << endl;
    cout << "Kolejka" << endl;
    elem* pocz_kolejki = NULL;
    elem* kon_kolejki = NULL;
    add(pocz_kolejki, kon_kolejki, 1);
    add(pocz_kolejki, kon_kolejki, 5);
    add(pocz_kolejki, kon_kolejki, 8);
    cout << "Poczatek: " << pocz_kolejki->dane << "\nKolejka: " << kon_kolejki->dane << " - " << kon_kolejki->nast->dane << " - " << kon_kolejki->nast->nast->dane << endl;
    cout << next(pocz_kolejki, kon_kolejki) << endl;
    cout << next(pocz_kolejki, kon_kolejki) << endl;
    cout << "Poczatek kolejki: " << firstEl(pocz_kolejki) << endl;
    cout << "Czy jest pusta? " << isEmptyKol(pocz_kolejki) << endl;
    return 0;
}
