#include "CorrectModulus.hpp"

size_t modulus(long long dividend, long long divisor) {
    if (0 > divisor) {
        dividend *= -1;
        divisor *= -1;
    }
    long long integer_result = dividend / divisor;
    if (0 > dividend) {
        integer_result--;
    }
    return dividend - integer_result * divisor;
}
