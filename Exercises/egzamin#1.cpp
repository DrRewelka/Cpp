#include <iostream>
#include <stdlib.h>
#include <math.h>

using namespace std;

int oblicz(int *tab, int ileElementow, int liczba)
{
    int counter = 0;
    while(--ileElementow >= 0)
    {
        if(*tab > liczba)
            counter++;
        if(*tab < liczba)
            *tab = 0;
        tab++;
    }
    return counter;
}

int obliczSuma(int *tab, int ileElementow, int liczba)
{
    int suma = 0;
    while(--ileElementow >= 0)
    {
        if(*tab < liczba)
            suma += *tab;
        if(*tab > liczba)
            *tab = 0;
        tab++;
    }
    return suma;
}

int ileWystapienZnakuWNapisie(char z, char *napis)
{
    int counter = 0;
    while(*napis != '\0')
    {
        if(tolower(*napis) == tolower(z))
            counter++;
        napis++;
    }
    return counter;
}

int ktorychZnakowNajmniej(char *napis)
{
    int counterD = 0;
    int counterM = 0;
    int counterC = 0;
    while(*napis != '\0')
    {
        if(*napis >= 'A' && *napis <= 'Z')
            counterD++;
        if(*napis >= 'a' && *napis <= 'z')
            counterM++;
        if(*napis >= '0' && *napis <= '9')
            counterC++;
        napis++;
    }
    if(counterD < counterM && counterD < counterC)
        return 1;
    if(counterM < counterD && counterM < counterC)
        return -1;
    if(counterC < counterD && counterC < counterM)
        return 0;
    if(counterC == counterD || counterC == counterM || counterD == counterM)
        return -2;
}

char najczesciejWystepujacaLitera(char *napis)
{
    int counter = 0;
    int finalCounter = 0;
    int counterError = 0;
    char c;
    char finalC;
    while(*napis != '\0')
    {
        if(tolower(*napis) >= 'a' && tolower(*napis) <= 'z')
        {
            c = tolower(*napis);
            while(*napis != '\0')
            {
                if(tolower(*napis) == c)
                    counter++;
                napis++;
            }
            if(counter > finalCounter)
                finalCounter = counter;
                finalC = c;
            counterError = INT_MAX;
        }
        else
            counterError--;
        napis++;
    }
    if(counterError < 0)
        return '\0';
    else
        return finalC;
}

bool jestPalindromem(char *napis)
{
    char *c = napis;
    while(*napis != '\0')
    {
        napis++;
    }
    napis--;
    while(*c != '\0')
    {
        while(tolower(*c) == ' ' && tolower(*c) != '\0')
            c++;
        while(tolower(*napis) == ' ' && tolower(*napis) != '\0')
            napis--;
        if(tolower(*c) != tolower(*napis))
            return false;
        c++;
        napis--;
    }
    return true;
}

int main()
{
    int tab[5] = {1, 3, 5, 20, 14};
    cout << "Liczba cyfr: " << oblicz(tab, 5, 5) << endl;
    cout << "Suma liczb: " << obliczSuma(tab, 5, 5) << endl;
    cout << "Ilosc znakow w napisie: " << ileWystapienZnakuWNapisie('k', "Kot kreskowy") << endl;
    cout << "Ktorych znakow najmniej? " << ktorychZnakowNajmniej("123ABCabc") << endl;
    cout << "Najczesciej wystepujaca litera: " << najczesciejWystepujacaLitera("Kot kreskowy") << endl;
    cout << "Czy to jest palindrom? " << jestPalindromem("Kobyla ma maly bok");
    return EXIT_SUCCESS;
}
