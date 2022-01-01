#include <nxs/regex/dfa.h>

#include <algorithm>

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
        if (iter->second.def == nil) {
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

} // namespace nxs::regex