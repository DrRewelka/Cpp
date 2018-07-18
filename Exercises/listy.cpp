#include <iostream>
#include <stdlib.h>

using namespace std;

//Definicja elementu listy
struct elem
{
    int dane;
    elem *nast;
};

//Operacja wstawienia elementu do listy w celu sprawdzenia dzialania procedury z zadania
void insertFront(int x, elem* &a)
{
    elem* temp = new elem;
    temp->dane = x;
    temp->nast = a;
    a = temp;
}

void reverseList(elem* &a) //odwrocenie listy
{
    elem* temp = new elem;
    elem* temp2 = new elem;
    if(a)
    {
        temp2 = a;
        while(temp2->nast)
        {
            temp = temp2->nast;
            temp2->nast = temp->nast;
            temp->nast = a;
            a = temp;
        }
    }
}

void readList(elem* &a) //czytanie listy
{
    elem* temp = new elem;
    temp = a;
    int i = 0;
    while(temp != NULL)
    {
        cout << "Element " << i++ << ": " << temp->dane << endl;
        temp = temp->nast;
    }
}

int main()
{
    elem* a = NULL;
    insertFront(1, a);
    insertFront(3, a);
    insertFront(5, a);
    insertFront(7, a);
    insertFront(9, a);
    readList(a);
    reverseList(a);
    cout << "Lista odwrocona: " << endl;
    readList(a);
    delete a;
    return 0;
}
