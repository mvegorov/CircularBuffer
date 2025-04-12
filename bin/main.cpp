#include <iostream>
#include "lib/CCircularBuffer.h"
#include <vector>

using namespace buff;

int main() {
    CCircularBuffer<char> a(5);
    a.put(1);
    a.put(2);
    a.put(3);

    for (auto i: a){
        std::cout << i;
    }

}