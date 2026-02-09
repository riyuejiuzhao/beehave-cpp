#include "beehave_tree.h"
#include "beehave_node.h"
#include "../blackboard.h"
#include "../debug/debugger_messages.h"
#include "../metrics/beehave_metrics.h"
#include "leaves/action_leaf.hpp"
#include "leaves/condition_leaf.hpp"
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/performance.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/time.hpp>
#include <godot_cpp/core/error_macros.hpp>
#include <godot_cpp/core/object.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

BeehaveTree::BeehaveTree()
{
    enabled = true;
    tick_rate = 1;
    actor_node_path = NodePath();
    process_thread = ProcessThread::PHYSICS;
    blackboard = nullptr;
    custom_monitor = false;
    actor = nullptr;
    status = -1;
    last_tick = -1;
    _internal_blackboard = nullptr;
    _process_time_metric_value = 0.0;
    _can_send_message = false;
}

BeehaveTree::~BeehaveTree()
{
}

void BeehaveTree::set_enabled(bool p_enabled)
{
    enabled = p_enabled;
    set_physics_process(enabled && process_thread == ProcessThread::PHYSICS);
    set_process(enabled && process_thread == ProcessThread::IDLE);

    if (enabled)
    {
        emit_signal("tree_enabled");
    }
    else
    {
        interrupt();
        emit_signal("tree_disabled");
    }
}

void BeehaveTree::set_tick_rate(int p_rate)
{
    tick_rate = p_rate;
}

void BeehaveTree::set_actor_node_path(const NodePath &p_path)
{
    actor_node_path = p_path;
    if (!actor_node_path.is_empty() && String(actor_node_path) != "..")
    {
        actor = get_node_or_null(actor_node_path);
    }
    else
    {
        actor = get_parent();
    }

    if (Engine::get_singleton()->is_editor_hint())
    {
        update_configuration_warnings();
    }
}

void BeehaveTree::set_process_thread(ProcessThread p_thread)
{
    process_thread = p_thread;
    set_enabled(enabled && process_thread != ProcessThread::MANUAL);
    set_physics_process(enabled && process_thread == ProcessThread::PHYSICS);
    set_process(enabled && process_thread == ProcessThread::IDLE);
}

void BeehaveTree::set_blackboard(Blackboard *p_blackboard)
{
    blackboard = p_blackboard;

    if (blackboard && _internal_blackboard)
    {
        remove_child(_internal_blackboard);
        _internal_blackboard->queue_free();
        _internal_blackboard = nullptr;
    }
    else if (!blackboard && !_internal_blackboard)
    {
        _internal_blackboard = memnew(Blackboard);
        add_child(_internal_blackboard, false, Node::INTERNAL_MODE_BACK);
    }
}

Blackboard *BeehaveTree::get_blackboard()
{
    if (!blackboard && !_internal_blackboard)
    {
        _internal_blackboard = memnew(Blackboard);
        add_child(_internal_blackboard, false, Node::INTERNAL_MODE_BACK);
    }
    return blackboard ? blackboard : _internal_blackboard;
}

void BeehaveTree::set_custom_monitor(bool p_enabled)
{
    custom_monitor = p_enabled;

    if (custom_monitor && !_process_time_metric_name.is_empty())
    {
        Performance::get_singleton()->add_custom_monitor(
            _process_time_metric_name,
            callable_mp(this, &BeehaveTree::_get_process_time_metric_value));

        BeehaveMetrics *metrics = BeehaveMetrics::get_singleton();
        if (metrics)
        {
            metrics->register_tree(this);
        }
    }
    else
    {
        if (!_process_time_metric_name.is_empty())
        {
            Performance::get_singleton()->remove_custom_monitor(_process_time_metric_name);

            BeehaveMetrics *metrics = BeehaveMetrics::get_singleton();
            if (metrics)
            {
                metrics->unregister_tree(this);
            }
        }

        // TODO: BeehaveDebuggerMessages::unregister_tree(get_instance_id());
    }
}

void BeehaveTree::set_actor(Node *p_actor)
{
    actor = p_actor;
    if (actor == nullptr)
    {
        actor = get_parent();
    }

    if (Engine::get_singleton()->is_editor_hint())
    {
        update_configuration_warnings();
    }
}

void BeehaveTree::set_can_send_message(bool p_enabled)
{
    _can_send_message = p_enabled;
}

