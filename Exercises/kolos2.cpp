#include <stdlib.h>
#include <iostream>

using namespace std;

int str_len(char *s)
{
    int iloscZnakow;
    for(; *s != '\0'; *s++)
        iloscZnakow++;
    cout << "Ilosc znakow w napisie: " << iloscZnakow;
    return iloscZnakow;
}

int main()
{
    char napis[25] = "Ma³y kotek";
    str_len(napis);

    return EXIT_SUCCESS;
}
