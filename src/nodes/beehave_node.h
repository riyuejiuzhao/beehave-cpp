#ifndef BEEHAVE_NODE_H
#define BEEHAVE_NODE_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/core/gdvirtual.gen.inc>
#include "../blackboard.h"

namespace godot
{

    class BeehaveNode : public Node
    {
        GDCLASS(BeehaveNode, Node)

    public:
        enum Status
        {
            SUCCESS = 0,
            FAILURE = 1,
            RUNNING = 2
        };

        BeehaveNode();
        ~BeehaveNode();

        // Main behavior tree methods
        int safe_tick(Node *actor, Blackboard *blackboard);
        virtual void interrupt(Node *actor, Blackboard *blackboard);
        virtual void before_run(Node *actor, Blackboard *blackboard);
        virtual void after_run(Node *actor, Blackboard *blackboard);

        // Utility methods
        virtual TypedArray<StringName> get_class_name() const;
        bool can_send_message(Blackboard *blackboard);
        PackedStringArray _get_configuration_warnings() const override;

    protected:
        static void _bind_methods();
        virtual int tick(Node *actor, Blackboard *blackboard);
        virtual void after_tick(Node *actor, Blackboard *blackboard, int response);

        GDVIRTUAL2R(int, _tick, Node *, Blackboard *);
    };

}

VARIANT_ENUM_CAST(godot::BeehaveNode::Status);

#endif // BEEHAVE_NODE_H