void BeehaveTree::_ready()
{
    SceneTree *tree = get_tree();
    if (tree)
    {
        // Connect scene tree signals
        if (!tree->is_connected("node_added", callable_mp(this, &BeehaveTree::_on_scene_tree_node_added_removed).bind(true)))
        {
            tree->connect("node_added", callable_mp(this, &BeehaveTree::_on_scene_tree_node_added_removed).bind(true));
        }
        if (!tree->is_connected("node_removed", callable_mp(this, &BeehaveTree::_on_scene_tree_node_added_removed).bind(false)))
        {
            tree->connect("node_removed", callable_mp(this, &BeehaveTree::_on_scene_tree_node_added_removed).bind(false));
        }
    }

    if (process_thread == 0) // Default to PHYSICS if not set
    {
        process_thread = ProcessThread::PHYSICS;
    }

    if (!actor)
    {
        if (!actor_node_path.is_empty())
        {
            actor = get_node_or_null(actor_node_path);
        }
        else
        {
            actor = get_parent();
        }
    }

    if (!blackboard)
    {
        set_blackboard(nullptr); // Trigger internal blackboard creation
    }

    // Setup metric name
    if (actor)
    {
        _process_time_metric_name = "beehave [microseconds]/process_time_" +
                                    actor->get_name() + "-" +
                                    itos(get_instance_id());
    }

    set_physics_process(enabled && process_thread == ProcessThread::PHYSICS);
    set_process(enabled && process_thread == ProcessThread::IDLE);

    // Register custom monitor
    if (custom_monitor && !Engine::get_singleton()->is_editor_hint())
    {
        Performance::get_singleton()->add_custom_monitor(
            _process_time_metric_name,
            callable_mp(this, &BeehaveTree::_get_process_time_metric_value));

        BeehaveMetrics *metrics = BeehaveMetrics::get_singleton();
        if (metrics)
        {
            metrics->register_tree(this);
        }
    }

    if (Engine::get_singleton()->is_editor_hint())
    {
        call_deferred("update_configuration_warnings");
    }
    else
    {
        // TODO: Register with global debugger
        // _get_global_debugger()->register_tree(this);
        // BeehaveDebuggerMessages::register_tree(_get_debugger_data(this));
    }
}

void BeehaveTree::_on_scene_tree_node_added_removed(Node *p_node, bool p_is_added)
{
    if (Engine::get_singleton()->is_editor_hint())
    {
        return;
    }

    BeehaveNode *beehave_node = Object::cast_to<BeehaveNode>(p_node);
    if (beehave_node && is_ancestor_of(p_node))
    {
        if (p_is_added)
        {
            // TODO: Register tree with debugger when node is ready
            // beehave_node->connect("ready", callable for registering tree, CONNECT_ONE_SHOT);
        }
        else
        {
            // TODO: Unregister and request ready
            // BeehaveDebuggerMessages::unregister_tree(get_instance_id());
            // request_ready();
        }
    }
}

void BeehaveTree::_physics_process(double p_delta)
{
    tick();
}

void BeehaveTree::_process(double p_delta)
{
    tick();
}

int BeehaveTree::tick()
{
    if (Engine::get_singleton()->is_editor_hint())
    {
        return -1;
    }

    if (last_tick != -1 && last_tick < tick_rate - 1)
    {
        last_tick += 1;
        return -1;
    }

    last_tick = 0;

    // Start timing for metric
    uint64_t start_time = Time::get_singleton()->get_ticks_usec();

    Blackboard *bb = get_blackboard();
    bb->set_value("can_send_message", _can_send_message);

    // if (_can_send_message && !Engine::get_singleton()->is_editor_hint())
    // {
    //     GD_LOG_INFO("process_begin, instance_id={0}, blackboard={1}", 
    //         get_instance_id(), bb->get_debug_data());
    // }

    if (actor == nullptr || get_child_count() == 0)
    {
        return BeehaveNode::FAILURE;
    }

    BeehaveNode *child = Object::cast_to<BeehaveNode>(get_child(0));
    if (child == nullptr)
    {
        return BeehaveNode::FAILURE;
    }

    if (status != BeehaveNode::RUNNING)
    {
        child->before_run(actor, bb);
    }

    status = child->safe_tick(actor, bb);

    // if (_can_send_message)
    // {
    //     GD_LOG_INFO("process_tick, child_id={0}, instance_id={1}, status={2}, blackboard_data={3}",
    //                 child->get_instance_id(), get_instance_id(), status, bb->get_debug_data());
    // }

    // Clear running action if nothing is running
    if (status != BeehaveNode::RUNNING)
    {
        bb->set_value("running_action", Variant(), itos(actor->get_instance_id()));
        child->after_run(actor, bb);
    }

    // if (_can_send_message && !Engine::get_singleton()->is_editor_hint())
    // {
    //     GD_LOG_INFO("process_end, instance_id={0}, blackboard={1}", get_instance_id(), bb->get_debug_data());
    // }

    // Check the cost for this frame and save it for metric report
    _process_time_metric_value = Time::get_singleton()->get_ticks_usec() - start_time;

    return status;
}

