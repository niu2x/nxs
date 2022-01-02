#ifndef NXS_REGEX_EXPRESS_VM_H
#define NXS_REGEX_EXPRESS_VM_H

#include <stack>
#include <cassert>

namespace nxs::regex {

template <class EXP, class OP, class OP_CMP, class EVAL>

class express_vm_t {
public:
    express_vm_t() { }
    ~express_vm_t() { }

    void push(const OP& op, const EXP& exp)
    {
        while (!(vm_.top().op_stack.empty()
            || OP_CMP::less_equal(vm_.top().op_stack.top(), op))) {

            auto a = vm_.top().exp_stack.top();
            vm_.top().exp_stack.pop();
            auto b = vm_.top().exp_stack.top();
            vm_.top().exp_stack.pop();

            auto op = vm_.top().op_stack.top();
            vm_.top().op_stack.pop();

            vm_.top().exp_stack.push(EVAL::eval(a, b, op));
        }
        vm_.top().op_stack.push(op);
        vm_.top().exp_stack.push(exp);
    }

    void push(const OP& op)
    {
        assert(vm_.top().op_turn);
        while (!(vm_.top().op_stack.empty()
            || OP_CMP::less_equal(vm_.top().op_stack.top(), op))) {

            auto a = vm_.top().exp_stack.top();
            vm_.top().exp_stack.pop();
            auto b = vm_.top().exp_stack.top();
            vm_.top().exp_stack.pop();

            auto op = vm_.top().op_stack.top();
            vm_.top().op_stack.pop();

            vm_.top().exp_stack.push(EVAL::eval(a, b, op));
        }
        vm_.top().op_stack.push(op);
        vm_.top().op_turn = false;
    }

    void push(const EXP& exp)
    {
        if ((vm_.top().op_turn))
            push(def_op_);
        vm_.top().exp_stack.push(exp);
        vm_.top().op_turn = true;
    }

    void push_level(const EXP& exp)
    {
        vm_.push({ .op_turn = true });
        vm_.top().exp_stack.push(exp);
    }

    EXP pop_level()
    {

        while (!(vm_.top().op_stack.empty())) {

            auto a = vm_.top().exp_stack.top();
            vm_.top().exp_stack.pop();
            auto b = vm_.top().exp_stack.top();
            vm_.top().exp_stack.pop();

            auto op = vm_.top().op_stack.top();
            vm_.top().op_stack.pop();

            vm_.top().exp_stack.push(EVAL::eval(b, a, op));
        }

        auto result = vm_.top().exp_stack.top();
        vm_.top().exp_stack.pop();

        vm_.pop();

        return result;
    }

    void clear() { vm_ = {}; }

    EXP top_exp() const { return vm_.top().exp_stack.top(); }

    void set_top_exp(const EXP& exp) { vm_.top().exp_stack.top() = exp; }

    void set_default_op(const OP& op) { def_op_ = op; }

private:
    struct express_stack_t {
        std::stack<EXP> exp_stack;
        std::stack<OP> op_stack;
        bool op_turn;
    };
    std::stack<express_stack_t> vm_;
    OP def_op_;
};

} // namespace nxs::regex

#endif