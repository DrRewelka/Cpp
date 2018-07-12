#include <iostream>
#include <stdio.h>

using namespace std;

bool isPrime(long long k)
{
    if(k < 2)
        return false;
    for(long long i = 2; i * i <= k; i++)
    {
        if(k % i == 0)
            return false;
    }
    return true;
}

int main()
{
    string s = "";
    int t;
    long long k;
    scanf("%u", &t);
    for(int i = 0; i < t; i++)
    {
        int oneCount = 0;
        int divCount = 0;
        int sumCount = 0;
        scanf("%ull", &k);
        if(!isPrime(k))
        {
            while(k != 2 || k != 3 || k != 5)
            {
                k = k / 2;
                divCount++;
            }
            if(k == 2)
            {
                s = "(1+1)";
                for(int j = 0; j < divCount; j++)
                    s += "*(1+1)";
            }
            else if(k == 3)
            {
                s = "(1+1+1)";
                for(int j = 0; j < divCount; j++)
                    s += "*(1+1)";
            }
            else if(k == 5)
            {
                s = "(1+1+1+1+1)";
                for(int j = 0; j < divCount; j++)
                    s += "*(1+1)";
            }
        }
        else
        {
            while(k != 2 || k != 3 || k != 5)
            {
                if(k % 2 != 0)
                {
                    s += "*(1+1)+1)";
                    sumCount++;
                }
                else
                    s += "*(1+1)";
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
        cout << s;
    }
    return 0;
}