PackedStringArray BeehaveTree::_get_configuration_warnings() const
{
    PackedStringArray warnings;

    if (actor == nullptr)
    {
        warnings.push_back("Configure target node on tree");
    }

    TypedArray<Node> children = get_children();
    for (int i = 0; i < children.size(); i++)
    {
        Node *child = Object::cast_to<Node>(children[i]);
        if (child && !Object::cast_to<BeehaveNode>(child))
        {
            warnings.push_back("All children of this node should inherit from BeehaveNode class.");
            break;
        }
    }

    if (get_child_count() != 1)
    {
        warnings.push_back("BeehaveTree should have exactly one child node.");
    }

    return warnings;
}

ActionLeaf *BeehaveTree::get_running_action()
{
    Blackboard *bb = get_blackboard();
    if (actor)
    {
        Variant value = bb->get_value("running_action", Variant(), itos(actor->get_instance_id()));
        return Object::cast_to<ActionLeaf>(value);
    }
    return nullptr;
}

ConditionLeaf *BeehaveTree::get_last_condition()
{
    Blackboard *bb = get_blackboard();
    if (actor)
    {
        Variant value = bb->get_value("last_condition", Variant(), itos(actor->get_instance_id()));
        return Object::cast_to<ConditionLeaf>(value);
    }
    return nullptr;
}

String BeehaveTree::get_last_condition_status()
{
    Blackboard *bb = get_blackboard();
    if (actor && bb->has_value("last_condition_status", itos(actor->get_instance_id())))
    {
        int cond_status = bb->get_value("last_condition_status", Variant(), itos(actor->get_instance_id()));
        if (cond_status == BeehaveNode::SUCCESS)
        {
            return "SUCCESS";
        }
        else if (cond_status == BeehaveNode::FAILURE)
        {
            return "FAILURE";
        }
        else if (cond_status == BeehaveNode::RUNNING)
        {
            return "RUNNING";
        }
    }
    return "";
}

void BeehaveTree::interrupt()
{
    if (get_child_count() != 0)
    {
        BeehaveNode *first_child = Object::cast_to<BeehaveNode>(get_child(0));
        if (first_child)
        {
            first_child->interrupt(actor, get_blackboard());
        }
    }
}

void BeehaveTree::enable()
{
    set_enabled(true);
}

void BeehaveTree::disable()
{
    set_enabled(false);
}

void BeehaveTree::_exit_tree()
{
    if (Engine::get_singleton()->is_editor_hint())
    {
        return;
    }

    if (custom_monitor)
    {
        if (!_process_time_metric_name.is_empty())
        {
            Performance::get_singleton()->remove_custom_monitor(_process_time_metric_name);
        }

        BeehaveMetrics *metrics = BeehaveMetrics::get_singleton();
        if (metrics)
        {
            metrics->unregister_tree(this);
        }

        // TODO: BeehaveDebuggerMessages::unregister_tree(get_instance_id());
    }
}

int BeehaveTree::_get_process_time_metric_value()
{
    return static_cast<int>(_process_time_metric_value);
}

Dictionary BeehaveTree::_get_debugger_data(Node *p_node)
{
    BeehaveTree *tree = Object::cast_to<BeehaveTree>(p_node);
    BeehaveNode *node = Object::cast_to<BeehaveNode>(p_node);

    if (!tree && !node)
    {
        return Dictionary();
    }

    Dictionary data;
    data["path"] = p_node->get_path();
    data["name"] = p_node->get_name();

    if (tree)
    {
        TypedArray<StringName> class_names = tree->get_class_name();
        if (class_names.size() > 0)
        {
            data["type"] = class_names[0];
        }
    }
    else if (node)
    {
        TypedArray<StringName> class_names = node->get_class_name();
        if (class_names.size() > 0)
        {
            data["type"] = class_names[0];
        }
    }

    data["id"] = itos(p_node->get_instance_id());

    if (p_node->get_child_count() > 0)
    {
        Array children;
        for (int i = 0; i < p_node->get_child_count(); i++)
        {
            Node *child = p_node->get_child(i);
            Dictionary child_data = _get_debugger_data(child);
            if (!child_data.is_empty())
            {
                children.push_back(child_data);
            }
        }
        data["children"] = children;
    }

    return data;
}

TypedArray<StringName> BeehaveTree::get_class_name() const
{
    TypedArray<StringName> classes;
    classes.push_back(StringName("BeehaveTree"));
    return classes;
}

