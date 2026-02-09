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
    };

}

#endif // CONDITION_LEAF_HPP
