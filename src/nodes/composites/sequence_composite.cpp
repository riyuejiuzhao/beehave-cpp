#include "sequence_composite.hpp"
#include "../leaves/action_leaf.hpp"
#include "../leaves/condition_leaf.hpp"
#include "../../debug/debugger_messages.h"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

SequenceComposite::SequenceComposite()
    : successful_index(0), previous_failure_or_running_index(-1)
{
}

SequenceComposite::~SequenceComposite()
{
}

int SequenceComposite::tick(Node *actor, Blackboard *blackboard)
{
    TypedArray<Node> children = get_children();

    for (int i = 0; i < children.size(); i++)
    {
        BeehaveNode *child = Object::cast_to<BeehaveNode>(children[i]);
        if (child->get_index() < successful_index)
            continue;
        if (child != running_child)
            child->before_run(actor, blackboard);

        int response = child->safe_tick(actor, blackboard);
        if (can_send_message(blackboard))
            GD_LOG_INFO("Actor ID={0}, Child Index={1}, Response={2} Blackboard={3}",
                        actor->get_instance_id(), child->get_index(), response, blackboard->get_debug_data());

        ConditionLeaf *condition = Object::cast_to<ConditionLeaf>(child);
        if (condition != nullptr)
        {
            String id = String::num_int64(actor->get_instance_id());
            blackboard->set_value("last_condition", child, id);
            blackboard->set_value("last_condition_status", response, id);
        }

        switch (response)
        {
        case SUCCESS:
            if (running_child != nullptr && running_child == child)
            {
                // Do not interrupt as this child finishes running.
                _cleanup_running(running_child, actor, blackboard);
            }
            successful_index += 1;
            child->after_run(actor, blackboard);
            break;

        case FAILURE:
            if (running_child != nullptr)
            {
                running_child->interrupt(actor, blackboard);
                _cleanup_running(running_child, actor, blackboard);
            }
            _interrupt_children(actor, blackboard, i, previous_failure_or_running_index);
            previous_failure_or_running_index = child->get_index();
            successful_index = 0;

            // Interrupt any child that was RUNNING before but do not reset.
            if (running_child != nullptr)
            {
                running_child->interrupt(actor, blackboard);
                running_child = nullptr;
            }

            child->after_run(actor, blackboard);
            return FAILURE;

        case RUNNING:
            if (running_child != nullptr && child != running_child)
            {
                running_child->interrupt(actor, blackboard);
                _cleanup_running(running_child, actor, blackboard);
            }

            if (child != running_child)
                running_child = child;
            ActionLeaf *action = Object::cast_to<ActionLeaf>(child);
            if (action != nullptr)
            {
                String id = String::num_int64(actor->get_instance_id());
                blackboard->set_value("running_action", child, id);
            }
            _interrupt_children(actor, blackboard, i, previous_failure_or_running_index);
            previous_failure_or_running_index = i;
            return RUNNING;
        }
    }

    successful_index = 0;
    return SUCCESS;
}

void SequenceComposite::interrupt(Node *actor, Blackboard *blackboard)
{
    _interrupt_children(actor, blackboard, successful_index - 1, previous_failure_or_running_index);
    if (running_child != nullptr)
    {
        running_child->interrupt(actor, blackboard);
        _cleanup_running(running_child, actor, blackboard);
    }
    _reset();
    Composite::interrupt(actor, blackboard);
}

void SequenceComposite::_reset()
{
    successful_index = 0;
    previous_failure_or_running_index = -1;
}

TypedArray<StringName> SequenceComposite::get_class_name() const
{
    TypedArray<StringName> classes = Composite::get_class_name();
    classes.push_back(StringName("SequenceComposite"));
    return classes;
}

void SequenceComposite::_bind_methods()
{
}
