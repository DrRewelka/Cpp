#include <iostream>
#include <cstdio>

using namespace std;

char tab[3] = {'a','b','c'};

char* slowo(char* tablica, long long n, long long k)
{
    char* temp = new char[n];
    char* temp2 = new char[n];
    for(int i = 0; i < k; i++)
    {
        do
        {
            temp[i] = 'a';
            if(temp[i-1] == temp[i])
                temp[i] = 'b';
            if(i == (n-1))
            {
                temp[i]++;
                if(temp[i] > 'c')
                {
                    temp[i-1]++;
                }
                if(temp[i-1] == temp[i])
                    temp[i-1]++;
            }
        }
        while(temp[i-1] == temp[i]);
    }
    for(int i = 0; i < n; i++)
    {
        temp2[i] = temp[i];
    }
    return temp2;
}

int main()
{
    long long int n;
    long long int k;
    scanf("%llu %llu", &n, &k);
    cout << slowo(tab, n, k);
    return 0;
}
