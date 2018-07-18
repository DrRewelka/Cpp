#include <cstdlib>
#include <iostream>

using namespace std;

double money[12];

void wpisz()
{
    int counter = 0;
    int i = 0;
    cout << "Wpisz dochody w kolejnych miesiacach.\n";
    while(i < 12)
    {
        cout << ++counter << ". ";
        cin >> money[i++];
    }
}

void check()
{
    int counter = 0;
    int i = 0;
    while(i < 12)
        cout << ++counter << ". " << money[i++] << endl;
}

void sumaryczny()
{
    double suma = 0;
    for(int i = 0; i < 12; i++)
    {
        suma += money[i];
    }
    double srednia = suma/12;
    cout << "Suma dochodow: " << suma << endl << "Roczna srednia dochodow na miesiac: " << srednia << endl;
}

void minMax()
{
    int i = 0;
    int mini = money[i];
    int maxi = money[i];
    for(i = 0; i < 12; i++)
    {
        if(money[i] < mini)
            mini = money[i];
        if(money[i] > maxi)
            maxi = money[i];
    }
    cout << "Najmniejsze dochody: " << mini << endl;
    cout << "Najwieksze dochody: " << maxi << endl;
}

int main()
{
    wpisz();
    check();
    sumaryczny();
    minMax();
}
