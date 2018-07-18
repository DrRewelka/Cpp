#include <stdlib.h>
#include <iostream>

using namespace std;

int lwr_str_cnt(char *s)
{
    int counter;
    while(*s != 0)
    {
        if(*s >= 'a' && *s <= 'z')
            counter++;
        *s++;
    }
    cout << "Ilosc ma³ych znakow w napisie: " << counter;
    return counter;
}

int main()
{
    char s[25] = "PoKeMoNy";
    lwr_str_cnt(s);

    return EXIT_SUCCESS;
}
