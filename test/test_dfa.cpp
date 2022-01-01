#include <nxs/regex/regex.h>

int main()
{
    auto dfa = nxs::regex::dfa_t::builder_t()
                   .set_init_state(1)
                   .add_terminate_state(1)
                   .add_action(1, '0', 1)
                   .add_action(1, '1', 2)
                   .add_action(2, '1', 2)
                   .add_action(2, '0', 1)
                   .build();

    dfa.set_debug(true);

    dfa.put('1');
    dfa.put('1');
    dfa.put('0');
    dfa.put('1');
    dfa.put('0');
    dfa.put('0');
    dfa.put('0');
    dfa.put('0');
    dfa.put('0');
    dfa.put('0');
    dfa.put('0');
    dfa.put('0');
    dfa.put('0');
    dfa.put('0');
    dfa.put('1');
    dfa.put('0');

    return 0;
}