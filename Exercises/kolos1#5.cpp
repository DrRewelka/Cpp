#include <stdlib.h>
#include <iostream>

using namespace std;

double temps[7];

void insertData()
{
    cout << "Wpisz odczyty temperatur.\n";
    for(int i = 0; i < 7; i++)
    {
        cin >> temps[i];
    }
}

int main()
{
    insertData();
    double mini, maxi;
    int counter = 0;
    cout << "Wpisz dolna granice: ";
    cin >> mini;
    cout << "\nWpisz gorna granice: ";
    cin >> maxi;
    for(int i = 0; i < 7; i++)
    {
        if(mini <= temps[i])
            if(maxi >= temps[i])
                counter++;
        if(mini > maxi)
        {
            mini == 0;
            maxi == 50;
        }
    }
    cout << "\nLiczba temperatur z zakresu: " << counter;
    return EXIT_SUCCESS;
}
