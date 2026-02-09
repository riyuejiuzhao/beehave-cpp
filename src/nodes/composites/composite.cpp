#include "composite.hpp"
#include "../../util.hpp"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

Composite::Composite()
    : running_child(nullptr)
{
}

Composite::~Composite()
{
}

PackedStringArray Composite::_get_configuration_warnings() const
{
    PackedStringArray warnings = BeehaveNode::_get_configuration_warnings();

    TypedArray<Node> children = get_children();
    const int beehave_child_count = godot::count_if(children, [](Node *x)
                                                    { return Object::cast_to<BeehaveNode>(x) != nullptr; });
    if (beehave_child_count < 2)
        warnings.append("Any composite node should have at least two children. Otherwise it is not useful.");

    return warnings;
}

void Composite::interrupt(Node *actor, Blackboard *blackboard)
{
    if (running_child != nullptr)
    {
        running_child->interrupt(actor, blackboard);
        running_child = nullptr;
    }
    BeehaveNode::interrupt(actor, blackboard);
}

void Composite::after_run(Node *actor, Blackboard *blackboard)
{
    running_child = nullptr;
}

TypedArray<StringName> Composite::get_class_name() const
{
    TypedArray<StringName> classes = BeehaveNode::get_class_name();
    classes.push_back(StringName("Composite"));
    return classes;
}

void Composite::_cleanup_running(Node *child, Node *actor, Blackboard *blackboard)
{
    if (child != running_child)
        return;

    running_child = nullptr;
    String id = String::num_int64(actor->get_instance_id());
    Node *running_action = Object::cast_to<Node>(blackboard->get_value("running_action", Variant(), id));
    if (child == running_action)
        blackboard->set_value("running_action", Variant(), id);
}

void Composite::_interrupt_children(Node *actor, Blackboard *blackboard, int from_index, int last_index)
{
    TypedArray<Node> children = get_children();
    int start = from_index + 1;
    int end = 0;
    if (last_index > from_index)
        end = last_index + 1;
    else
        return;

    for (int j = start; j < end; j++)
    {
        BeehaveNode *stale = Object::cast_to<BeehaveNode>(children[j]);
        stale->interrupt(actor, blackboard);
    }
}

void Composite::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("_cleanup_running", "child", "actor", "blackboard"), &Composite::_cleanup_running);
    ClassDB::bind_method(D_METHOD("_interrupt_children", "actor", "blackboard", "from_index", "last_index"), &Composite::_interrupt_children);
}
