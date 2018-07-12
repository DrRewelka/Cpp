#include <iostream>
#include <stdio.h>
#include <math.h>

using namespace std;

struct qiu
{
    int dane;
    qiu *nast;
};

struct elem
{
    int dane;
    elem *nast;
};

void push(elem* &stos, int x)
{
    elem *temp = new elem;
    temp->dane = x;
    temp->nast = stos;
    stos = temp;
}

void addToQueue(int x, qiu* &pocz_kolejki, qiu* &kon_kolejki)
{
    qiu* temp = new qiu;
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

int readFromQueue(qiu* pocz_kolejki)
{
    if(pocz_kolejki != NULL)
        return pocz_kolejki->dane;
}

int pop(elem* &stos)
{
    if(stos != NULL)
    {
        elem *last = stos;
        stos = stos->nast;
        return last->dane;
    }
}

int main()
{
    qiu* pocz_kolejki = NULL;
    qiu* kon_kolejki = NULL;
    elem* stos = NULL;
    long long n, t;
    int a;
    scanf("%llu %llu", &n, &t);

    if(t%2 == 0)
    {
        for(long long i = 0ll; i < pow(2, n); i++)
        {
            if(i != pow(2, n) - 1)
                scanf("%u ", &a);
            else
                scanf("%u", &a);
            addToQueue(a, pocz_kolejki, kon_kolejki);
        }
    }
    else
    {
        for(long long i = 0ll; i < pow(2, n); i++)
        {
            if(i != pow(2, n) - 1)
                scanf("%u ", &a);
            else
                scanf("%u", &a);
            push(stos, a);
        }
    }
    if(t%2 == 0)
    {
        while(pocz_kolejki != NULL)
        {
            cout << readFromQueue(pocz_kolejki) << " ";
            pocz_kolejki = pocz_kolejki->nast;
        }
    }
    else
    {
        while(stos != NULL)
            cout << pop(stos) << " ";
    }
    return 0;
}
