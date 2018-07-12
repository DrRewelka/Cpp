#include "cielib.h"
#include "cielib.c"
#include <iostream>
#include <math.h>

using namespace std;

int myPos[500];
int maxPos[500];
int minPos[500];

void KrotkaIsInAnotherCastle(int currentDimension, int _r)
{
    int lowPos, highPos;
    int mid = coords[currentDimension];

    if(czyCieplo(coords) == 1)
    {
        if(highPos > maxPos[currentDimension])
        {

        }
    }
    else
    {

    }
}

int main()
{
    int currentDim;
    int d = podajD();
    int k = podajK();
    int r = podajR();

    currentDim = 0;

    for(int i = 0; i < d; i++)
    {
        minPos[i] = 0;
        maxPos[i] = r;
        myPos[i] = r >> 1;
    }

    czyCieplo(myPos);

    KrotkaIsInAnotherCastle(currentDim, r);
    return 0;
}
