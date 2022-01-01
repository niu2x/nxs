#ifndef NXS_REGEX_DFA_H
#define NXS_REGEX_DFA_H

#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace nxs::regex {

class nfa_t;

class dfa_t {
public:
    using charcode_t = uint32_t;
    using state_t = uint32_t;

    static constexpr state_t nil_state = 0xFFFFFFFF;

    enum status_t {
        ACCEPT,
        REJECT,
        RESOLVE,
    };

    struct state_transition_t {
        state_t def;
        std::unordered_map<charcode_t, state_t> actions;
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
                transition[from] = { .def = nil_state };
                iter = transition.find(from);
            }
            iter->second.actions[c] = to;
            return *this;
        }

        builder_t& add_def_action(state_t from, state_t to)
        {
            auto iter = transition.find(from);
            if (iter == transition.end()) {
                transition[from] = { .def = nil_state };
                iter = transition.find(from);
            }
            iter->second.def = to;
            return *this;
        }

        dfa_t build() const { return dfa_t(*this); }

        friend class dfa_t;

    private:
        std::unordered_set<state_t> terminates;
        transition_t transition;
        state_t init_state;
    };

    dfa_t()
    : dfa_t(builder_t().build())
    {
    }
    dfa_t(const builder_t&);
    ~dfa_t();
    dfa_t(const dfa_t&) = default;
    dfa_t& operator=(const dfa_t&) = default;

    void reset();

    state_t put(charcode_t c);

    state_t current_state() const { return current_state_; }

    void set_debug(bool debug) { debug_ = debug; }

    bool resolve() const;

    void to_nfa(nfa_t&) const;

private:
    std::unordered_set<state_t> terminates_;
    transition_t transition_;
    state_t current_state_;
    state_t init_state_;
    bool debug_;
};

} // namespace nxs::regex

#endif