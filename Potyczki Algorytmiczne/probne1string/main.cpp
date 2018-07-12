#include <iostream>
#include <cstdio>

using namespace std;

string slowo(long long int n, long long int k)
{
    bool change = false;
    string s = "";
    for(int i = 0; i < k; i++)
    {
        if(s.length() < n)
        {
            if(s[s.length()-1] == 'a')
                s += 'b';
            else
                s += 'a';
        }
        else
        {
            do
            {
                if(++s[s.length()-1] > 'c')
                {
                    s = s.substr(0, s.length()-1);
                    change = true;
                }
                else
                {
                    change = false;
                }
            }
            while(((s[s.length()-2] == s[s.length()-1]) && (s != "")) || change);
        }
    }
    return s == "" ? "NIE" : s;
}

int main()
{
    long long int n;
    long long int k;
    scanf("%llu %llu", &n, &k);
    printf("%s", slowo(n,k).c_str());
    return 0;
}
