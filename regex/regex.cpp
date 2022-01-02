#include <nxs/regex/regex.h>

#include <stack>
#include <cassert>
#include <iostream>

#include "express_vm.h"
#include "utils.h"

namespace nxs::regex {

std::string To_UTF8(const std::u32string& s)
{
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
    return conv.to_bytes(s);
}

std::u32string To_UTF32(const std::string& s)
{
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
    return conv.from_bytes(s);
}

static bool readchar(std::u32string& sz, char32_t& c, bool& plain)
{
    c = sz[0];
    sz = sz.substr(1);

    plain = !(c == '+' || c == '*' || c == '[' || c == ']' || c == '('
        || c == ')' || c == '.' || c == '|' || c == '?' || c == '-');

    if (c == '\\') {
        plain = true;
        if (sz.size() == 0)
            return false;
        c = sz[0];
        sz = sz.substr(1);
        if (c == 't')
            c = '\t';
        if (c == 'n')
            c = '\n';
        if (c == 'r')
            c = '\r';
    }

    return true;
}

bool parse(const std::string& sz, dfa_t* out)
{
    auto u32 = To_UTF32(sz);

    struct op_cmp_t {
        static bool less_equal(int a, int b) { return a <= b; }
    };

    enum op_t {
        concat = 0,
        logic_or = 1,
    };

    struct eval_t {
        static nfa_t eval(const nfa_t& a, const nfa_t& b, int op)
        {
            if (op == concat)
                return to_nfa(to_dfa(a * b));
            if (op == logic_or)
                return to_nfa(to_dfa(a + b));
            assert(false);
        }
    };

    express_vm_t<nfa_t, int, op_cmp_t, eval_t> vm;

    vm.clear();
    vm.push_level(
        nfa_t::builder_t().set_init_state(1).add_terminate_state(1).build());

    vm.set_default_op(concat);

    char32_t chr;
    bool plain;

    while (u32.size() > 0) {

        bool success = readchar(u32, chr, plain);
        if (!success)
            return false;

        if (plain) {
            auto builder = nfa_t::builder_t();
            builder.set_init_state(1);
            builder.add_action(1, chr, 2);
            builder.add_terminate_state(2);

            vm.push(builder.build());
        } else {

            switch (chr) {
                case '*': {
                    auto nfa = vm.top_exp();
                    nfa = nfa.repeat_zero_or_more();
                    vm.set_top_exp(nfa);
                    break;
                }
                case '+': {
                    auto nfa = vm.top_exp();
                    nfa = nfa.repeat_one_or_more();
                    vm.set_top_exp(nfa);
                    break;
                }

                case '?': {
                    auto nfa = vm.top_exp();
                    nfa = nfa.repeat(0, 2);
                    vm.set_top_exp(nfa);
                    break;
                }

                case '.': {
                    auto builder = nfa_t::builder_t();
                    builder.set_init_state(1);
                    builder.add_def_action(1, 2);
                    builder.add_terminate_state(2);
                    vm.push(builder.build());

                    break;
                }

                case '|': {
                    vm.push(logic_or);
                    break;
                }

                case '(': {
                    vm.push_level(nfa_t::builder_t()
                                      .set_init_state(1)
                                      .add_terminate_state(1)
                                      .build());

                    break;
                }

                case ')': {
                    auto exp = vm.pop_level();
                    vm.push(exp);
                    break;
                }

                case '[': {

                    vm.push_level(nfa_t::builder_t().set_init_state(1).build());

                    do {

                        bool success = readchar(u32, chr, plain);
                        if (!success)
                            return false;

                        if (plain) {

                            if (u32.size() > 1 && u32[0] == '-') {
                                auto a = chr;

                                success = readchar(u32, chr, plain);
                                if (!success)
                                    return false;

                                success = readchar(u32, chr, plain);
                                if (!success)
                                    return false;

                                if (!plain)
                                    return false;

                                for (char32_t c = a; c <= chr; c++) {
                                    auto builder = nfa_t::builder_t();
                                    builder.set_init_state(1);
                                    builder.add_action(1, c, 2);
                                    builder.add_terminate_state(2);
                                    vm.push(logic_or, builder.build());
                                }

                            } else {
                                auto builder = nfa_t::builder_t();
                                builder.set_init_state(1);
                                builder.add_action(1, chr, 2);
                                builder.add_terminate_state(2);
                                vm.push(logic_or, builder.build());
                            }

                        } else {
                        }
                    } while (!((!plain) && chr == ']'));

                    vm.push(vm.pop_level());

                    break;
                }

                default: {
                    return false;
                }
            }
        }
    }

    auto result = vm.pop_level();
    // result.dot(std::cout);
    *out = to_dfa(result);
    return true;
}

} // namespace nxs::regex