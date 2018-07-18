#include <stdlib.h>
#include <iostream>
#include <float.h>

using namespace std;

int main()
{
    double suma, srednia, maxi, mini;
    double time;
    int counter;
    cout << "Wprowadzaj czasy. Wpisanie '0' konczy wprowadzanie.\n";
    cout << ">";
    cin >> time;
    if(time == 0)
        cout << "Gratuluje. Nie umiesz biegac.";
    else
    {
        mini = time;
        maxi = time;
        counter = 1;
        cout << ">";
        cin >> time;
        while(time != 0)
        {
            suma += time;

            if(time < mini)
                mini = time;
            if(time > maxi)
                maxi = time;
            counter++;
            cout << ">";
            cin >> time;
            cout << endl;
        }
        srednia = suma/counter;
        cout << "Czas sumaryczny: " << suma << endl;
        cout << "Czas sredni: " << srednia << endl;
        cout << "Czas najgorszy: " << maxi << endl;
        cout << "Czas najlepszy: " << mini << endl;
    }
    return EXIT_SUCCESS;
}
