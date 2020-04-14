#include <stdlib.h>
#include <time.h>

#include <iostream>
#include <vector>

#include "../bitMap.hpp"

using namespace std;

int main(int argc, char *argv[]) {

    BitMap bitMap(8);
//    bitMap.set(0,true);
    bitMap.set(1,true);
    bitMap.set(2,true);
    bitMap.set(3,true);
    bitMap.set(4,true);
    bitMap.set(5,true);
    bitMap.set(6,true);
    bitMap.set(7,true);
    cout<<bitMap<<endl;



    return 0;
}