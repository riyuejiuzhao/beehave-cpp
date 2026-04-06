#ifndef CONDITION_LEAF_HPP
#define CONDITION_LEAF_HPP

#include "leaf.hpp"

namespace godot
{
    class ConditionLeaf : public Leaf
    {
        GDCLASS(ConditionLeaf, Leaf)

    public:
        ConditionLeaf();
        ~ConditionLeaf();

        virtual TypedArray<StringName> get_class_name() const override;

    protected:
        static void _bind_methods();
        void after_tick(Node *actor, Blackboard *blackboard, int response) override;
    };

}

#endif // CONDITION_LEAF_HPP
