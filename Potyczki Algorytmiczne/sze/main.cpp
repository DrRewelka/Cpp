#include <iostream>
#include <stdio.h>

using namespace std;

int main()
{
    int n, m;
    int counter = 0;
    int sameCounter = 0;
    long p, k, c;
    scanf("%u %u", &n, &m);
    int pTime[n];
    int kTime[n];
    int cTime[n];
    for(int i = 0; i < n; i++)
    {
        scanf("%lu %lu %lu", &p, &k, &c);
        pTime[i] = p;
        kTime[i] = k;
        cTime[i] = c;
    }
    for(int i = 0; i < n; i++)
    {
        if(kTime[i] - pTime[i] >= cTime[i])
            counter++;
        for(int j = i + 1; j < n; j++)
        {
            if(pTime[i] == pTime[j] && kTime[i] == kTime[j] && cTime[i] == cTime[j])
                sameCounter++;
        }
    }
    if(counter == n && sameCounter < m)
        printf("TAK");
    else
        printf("NIE");
    return 0;
}
