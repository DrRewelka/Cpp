#include <stdlib.h>
#include <iostream>
#include <time.h>

using namespace std;

int main()
{
    srand(time(0));
    int random = rand() % 100 + 1;
    int numb;
    int mini = 0;
    int maxi = 0;

    while(mini == 0 || maxi == 0)
    {
        cout << "Moja liczba to: " << random << endl;
        cout << "Czy ta liczba jest:\n1.Taka, jaka wymysliles.\n2.Mniejsza od twojej.\n3.Wieksza od twojej.\n";
        cin >> numb;

        if(numb == 1)
        {
            cout << "Odgadlem!";
            return EXIT_SUCCESS;
        }

        if(numb == 2)
        {
            mini = random;
            random = rand() % (101 - mini) + mini;
            cout << random << endl;
        }

        if(numb == 3)
        {
            maxi = random;
            random = rand() % (maxi + 1);
            cout << random << endl;
        }
    }

    while(mini != 0 && maxi != 0)
    {
        cout << "Moja liczba to: " << random << endl;
        cout << "Czy ta liczba jest:\n1.Taka, jaka wymysliles.\n2.Mniejsza od twojej.\n3.Wieksza od twojej.\n";
        cin >> numb;

        if(numb == 1)
        {
            cout << "Odgadlem!";
            return EXIT_SUCCESS;
        }

        if(numb == 2)
        {
            mini = random;
            random = rand() % (maxi + 1 - mini) + mini;
            cout << random << endl;
        }

        if(numb == 3)
        {

        }

        if(maxi == mini)
        {
            cout << "Cos czuje, ze oszukujesz!";
            break;
        }
    }
}
