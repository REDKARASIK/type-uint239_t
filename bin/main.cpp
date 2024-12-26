#include <lib/number.h>
#include <iostream>


int main() {
    uint239_t value = FromInt(8, 0);
    uint239_t value2 = FromInt(2, 0);
    std::cout << (value / value2) << '\n';
    return 0;
}