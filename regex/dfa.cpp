#include <nxs/regex/dfa.h>
#include <nxs/regex/nfa.h>

#include "utils.h"

#include <algorithm>
#include <iostream>
#include <unordered_set>

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

    os << "start";
    os << " -> ";
    os << init_state_;

    os << "}";
}

dfa_t dfa_t::optimize() const
{
    std::unordered_set<state_t> states;
    for (auto iter : transition_) {
        states.emplace(iter.first);
    }

    for (auto iter : terminates_) {
        states.emplace(iter);
    }

    std::vector<state_t> states0 { states.begin(), states.end() };

    std::sort(states0.begin(), states0.end());

    for (int i = 0; i < states0.size() - 1; i++) {
        for (int j = i + 1; j < states0.size(); j++) {
            if (mergeable(states0[i], states0[j])) {
                return merge_state(states0[i], states0[j]).optimize();
            }
        }
    }
    return *this;
}

dfa_t dfa_t::merge_state(state_t a, state_t b) const
{

    auto builder = builder_t();

    builder.set_init_state(init_state_);

    for (auto& iter : this->transition_) {
        if (iter.first != b) {

            for (auto& iter2 : iter.second.actions) {
                builder.add_action(iter.first, iter2.first,
                    iter2.second == b ? a : iter2.second);
            }

            if (iter.second.def != nil_state) {
                builder.add_def_action(
                    iter.first, iter.second.def == b ? a : iter.second.def);
            }
        } else {

            // for (auto& iter2 : iter.second.actions) {
            //     builder.add_action(a, iter2.first,
            //         iter2.second == b ? a : iter2.second);
            // }

            // if (iter.second.def != nil_state) {
            //     builder.add_def_action(
            //         a, iter.second.def == b ? a : iter.second.def);
            // }
        }
    }

    for (auto k : terminates_) {
        if (k != b)
            builder.add_terminate_state(k);
    }
    return builder.build();
}

bool dfa_t::mergeable(state_t a, state_t b) const
{
    if (debug_) {
        std::cout << "mergeable " << a << "," << b << std::endl;
    }

    if (a == init_state_ || b == init_state_) {
        if (debug_) {
            std::cout << " mergeable false : a or b is init_state_"
                      << std::endl;
        }
        return false;
    }

    if (is_terminate_state(a) != is_terminate_state(b)) {
        if (debug_) {
            std::cout << " mergeable false : a/b's terminate diff" << std::endl;
        }
        return false;
    }

    auto a_state_trans = state_transition(a);
    auto b_state_trans = state_transition(b);

    if (a_state_trans.def != b_state_trans.def) {
        if (debug_) {
            std::cout << " mergeable false : a/b's def diff" << std::endl;
        }
        return false;
    }

    if (a_state_trans.actions.size() != b_state_trans.actions.size()) {
        if (debug_) {
            std::cout << " mergeable false : a/b's actions size diff"
                      << std::endl;
        }
        return false;
    }

    for (auto iter : a_state_trans.actions) {

        auto a_action = iter.second;
        auto b_action = state_action(b_state_trans, iter.first);

        if (a_action != b_action
            && ((a_action != a && a_action != b)
                || (b_action != a && b_action != b))
            // && ( a_action != nil_state && b_action != nil_state)
        ) {
            if (debug_) {
                std::cout << " mergeable false : a/b's actions at "
                          << iter.first << " diff" << std::endl;
            }

            return false;
        }
    }

    return true;
}

dfa_t::state_t dfa_t::state_action(
    const state_transition_t& st, charcode_t c) const
{
    auto iter = st.actions.find(c);
    if (iter != st.actions.end())
        return iter->second;
    return nil_state;
}

dfa_t::state_transition_t dfa_t::state_transition(state_t s) const
{
    auto iter = transition_.find(s);
    if (iter != transition_.end()) {
        return iter->second;
    }
    return {};
}

bool dfa_t::is_terminate_state(state_t st) const
{
    return std::find(terminates_.begin(), terminates_.end(), st)
        != terminates_.end();
}

} // namespace nxs::regex