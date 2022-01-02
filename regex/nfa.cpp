#include <nxs/regex/nfa.h>

#include <algorithm>
#include <set>
#include <iostream>
#include <codecvt>
#include <locale>

static std::string To_UTF8(const std::u32string& s)
{
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
    return conv.to_bytes(s);
}

namespace nxs::regex {

nfa_t::nfa_t(const builder_t& builder)
: init_state_(builder.init_state)
, transition_(builder.transition)
, terminates_(builder.terminates)
{
}

nfa_t::~nfa_t() { }

std::unordered_set<nfa_t::state_t> nfa_t::all_states() const
{
    std::unordered_set<state_t> states = terminates_;
    for (auto& iter : transition_) {
        states.emplace(iter.first);
    }
    return states;
}

bool nfa_t::is_terminate_state(state_t st) const
{
    return std::find(terminates_.begin(), terminates_.end(), st)
        != terminates_.end();
}

static bool compartor(
    std::unordered_set<nfa_t::state_t> a, std::unordered_set<nfa_t::state_t> b)
{
    if (a.size() > b.size())
        return false;

    if (a.size() < b.size())
        return true;

    std::vector<nfa_t::state_t> aa;
    std::vector<nfa_t::state_t> bb;

    for (auto it : a)
        aa.push_back(it);
    for (auto it : b)
        bb.push_back(it);

    std::sort(aa.begin(), aa.end());
    std::sort(bb.begin(), bb.end());

    return aa < bb;
}

void nfa_t::to_dfa(dfa_t& out) const
{
    dfa_t::builder_t builder;

    state_factory_t sf;

    auto current_states = continues_states(init_state_, true);
    builder.set_init_state(sf.get(current_states));

    std::set<std::unordered_set<state_t>, decltype(compartor)*> visited(
        compartor);
    std::set<std::unordered_set<state_t>, decltype(compartor)*> works(
        compartor);

    works.emplace(current_states);

    while (!works.empty()) {
        current_states = *works.begin();

        // for (auto ssss : current_states)
        // std::cout << ssss << ",";
        // std::cout << std::endl;

        works.erase(works.begin());
        visited.emplace(current_states);

        auto all_chars = all_acceptable_chars(current_states);
        all_chars.erase(nil_char);

        for (auto& chr : all_chars) {
            auto new_state
                = continues_states(next_state(current_states, chr), true);
            builder.add_action(sf.get(current_states), chr, sf.get(new_state));

            if (std::find(visited.begin(), visited.end(), new_state)
                == visited.end()) {
                works.emplace(new_state);
            }
        }

        {
            auto new_state
                = continues_states(next_def_state(current_states), true);
            if (new_state.size() > 0) {
                builder.add_def_action(
                    sf.get(current_states), sf.get(new_state));
                if (std::find(visited.begin(), visited.end(), new_state)
                    == visited.end()) {
                    works.emplace(new_state);
                }
            }
        }

        for (auto st : current_states) {
            if (is_terminate_state(st)) {
                builder.add_terminate_state(sf.get(current_states));
                break;
            }
        }
    }

    out = builder.build();
}

std::unordered_set<nfa_t::state_t> nfa_t::next_state(
    const std::unordered_set<nfa_t::state_t>& sts, charcode_t c) const
{
    std::unordered_set<nfa_t::state_t> next_sts;
    for (auto& st : sts) {
        next_sts.merge(next_state(st, c));
    }
    return next_sts;
}

std::unordered_set<nfa_t::state_t> nfa_t::next_def_state(
    const std::unordered_set<nfa_t::state_t>& sts) const
{

    std::unordered_set<nfa_t::state_t> next_sts;
    for (auto& st : sts) {
        next_sts.merge(next_def_state(st));
    }
    return next_sts;
}

std::unordered_set<nfa_t::state_t> nfa_t::next_def_state(state_t st) const
{
    auto st_stransition = state_transition(st);
    return st_stransition.def;
}

std::unordered_set<nfa_t::state_t> nfa_t::next_state(
    state_t st, charcode_t c) const
{
    auto st_stransition = state_transition(st);
    auto actions = state_action(st_stransition, c);
    if (actions.size() > 0)
        return actions;
    return st_stransition.def;
}

std::unordered_set<nfa_t::charcode_t> nfa_t::all_acceptable_chars(
    state_t st) const
{
    std::unordered_set<nfa_t::charcode_t> chars;
    auto st_stransition = state_transition(st);
    for (auto& iter : st_stransition.actions) {
        chars.emplace(iter.first);
    }
    return chars;
}

std::unordered_set<nfa_t::charcode_t> nfa_t::all_acceptable_chars(
    const std::unordered_set<state_t>& sts) const
{
    std::unordered_set<nfa_t::charcode_t> chars;
    for (auto& st : sts)
        chars.merge(all_acceptable_chars(st));
    return chars;
}

std::unordered_set<nfa_t::state_t> nfa_t::continues_states(
    const std::unordered_set<state_t>& sts, bool recursive) const
{
    std::unordered_set<nfa_t::charcode_t> chars;
    for (auto& st : sts)
        chars.merge(continues_states(st, false));

    if (recursive) {
        auto chars2 = continues_states(chars, false);
        if (chars2 == chars)
            return chars;
        return continues_states(chars2, true);
    }

    return chars;
}

std::unordered_set<nfa_t::state_t> nfa_t::continues_states(
    state_t s, bool recursive) const
{
    if (recursive) {
        std::unordered_set<state_t> sts { s };
        return continues_states(sts, recursive);
    }

    auto st = state_transition(s);
    auto states = state_action(st, nil_char);
    states.emplace(s);
    return states;
}

std::unordered_set<nfa_t::state_t> nfa_t::state_action(
    const state_transition_t& st, charcode_t c) const
{
    auto iter = st.actions.find(c);
    if (iter != st.actions.end())
        return iter->second;
    return {};
}

nfa_t::state_transition_t nfa_t::state_transition(state_t s) const
{
    auto iter = transition_.find(s);
    if (iter != transition_.end()) {
        return iter->second;
    }
    return {};
}

void nfa_t::dot(std::ostream& os) const
{
    os << "digraph { ";

    char32_t csz[] = { 'A', '\0' };
    std::u32string sz(csz);

    for (auto& iter : transition_) {
        for (auto& iter2 : iter.second.actions) {
            for (auto& it : iter2.second) {
                os << iter.first;
                os << " -> ";
                os << it;

                if (iter2.first == nil_char) {
                    os << " [ label = \"empty\" ];";
                } else {
                    sz[0] = iter2.first;
                    os << " [ label = \"" << To_UTF8(sz) << "\" ];";
                }
            }
        }

        for (auto& it : iter.second.def) {
            os << iter.first;
            os << " -> ";
            os << it;
            os << " [ label = \"*\" ];";
        }
    }

    for (auto it : terminates_) {
        os << it << " [ shape = doublecircle ]; ";
    }

    os << init_state_ << "[ shape = box ]; ";

    os << "}";
}

nfa_t nfa_t::operator+(const nfa_t& other) const
{

    auto builder = builder_t();

    builder.set_init_state(0);

    state_t state_offset = 1;
    state_t max_state = 1;

    for (auto& iter : this->transition_) {
        for (auto& iter2 : iter.second.actions) {
            for (auto& it : iter2.second) {
                builder.add_action(
                    iter.first + state_offset, iter2.first, it + state_offset);
                max_state = std::max(max_state, iter.first);
                max_state = std::max(max_state, it);
            }
        }

        for (auto& it : iter.second.def) {
            builder.add_def_action(
                iter.first + state_offset, it + state_offset);
            max_state = std::max(max_state, it);
        }
    }

    for (auto it : this->terminates_) {
        builder.add_terminate_state(it + state_offset);
        max_state = std::max(max_state, it);
    }

    builder.add_action(0, nil_char, this->init_state_ + state_offset);
    state_offset += max_state + 1;

    for (auto& iter : other.transition_) {
        for (auto& iter2 : iter.second.actions) {
            for (auto& it : iter2.second) {
                builder.add_action(
                    iter.first + state_offset, iter2.first, it + state_offset);
                max_state = std::max(max_state, iter.first);
                max_state = std::max(max_state, it);
            }
        }

        for (auto& it : iter.second.def) {
            builder.add_def_action(
                iter.first + state_offset, it + state_offset);
            max_state = std::max(max_state, it);
        }
    }

    for (auto it : other.terminates_) {
        builder.add_terminate_state(it + state_offset);
        max_state = std::max(max_state, it);
    }

    builder.add_action(0, nil_char, other.init_state_ + state_offset);

    return builder.build();
}

nfa_t nfa_t::operator*(const nfa_t& other) const
{

    auto builder = builder_t();

    builder.set_init_state(0);

    state_t state_offset = 1;
    state_t max_state = 1;

    for (auto& iter : this->transition_) {
        for (auto& iter2 : iter.second.actions) {
            for (auto& it : iter2.second) {
                builder.add_action(
                    iter.first + state_offset, iter2.first, it + state_offset);
                max_state = std::max(max_state, iter.first);
                max_state = std::max(max_state, it);
            }
        }

        for (auto& it : iter.second.def) {
            builder.add_def_action(
                iter.first + state_offset, it + state_offset);
            max_state = std::max(max_state, it);
        }
    }

    for (auto it : this->terminates_) {
        max_state = std::max(max_state, it);
    }

    builder.add_action(0, nil_char, this->init_state_ + state_offset);

    auto mid = max_state + state_offset + 1;

    for (auto it : this->terminates_) {
        builder.add_action(it + state_offset, nil_char, mid);
    }

    state_offset = mid + 1;

    builder.add_action(mid, nil_char, other.init_state_ + state_offset);

    for (auto& iter : other.transition_) {
        for (auto& iter2 : iter.second.actions) {
            for (auto& it : iter2.second) {
                builder.add_action(
                    iter.first + state_offset, iter2.first, it + state_offset);
                max_state = std::max(max_state, iter.first);
                max_state = std::max(max_state, it);
            }
        }

        for (auto& it : iter.second.def) {
            builder.add_def_action(
                iter.first + state_offset, it + state_offset);
            max_state = std::max(max_state, it);
        }
    }

    for (auto it : other.terminates_) {
        builder.add_terminate_state(it + state_offset);
        max_state = std::max(max_state, it);
    }

    return builder.build();
}

} // namespace nxs::regex