#include <iostream>
#include <stdio.h>

using namespace std;

int main()
{
    char w;
    int t, n, m, a, b;
    int counterBajtka, counterBitka, counterRepeat;
    scanf("%u", &t);

    for(int i = 0; i < t; i++)
    {
        int bajtek[100000];
        int bitek[100000];
        scanf("%u %u", &n, &m);
        counterBajtka = 0;
        counterBitka = 0;
        for(int j = 0; j < m; j++)
        {
            scanf("%u %c %u", &a, &w, &b);
            if(1 <= a <= n && 1 <= b <= n)
            {
                if(w == '<')
                {
                    counterRepeat = 0;
                    bitek[j] = b;
                    for(int b = 0; b < j; b++)
                        if(bitek[b] == bitek[j] && n != 1)
                            counterRepeat++;
                    if(counterRepeat == 0)
                        counterBitka++;
                }
                else if(w == '>')
                {
                    counterRepeat = 0;
                    bajtek[j] = a;
                    for(int b = 0; b < j; b++)
                        if(bajtek[b] == bajtek[j] && n != 1)
                            counterRepeat++;
                    if(counterRepeat == 0)
                        counterBajtka++;
                }
            }
        }
        if(n != 1)
        {
            if(counterBajtka == n)
                cout << "WYGRANA" << endl;
            else if(counterBajtka != n && counterBitka != n)
                cout << "REMIS" << endl;
            else if(counterBitka == n)
                cout << "PRZEGRANA" << endl;
        }
        else
        {
            if(counterBajtka > counterBitka)
                cout << "WYGRANA" << endl;
            else if(counterBitka > counterBajtka)
                cout << "PRZEGRANA" << endl;
        }
    }
    return 0;
}
