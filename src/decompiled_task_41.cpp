#include <cstdio>
#include <limits>
#include <iostream>
#include "utilities.h"
#include <vector>

int main(int argc [[maybe_unused]], char** argv [[maybe_unused]]) {
    long r0 [[maybe_unused]] = 0;
    if(argc > 1) {
        r0 = std::atol(argv[1]);
    }
    long x = 123; // reg(2) 0:
    x = x&456; // 1:
    if (x == 72) { // 2:
        x = 1;
    } else {
        x = 0;
    }
    if (x == 1) { // jump? inst 2
        x = 0;
    }
    long y = 0; // reg(4)
    long A = 0; // reg(3)
    //long B = 0; // reg(1)

    std::vector<long> encountered;

    y = x|65536; // reg(4) 6:
    x = 6718165; // 7:
    printf("end of pre-amble\n");
    printf("x: %li A: %li y: %li\n", x, A, y);

    A = y&255; // reg(3) 8:
    x = x+A; // 9:
    x = x&16777215; // 10:
    x = x*65899; // 11:
    x = x&16777215; // 12:
    printf("first adjustment of x\n");
    printf("x: %li A: %li y: %li\n", x, A, y);

    //if (256 > y) { // 13:
    //    A = 1; // jump?
    //} else {
    //    //A = 0; // part of 13:
    //    //A = 0; // 17:
    //    //long B = A+1; // 18:
    //    //printf("B: %li x: %li A: %li y: %li\n", B, x, A, y);
    //    //B = B*256; // 19:
    //    //printf("B: %li x: %li A: %li y: %li\n", B, x, A, y);
    //    //while(B <= y) { // 20:
    //    //    A += 1; // 24:
    //    //    B = A+1; // 18:
    //    //    B = B*256; // 19:
    //    //}
    //    A = (y/256);
    //    B = 1; // from 20:
    //    printf("End loop\n");
    //    printf("B: %li x: %li A: %li y: %li\n", B, x, A, y);
    //    y = A; // 26:
    //    printf("26: B: %li x: %li A: %li y: %li\n", B, x, A, y);

    //    A = y&255; // reg(3) 8:
    //    printf("8: B: %li x: %li A: %li y: %li\n", B, x, A, y);
    //    x = x+A; // 9:
    //    printf("9: B: %li x: %li A: %li y: %li\n", B, x, A, y);
    //    x = x&16777215; // 10:
    //    printf("10: B: %li x: %li A: %li y: %li\n", B, x, A, y);
    //    x = x*65899; // 11:
    //    printf("11: B: %li x: %li A: %li y: %li\n", B, x, A, y);
    //    x = x&16777215; // 12:

    //    printf("12: B: %li x: %li A: %li y: %li\n", B, x, A, y);

    //    //if (256 > y) { // 13:
    //    //    A = 1;
    //    //} else {
    //    //    A = 0; //    //    A = 0;
    //    //    B = A+1;
    //    //    B = B*256;
    //    //    while(B <= y) {
    //    //        A += 1;
    //    //        B = A+1;
    //    //        B = B*256;
    //    //    }
    //    //}
    //}

    do {
        printf("Start Outer loop\n");
        printf("x: %li A: %li y: %li\n", x, A, y);
        while(256 <= y) {
            printf("Inner loop Iteration\n");
            printf("x: %li A: %li y: %li\n", x, A, y);
            A = (y/256);

            printf("x: %li A: %li y: %li\n", x, A, y);
            y = A; // 26:
            printf("x: %li A: %li y: %li\n", x, A, y);
            A = y&255; // reg(3) 8:
            printf("x: %li A: %li y: %li\n", x, A, y);
            x = x+A; // 9:
            x = x&16777215; // 10:
            x = x*65899; // 11:
            x = x&16777215; // 12:
            printf("x: %li A: %li y: %li\n", x, A, y);
            printf("End Inner loop Iteration\n");
        }
        if(hasElement(encountered, x)) {
            // Repeat detected!
            printf("Repeat in x detected!\n");
            break;
        } else {
            encountered.push_back(x);
        }
        if(x == r0) {
            // Encountered end
            printf("Encountered end!");
            break;
        } else {
            y = x|65536;
            x = 6718165;
            A = y&255;
            x = x+A;
            x = x&16777215;
            x = x*65899;
            x = x&16777215;
            printf("End of loop update\n");
            printf("x: %li A: %li y: %li\n", x, A, y);
        }
    } while(true);
    ConstForEach(encountered, [](auto& a) { std::cout << a << std::endl; });
}
