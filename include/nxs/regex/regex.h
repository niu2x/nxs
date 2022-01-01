#ifndef NXS_REGEX_H
#define NXS_REGEX_H

#include <nxs/regex/dfa.h>
#include <nxs/regex/nfa.h>

namespace nxs::regex {

inline dfa_t to_dfa(const nfa_t& nda)
{
    dfa_t dfa;
    nda.to_dfa(dfa);
    return dfa;
}

inline nfa_t to_nfa(const dfa_t& dfa)
{
    nfa_t nfa;
    dfa.to_nfa(nfa);
    return nfa;
}

} // namespace nxs::regex

#endif