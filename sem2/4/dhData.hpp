#ifndef DH_DATA_HPP
#define DH_DATA_HPP

#include <cstddef>
#include <iostream>
#include <vector>

struct KeysPair {
    size_t open;
    size_t secret;
};

class DHData {
   public:
    DHData(size_t iN, size_t iP, size_t iG);
    void init();
    KeysPair getKeysPair(size_t idx);
    size_t generateCommonKey(size_t idx1, size_t idx2);
    void print(std::ostream& os);

   private:
    const size_t N;
    const size_t P;
    const size_t G;
    void generateKeysPair(size_t idx);
    std::vector<KeysPair> usersKeys;
};

#endif
