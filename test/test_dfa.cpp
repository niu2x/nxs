#include <iostream>
#include <nxs/regex/regex.h>

int main()
{
    // auto nfa1 = nxs::regex::nfa_t::builder_t()
    //                 .set_init_state(1)
    //                 .add_terminate_state(4)
    //                 .add_action(1, '0', 1)
    //                 .add_action(1, '1', 1)
    //                 .add_action(1, '1', 2)
    //                 .add_action(2, '0', 3)
    //                 .add_action(2, '1', 3)
    //                 .add_action(3, '0', 4)
    //                 .add_action(3, '1', 4)
    //                 .build();

    // auto nfa2 = nxs::regex::nfa_t::builder_t()
    //                 .set_init_state(1)
    //                 .add_terminate_state(4)
    //                 .add_def_action(1, 3)
    //                 .add_action(1, -1, 2)
    //                 .add_action(2, 'A', 3)
    //                 .add_action(3, 'B', 4)
    //                 .build();
    //
    // auto nfa1 = nxs::regex::nfa_t::builder_t()
    //                 .set_init_state(1)
    //                 .add_terminate_state(4)
    //                 .add_def_action(1, 3)
    //                 .add_action(1, -1, 2)
    //                 .add_action(2, 'A', 3)
    //                 .add_action(3, 'B', 4)
    //                 .build();

    // auto nfa2 = nxs::regex::nfa_t::builder_t()
    //                 .set_init_state(1)
    //                 .add_terminate_state(4)
    //                 .add_def_action(1, 3)
    //                 .add_action(1, -1, 2)
    //                 .add_action(2, 'A', 3)
    //                 .add_action(3, 'B', 4)
    //                 .build();

    // auto dfa = nxs::regex::to_dfa(nfa);
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

    // auto nfa = nfa1 * nfa2;
    // nfa.dot(std::cout);

    nxs::regex::dfa_t dfa = nxs::regex::dfa_t::builder_t()
                                .set_init_state(1)
                                .add_terminate_state(0)
                                .add_action(1, 'A', 2)
                                .add_action(1, 'B', 3)

                                .add_action(2, 'D', 0)
                                .add_action(3, 'F', 0)

                                .build()
                                .optimize();

    ;
    dfa.dot(std::cout);

    // dfa.set_debug(true);
    // dfa.put('0');
    // dfa.put('1');
    // dfa.put('0');
    // dfa.put('0');
    // dfa.put('1');
    // dfa.put('1');
    // dfa.put('1');

    return 0;
}