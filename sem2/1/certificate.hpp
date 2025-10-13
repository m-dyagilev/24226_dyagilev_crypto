#ifndef CERTIFICATE_HPP
#define CERTIFICATE_HPP

#include <string>

struct Certificate {
    std::string owner;
    std::string signedBy;
};

#endif
