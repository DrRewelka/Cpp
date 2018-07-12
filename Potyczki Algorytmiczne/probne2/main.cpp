#include <iostream>
#include "kanapka.h"
#include <cstdio>
//#include <message.h>

using namespace std;

int kanapka(long long n)
{
    int maxL = NULL;
    int maxP = NULL;
    int checker = NULL;
    int suma = NULL;

    for(int i = 0; i < n; i++)
    {
        suma += GetTaste_SmallSample(i);
        if(suma > maxL)
        {
            maxL = suma;
            checker = i;
        }
    }
    suma = 0;
    for(int i = n-1; i > checker; i--)
    {
        suma += GetTaste_SmallSample(i);
        if(suma > maxP)
            maxP = suma;
    }
    suma = maxP + maxL;
    return suma;
}

int main()
{
    long long n = GetN_SmallSample();
    printf("%u", kanapka(n));
    return 0;
}
