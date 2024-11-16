#include "ThreadSafeVector.hpp"

#include <iostream>
#include <immintrin.h>

using namespace std;    
using namespace datatype;

int main() {
    ThreadSafeVector<int> d(vc_t::NO_CONFIG);
    //d++;
    int values[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
    int result[16];

    // Load the vector into an AVX-512 register
    __m512i vec = _mm512_loadu_si512(values);

    // Create a vector of ones
    __m512i ones = _mm512_set1_epi32(1);
    // Increment each element
    __m512i incremented = _mm512_add_epi32(vec, ones);

    // Store the result back into memory
    _mm512_storeu_si512(result, incremented);

    // Print the results
    for (int i : result) {
        std::cout << i << " ";
    }
    return 0;
}