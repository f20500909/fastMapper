#include <stdlib.h>
#include <time.h>

#include <iostream>
#include <vector>

#include "../bitMap.hpp"

using namespace std;

int main(int argc, char *argv[]) {

    BitMap bitMap(32);
    bitMap.set(0,true);

    bitMap.set(6,false);

    bitMap.set(15,false);
    bitMap.set(16,true);
    cout<<bitMap<<endl;

    BitMap b2(32);
    b2.set(0,true);

    cout<<b2<<endl;

    return 0;
}