void BeehaveTree::_bind_methods()
{
    // Bind property setters/getters
    ClassDB::bind_method(D_METHOD("set_enabled", "enabled"), &BeehaveTree::set_enabled);
    ClassDB::bind_method(D_METHOD("get_enabled"), &BeehaveTree::get_enabled);
    ClassDB::bind_method(D_METHOD("set_tick_rate", "rate"), &BeehaveTree::set_tick_rate);
    ClassDB::bind_method(D_METHOD("get_tick_rate"), &BeehaveTree::get_tick_rate);
    ClassDB::bind_method(D_METHOD("set_actor_node_path", "path"), &BeehaveTree::set_actor_node_path);
    ClassDB::bind_method(D_METHOD("get_actor_node_path"), &BeehaveTree::get_actor_node_path);
    ClassDB::bind_method(D_METHOD("set_process_thread", "thread"), &BeehaveTree::set_process_thread);
    ClassDB::bind_method(D_METHOD("get_process_thread"), &BeehaveTree::get_process_thread);
    ClassDB::bind_method(D_METHOD("set_blackboard", "blackboard"), &BeehaveTree::set_blackboard);
    ClassDB::bind_method(D_METHOD("get_blackboard"), &BeehaveTree::get_blackboard);
    ClassDB::bind_method(D_METHOD("set_custom_monitor", "enabled"), &BeehaveTree::set_custom_monitor);
    ClassDB::bind_method(D_METHOD("get_custom_monitor"), &BeehaveTree::get_custom_monitor);
    ClassDB::bind_method(D_METHOD("set_actor", "actor"), &BeehaveTree::set_actor);
    ClassDB::bind_method(D_METHOD("get_actor"), &BeehaveTree::get_actor);
    ClassDB::bind_method(D_METHOD("set_can_send_message", "enabled"), &BeehaveTree::set_can_send_message);
    ClassDB::bind_method(D_METHOD("get_can_send_message"), &BeehaveTree::get_can_send_message);

    // Bind main methods
    ClassDB::bind_method(D_METHOD("tick"), &BeehaveTree::tick);
    ClassDB::bind_method(D_METHOD("interrupt"), &BeehaveTree::interrupt);
    ClassDB::bind_method(D_METHOD("enable"), &BeehaveTree::enable);
    ClassDB::bind_method(D_METHOD("disable"), &BeehaveTree::disable);

    // Bind query methods
    ClassDB::bind_method(D_METHOD("get_running_action"), &BeehaveTree::get_running_action);
    ClassDB::bind_method(D_METHOD("get_last_condition"), &BeehaveTree::get_last_condition);
    ClassDB::bind_method(D_METHOD("get_last_condition_status"), &BeehaveTree::get_last_condition_status);
    ClassDB::bind_method(D_METHOD("get_class_name"), &BeehaveTree::get_class_name);

    // Bind internal methods needed for callables
    ClassDB::bind_method(D_METHOD("_on_scene_tree_node_added_removed", "node", "is_added"), &BeehaveTree::_on_scene_tree_node_added_removed);
    ClassDB::bind_method(D_METHOD("_get_process_time_metric_value"), &BeehaveTree::_get_process_time_metric_value);

    // Add properties
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "enabled", PROPERTY_HINT_NONE), "set_enabled", "get_enabled");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "tick_rate", PROPERTY_HINT_RANGE, "1,100,1"), "set_tick_rate", "get_tick_rate");
    ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "actor_node_path", PROPERTY_HINT_NODE_PATH_VALID_TYPES, "Node"), "set_actor_node_path", "get_actor_node_path");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "process_thread", PROPERTY_HINT_ENUM, "Idle,Physics,Manual"), "set_process_thread", "get_process_thread");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "blackboard", PROPERTY_HINT_NODE_TYPE, "Blackboard"), "set_blackboard", "get_blackboard");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "custom_monitor", PROPERTY_HINT_NONE), "set_custom_monitor", "get_custom_monitor");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "actor", PROPERTY_HINT_NODE_TYPE, "Node"), "set_actor", "get_actor");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "can_send_message", PROPERTY_HINT_NONE), "set_can_send_message", "get_can_send_message");

    // Add signals
    ADD_SIGNAL(MethodInfo("tree_enabled"));
    ADD_SIGNAL(MethodInfo("tree_disabled"));

    // Bind ProcessThread enum
    BIND_ENUM_CONSTANT(IDLE);
    BIND_ENUM_CONSTANT(PHYSICS);
    BIND_ENUM_CONSTANT(MANUAL);
}