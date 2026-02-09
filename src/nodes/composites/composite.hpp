#ifndef COMPOSITE_HPP
#define COMPOSITE_HPP

#include "../beehave_node.h"

namespace godot
{
    class Composite : public BeehaveNode
    {
        GDCLASS(Composite, BeehaveNode)

    public:
        Composite();
        ~Composite();

        virtual void interrupt(Node *actor, Blackboard *blackboard) override;
        virtual void after_run(Node *actor, Blackboard *blackboard) override;
        virtual PackedStringArray _get_configuration_warnings() const override;
        virtual TypedArray<StringName> get_class_name() const override;

    protected:
        static void _bind_methods();
        void _cleanup_running(Node *child, Node *actor, Blackboard *blackboard);
        void _interrupt_children(Node *actor, Blackboard *blackboard, int from_index, int last_index);

        BeehaveNode *running_child = nullptr;
    };

}

#endif // COMPOSITE_HPP
