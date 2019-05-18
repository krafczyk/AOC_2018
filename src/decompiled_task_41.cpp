#include <cstdio>

int main(int argc [[maybe_unused]], char** argv [[maybe_unused]]) {
    int x = 123; // reg(2)
    x = x&456;
    if (x == 72) {
        x = 1;
    } else {
        x = 0;
    }
    if (x == 1) { // jump? inst 2
        x = 0;
    }

    int y = x|65536; // reg(4)
    x = 6718165;
    printf("x: %i y: %i\n", x, y);

    int A = y&255; // reg(3)

    x = x+A;
    x = x&16777215;
    x = x*65899;
    x = x&16777215;

    printf("x: %i y: %i A: %i\n", x, y, A);

    if (256 > y) {
        A = 1;
    } else {
        A = 0;
    }
}
