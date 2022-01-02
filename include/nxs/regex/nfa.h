#ifndef NXS_REGEX_NFA_H
#define NXS_REGEX_NFA_H

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <sstream>
#include <algorithm>

#include <nxs/regex/dfa.h>

namespace nxs::regex {

class dfa_t;

class nfa_t {
public:
    using charcode_t = uint32_t;
    using state_t = uint32_t;

    static constexpr state_t nil_state = 0xFFFFFFFF;
    static constexpr charcode_t nil_char = 0xFFFFFFFF;

    enum status_t {
        ACCEPT,
        REJECT,
        RESOLVE,
    };

    struct state_transition_t {
        std::unordered_set<state_t> def;
        std::unordered_map<charcode_t, std::unordered_set<state_t>> actions;
    };

    using transition_t = std::unordered_map<state_t, struct state_transition_t>;

    class builder_t {
    public:
        builder_t() { }
        ~builder_t() { }
        builder_t(const builder_t&) = default;
        builder_t& operator=(const builder_t&) = default;

        builder_t& set_init_state(state_t s)
        {
            init_state = s;
            return *this;
        }

        builder_t& add_terminate_state(state_t s)
        {
            terminates.emplace(s);
            return *this;
        }

        builder_t& add_action(state_t from, charcode_t c, state_t to)
        {
            auto iter = transition.find(from);
            if (iter == transition.end()) {
                transition[from] = {};
                iter = transition.find(from);
            }
            auto iter2 = iter->second.actions.find(c);
            if (iter2 == iter->second.actions.end()) {
                iter->second.actions[c] = {};
                iter2 = iter->second.actions.find(c);
            }
            iter2->second.emplace(to);
            return *this;
        }

        builder_t& add_def_action(state_t from, state_t to)
        {
            auto iter = transition.find(from);
            if (iter == transition.end()) {
                transition[from] = {};
                iter = transition.find(from);
            }
            iter->second.def.emplace(to);
            return *this;
        }

        nfa_t build() const { return nfa_t(*this); }

        friend class nfa_t;

    private:
        std::unordered_set<state_t> terminates;
        transition_t transition;
        state_t init_state;
    };

    nfa_t()
    : nfa_t(builder_t().set_init_state(1).add_terminate_state(1).build())
    {
    }
    nfa_t(const builder_t&);
    ~nfa_t();
    nfa_t(const nfa_t&) = default;
    nfa_t& operator=(const nfa_t&) = default;

    void to_dfa(dfa_t&) const;

    std::unordered_set<state_t> continues_states(
        state_t s, bool recursive = false) const;
    std::unordered_set<state_t> continues_states(
        const std::unordered_set<state_t>& sts, bool recursive = false) const;
    std::unordered_set<state_t> all_states() const;

    state_transition_t state_transition(state_t s) const;
    std::unordered_set<state_t> state_action(
        const state_transition_t& st, charcode_t c) const;

    bool is_terminate_state(state_t st) const;

    std::unordered_set<charcode_t> all_acceptable_chars(state_t st) const;
    std::unordered_set<charcode_t> all_acceptable_chars(
        const std::unordered_set<state_t>& sts) const;

    std::unordered_set<state_t> next_state(
        const std::unordered_set<state_t>& sts, charcode_t c) const;
    std::unordered_set<state_t> next_state(state_t st, charcode_t c) const;

    std::unordered_set<state_t> next_def_state(
        const std::unordered_set<state_t>& sts) const;
    std::unordered_set<state_t> next_def_state(state_t st) const;

    void dot(std::ostream& os) const;

    nfa_t operator+(const nfa_t& other) const;
    nfa_t operator*(const nfa_t& other) const;
    nfa_t repeat_zero_or_more() const;
    nfa_t repeat_one_or_more() const;
    nfa_t repeat(int n) const;
    nfa_t repeat(int start, int end) const;

private:
    class state_factory_t {
    public:
        state_factory_t() { next_state_ = 0; }
        ~state_factory_t() { }

        state_t get(const std::unordered_set<state_t>& states)
        {
            std::vector<state_t> aa;

            for (auto it : states)
                aa.push_back(it);

            std::sort(aa.begin(), aa.end());

            std::stringstream ss;
            for (auto& s : aa)
                ss << s << "-";
            auto key = ss.str();
            if (store_.find(key) == store_.end()) {
                store_[key] = next_state_++;
            }
            return store_[key];
        }

    private:
        std::map<std::string, state_t> store_;
        state_t next_state_;
    };

    std::unordered_set<state_t> terminates_;
    transition_t transition_;
    state_t init_state_;
};

} // namespace nxs::regex

#endif