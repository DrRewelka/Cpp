#include <stdlib.h>
#include <iostream>

using namespace std;

int dgt_str_cnt(char *s)
{
    int counter;
    while(*s != 0)
    {
        if(*s >= '0' && *s <= '9')
            counter++;
        *s++;
    }
    cout << "Ilosc cyfr w napisie: " << counter;
    return counter;
}

int main()
{
    char s[25] = "10 LOl Kur4De 92";
    dgt_str_cnt(s);
    return EXIT_SUCCESS;
}
