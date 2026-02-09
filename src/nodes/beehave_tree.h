#ifndef BEEHAVETREE_H
#define BEEHAVETREE_H

#include "../blackboard.h"
#include "beehave_node.h"
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/variant/node_path.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/classes/node.hpp>

namespace godot
{
    class ActionLeaf;
    class ConditionLeaf;

    class BeehaveTree : public Node
    {
        GDCLASS(BeehaveTree, Node)

    public:

        enum ProcessThread
        {
            IDLE = 0,
            PHYSICS = 1,
            MANUAL = 2
        };

        BeehaveTree();
        ~BeehaveTree();

        // Property setters/getters
        void set_enabled(bool p_enabled);
        bool get_enabled() const { return enabled; }

        void set_tick_rate(int p_rate);
        int get_tick_rate() const { return tick_rate; }

        void set_actor_node_path(const NodePath &p_path);
        NodePath get_actor_node_path() const { return actor_node_path; }

        void set_process_thread(ProcessThread p_thread);
        ProcessThread get_process_thread() const { return process_thread; }

        void set_blackboard(Blackboard *p_blackboard);
        Blackboard *get_blackboard();

        void set_custom_monitor(bool p_enabled);
        bool get_custom_monitor() const { return custom_monitor; }

        void set_actor(Node *p_actor);
        Node *get_actor() const { return actor; }

        void set_can_send_message(bool p_enabled);
        bool get_can_send_message() const { return _can_send_message; }

        // Main methods
        int tick();
        void interrupt();
        void enable();
        void disable();

        // Query methods
        ActionLeaf *get_running_action();
        ConditionLeaf *get_last_condition();
        String get_last_condition_status();

        // Overrides
        PackedStringArray _get_configuration_warnings() const override;
        TypedArray<StringName> get_class_name() const;

        // Godot virtual functions (must be public for registration)
        void _ready();
        void _physics_process(double p_delta);
        void _process(double p_delta);
        void _exit_tree();

    private:
        // Properties
        bool enabled;
        int tick_rate;
        NodePath actor_node_path;
        ProcessThread process_thread;
        Blackboard *blackboard;
        bool custom_monitor;
        Node *actor;

        // State
        int status;
        int last_tick;

        // Internal
        Blackboard *_internal_blackboard;
        String _process_time_metric_name;
        double _process_time_metric_value;
        bool _can_send_message;

        // Helper methods
        void _on_scene_tree_node_added_removed(Node *p_node, bool p_is_added);
        int _get_process_time_metric_value();
        Dictionary _get_debugger_data(Node *p_node);

    protected:
        static void _bind_methods();
    };
}

VARIANT_ENUM_CAST(godot::BeehaveTree::ProcessThread);

#endif
