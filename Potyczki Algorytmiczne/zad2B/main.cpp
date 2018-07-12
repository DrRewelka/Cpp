#include <iostream>
#include <stdio.h>

using namespace std;

bool isPrime(long k)
{
    if(k < 2)
        return false;
    for(long p = 2; p * p <= k; p++)
        if(k % p == 0)
            return false;
    return true;
}

int main()
{
    int t;
    long k;
    scanf("%u", &t);
    for(int i = 0; i < t; i++)
    {
        string s = "";
        int oneCount = 0;
        int divCount = 0;
        int sumCount = 0;
        scanf("%lu", &k);
        if(k == 1)
            s = "1";
        else if(k != 2 && k != 3 && k != 5)
        {
            if(!isPrime(k))
            {
                while(k != 2 && k != 3 && k != 5)
                {
                    if(k % 2 != 0)
                    {
                        s = "*(1+1)+1)" + s;
                        sumCount++;
                    }
                    else
                        s = "*(1+1)" + s;
                    k = k / 2;
                    divCount++;
                }
                if(k == 2)
                {
                    s = "(1+1)" + s;
                    for(int j = 0; j < sumCount; j++)
                        s = "(" + s;
                }
                else if(k == 3)
                {
                    s = "(1+1+1)" + s;
                    for(int j = 0; j < sumCount; j++)
                        s = "(" + s;
                }
                else if(k == 5)
                {
                    s = "(1+1+1+1+1)" + s;
                    for(int j = 0; j < sumCount; j++)
                        s = "(" + s;
                }
            }
            else
            {
                while(k != 2 && k != 3 && k != 5)
                {
                    if(k % 2 != 0)
                    {
                        s = "*(1+1)+1)" + s;
                        sumCount++;
                    }
                    else
                        s = "*(1+1)" + s;
                    k = k / 2;
                    divCount++;
                }
                if(k == 2)
                {
                    s = "(1+1)" + s;
                    for(int j = 0; j < sumCount; j++)
                        s = "(" + s;
                }
                else if(k == 3)
                {
                    s = "(1+1+1)" + s;
                    for(int j = 0; j < sumCount; j++)
                        s = "(" + s;
                }
                else if(k == 5)
                {
                    s = "(1+1+1+1+1)" + s;
                    for(int j = 0; j < sumCount; j++)
                        s = "(" + s;
                }
            }
        }
        else if(k == 2)
            s = "(1+1)";
        else if(k == 3)
            s = "(1+1+1)";
        else if(k == 5)
            s = "(1+1+1+1+1)";
        for(int l = 0; l < s.size(); l++)
            if(s[l] == '1')
                oneCount++;
        if(oneCount <= 100)
            cout << s;
        else
            cout << "NIE";
        cout << endl;
    }
    return 0;
}
