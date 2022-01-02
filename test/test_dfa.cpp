#include <iostream>
#include <nxs/regex/regex.h>

int main()
{
    auto nfa = nxs::regex::nfa_t::builder_t()
                   .set_init_state(1)
                   .add_terminate_state(4)
                   .add_action(1, '0', 1)
                   .add_action(1, '1', 1)
                   .add_action(1, '1', 2)
                   .add_action(2, '0', 3)
                   .add_action(2, '1', 3)
                   .add_action(3, '0', 4)
                   .add_action(3, '1', 4)
                   .build();
    auto dfa = nxs::regex::to_dfa(nfa);
    // dfa.set_debug(true);
    //
    // dfa.put('1');
    // dfa.put('1');
    // dfa.put('1');
    // dfa.put('1');
    // dfa.put('1');
    // dfa.put('1');
    // dfa.put('1');
    // dfa.put('1');
    // dfa.put('1');
    // dfa.put('1');
    // dfa.put('1');
    // dfa.put('0');
    // dfa.put('0');
    // dfa.put('1');
    //

    // auto dfa = nxs::regex::dfa_t::builder_t()
    //         .set_init_state(1)
    //         .add_action(1, '0', 1)
    //         .add_action(1, '1', 2)
    //         .add_action(2, '1', 2)
    //         .add_action(2, '0', 3)
    //         .add_action(3, '0', 2)
    //         .add_action(3, '1', 2)
    //         // .add_def_action(2, 3)
    //         .add_terminate_state(2)
    //         .build();

    // dfa.dot(std::cout);
    dfa.dot(std::cout);

    // dfa.set_debug(true);
    // dfa.put('0');
    // dfa.put('1');
    // dfa.put('5');
    // dfa.put('0');
    // dfa.put('1');
    // dfa.put('3');
    // dfa.put('1');

    return 0;
}