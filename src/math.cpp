#include <nxs/nxs.h>

#include <string>

namespace niu2x::nxs {

int atoi(const char* base, int size)
{
    int result = 0;
    for (int i = 0; i < size; i++)
        result = result * 10 + (base[i] - '0');
    return result;
}

double stod(const char* base, int size)
{

    std::string v(base, base + size);
    return std::stod(v);
}

} // namespace niu2x::nxs
