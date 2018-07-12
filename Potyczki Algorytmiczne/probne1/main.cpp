#include <iostream>
#include <cstdio>

using namespace std;

char tab[3] = {'a','b','c'};

char* slowo(char* tablica, long long n, long long k)
{
    char* temp = new char[n];
    char* temp2 = new char[n];
    int i = 0;
    while(i != k)
    {
        int c = n;
        for(int j = 0; j < n; j++)
        {
            if(temp[0] == '\0')
                temp[j] = 'a';
            if(temp[j-1] == temp[j])
            {
                temp[j]++;
                i++;
            }
            else
                i++;
        }
        while(c != 0)
        {
            if(temp[c] > 'c')
            {
                temp[c] = NULL;
                temp[c-1]++;
            }
            c--;
        }
        i++;
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
