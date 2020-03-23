#include <stdlib.h>
#include <time.h>

#include <iostream>
#include <vector>

#include "rtree.hpp"

using namespace std;
using namespace rtree;

static const int kMaxTreeElements = 100000;

int main(int argc, char *argv[]) {
    R_tree<int> tree;
    vector<int> found;

    srand((unsigned) time(NULL));

    for (int i = 0; i < kMaxTreeElements; ++i) {
        tree.insert(Rect(rand(), rand(), rand(), rand()), i);
    }

    tree.nearest_search(Point(0, 0), 100, found);
    cout << "nearest_search test: " << " seconds, found " << found.size() << endl;
    cout << found[0] << "  " << found[1] << endl;


    for (R_tree<int>::iterator it = tree.begin(); it != tree.end(); ++it) {
        *it = 0;
    }

    return 0;
}