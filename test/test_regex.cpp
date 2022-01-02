#include <iostream>
#include <nxs/regex/regex.h>

int main()
{
    std::string ss;
    std::cin >> ss;
    nxs::regex::dfa_t dfa;
    auto success = nxs::regex::parse(ss, &dfa);
    if (success) {
        dfa.dot(std::cout);
    } else {
        std::cerr << "error" << std::endl;
    }
    return 0;
}