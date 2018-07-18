#include <stdlib.h>
#include <iostream>
#include <time.h>

using namespace std;
int main()
{
    srand(time(0));
    int random = rand() % 100;
    int numb;

    cout << "Zaproponuj liczbe, ktora wybralem: ";
    cin >> numb;

    while(true)
    {
        if(numb == random)
        {
            cout << "To jest liczba ktora wybralem! Gratuluje!\n";
            return EXIT_SUCCESS;
        }

        if(numb < random)
            cout << "Moja liczba jest wieksza od twojej.\n";

        if(numb > random)
            cout << "Moja liczba jest mniejsza od twojej.\n";

        cout << "Wpisz nowa liczbe: ";
        cin >> numb;
    }
}
