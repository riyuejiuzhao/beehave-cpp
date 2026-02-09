#ifndef ACTION_LEAF_HPP
#define ACTION_LEAF_HPP

#include "leaf.hpp"

namespace godot
{
    /// Actions are leaf nodes that define a task to be performed by an actor.
    /// Their execution can be long running, potentially being called across multiple
    /// frame executions. In this case, the node should return `RUNNING` until the
    /// action is completed.
    class ActionLeaf : public Leaf
    {
        GDCLASS(ActionLeaf, Leaf)

    public:
        ActionLeaf();
        ~ActionLeaf();

        virtual TypedArray<StringName> get_class_name() const override;

    protected:
        static void _bind_methods();
    };

}

#endif // ACTION_LEAF_HPP
