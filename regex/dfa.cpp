#include <nxs/regex/dfa.h>
#include <nxs/regex/nfa.h>

#include <algorithm>
#include <string>
#include <codecvt>
#include <locale>

static std::string To_UTF8(const std::u32string& s)
{
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
    return conv.to_bytes(s);
}

namespace nxs::regex {

dfa_t::dfa_t(const builder_t& builder)
: init_state_(builder.init_state)
, transition_(builder.transition)
, terminates_(builder.terminates)
{
    current_state_ = init_state_;
    debug_ = false;
}

dfa_t::~dfa_t() { }

void dfa_t::reset() { current_state_ = init_state_; }

bool dfa_t::resolve() const
{
    return std::find(terminates_.begin(), terminates_.end(), current_state_)
        != terminates_.end();
}

dfa_t::state_t dfa_t::put(charcode_t c)
{
    if (debug_) {
        printf("dfa:%p current_state:%d%s put:%x\n", this, current_state_,
            resolve() ? "(resolve)" : "", c);
    }

    auto iter = transition_.find(current_state_);
    if (iter == transition_.end()) {
        if (debug_) {
            printf("dfa:%p reject: invalid current_state\n", this);
        }
        return REJECT;
    }

    auto iter2 = iter->second.actions.find(c);
    if (iter2 == iter->second.actions.end()) {
        if (iter->second.def == nil_state) {
            if (debug_) {
                printf("dfa:%p reject: no action\n", this);
            }
            return REJECT;
        }
        current_state_ = iter->second.def;
    } else {
        current_state_ = iter2->second;
    }

    bool is_resolve = resolve();

    if (debug_) {
        printf("dfa:%p %s new_state:%d\n", this,
            is_resolve ? "resolve" : "accept", current_state_);
    }

    return is_resolve ? RESOLVE : ACCEPT;
}

void dfa_t::to_nfa(nfa_t& out) const
{
    auto builer = nfa_t::builder_t();

    builer.set_init_state(init_state_);

    for (auto& ts : terminates_) {
        builer.add_terminate_state(ts);
    }

    for (auto& iter : transition_) {
        if (iter.second.def != nil_state) {
            builer.add_def_action(iter.first, iter.second.def);
        }

        for (auto& iter2 : iter.second.actions) {
            builer.add_action(iter.first, iter2.first, iter2.second);
        }
    }

    out = builer.build();
}

void dfa_t::dot(std::ostream& os) const
{
    os << "digraph { ";

    char32_t csz[] = { 'A', '\0' };
    std::u32string sz(csz);

    for (auto& iter : transition_) {
        for (auto& iter2 : iter.second.actions) {
            os << iter.first;
            os << " -> ";
            os << iter2.second;
            sz[0] = iter2.first;
            os << " [ label = \"" << To_UTF8(sz) << "\" ];";
        }

        if (iter.second.def != nil_state) {
            os << iter.first;
            os << " -> ";
            os << iter.second.def;
            os << " [ label = \"*\" ];";
        }
    }

    for (auto it : terminates_) {
        os << it << " [ shape = doublecircle ]; ";
    }

    os << init_state_ << "[ shape = box ]; ";

    os << "}";
}

} // namespace nxs::regex