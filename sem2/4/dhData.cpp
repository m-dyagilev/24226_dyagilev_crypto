#include "dhData.hpp"

#include <cassert>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <random>

#include "deg_mod.hpp"

DHData::DHData(size_t iN, size_t iP, size_t iG) : N(iN), P(iP), G(iG) {
    usersKeys.resize(N);
    std::srand(time(NULL));
}

void DHData::init() {
    for (size_t i = 0; i < N; ++i) {
        generateKeysPair(i);
    }
}

KeysPair DHData::getKeysPair(size_t idx) {
    assert(idx < N);
    return usersKeys[idx];
}

size_t DHData::generateCommonKey(size_t idx1, size_t idx2) {
    assert(idx1 < N);
    assert(idx2 < N);
    KeysPair keys1 = getKeysPair(idx1);
    KeysPair keys2 = getKeysPair(idx2);
    size_t z1 = degMod(keys2.open, keys1.secret, P);
    std::cout << idx1 << " performs " << keys2.open << "^" << keys1.secret
              << " mod " << P << std::endl;
    std::cout << idx1 << " gets key " << z1 << std::endl;
    size_t z2 = degMod(keys1.open, keys2.secret, P);
    std::cout << idx2 << " performs " << keys1.open << "^" << keys2.secret
              << " mod " << P << std::endl;
    std::cout << idx2 << " gets key " << z2 << std::endl;
    assert(z1 == z2);
    std::cout << "They got equal keys" << std::endl;
    return z1;
}

void DHData::print(std::ostream& os) {
    os << "System parameters: N == " << N << ", P == " << P << ", G == " << G
       << std::endl;

    size_t nLength = 0;
    size_t nCopy = N;
    while (0 < nCopy) {
        nCopy /= 10;
        nLength++;
    }

    size_t pLength = 0;
    size_t pCopy = P;
    while (0 < pCopy) {
        pCopy /= 10;
        pLength++;
    }

    os << std::setw(2 * nLength + 1) << "|";
    os << std::setw(2 * pLength) << "X" << "|";
    os << std::setw(2 * pLength) << "Y";
    os << std::endl;
    for (size_t i = 0; i < N; ++i) {
        os << std::setw(2 * nLength) << i << "|";
        KeysPair keys = getKeysPair(i);
        os << std::setw(2 * pLength) << keys.secret << "|";
        os << std::setw(2 * pLength) << keys.open << std::endl;
    }
}

void DHData::generateKeysPair(size_t idx) {
    assert(idx < N);
    usersKeys[idx].secret = std::rand() % P;
    usersKeys[idx].open = degMod(G, usersKeys[idx].secret, P);
}
