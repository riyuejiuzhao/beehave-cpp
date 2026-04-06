#include "selector_composite.hpp"
#include "../leaves/action_leaf.hpp"
#include "../../debug/debugger_messages.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

SelectorComposite::SelectorComposite()
    : last_execution_index(0)
    , previous_success_or_running_index(-1)
    , ready_to_interrupt_all(false)
{
}

SelectorComposite::~SelectorComposite()
{
}

int SelectorComposite::tick(Node *actor, Blackboard *blackboard)
{
    TypedArray<Node> children = get_children();
    int children_count = children.size();
    int processed_count = 0;

    for (int i = 0; i < children.size(); i++)
    {
        BeehaveNode *child = Object::cast_to<BeehaveNode>(children[i]);
        if (child == nullptr)
            continue;

        if (child->get_index() < last_execution_index)
        {
            processed_count++;
            continue;
        }

        if (child != running_child)
            child->before_run(actor, blackboard);

        int response = child->safe_tick(actor, blackboard);
        processed_count++;

        switch (response)
        {
        case SUCCESS:
            if (running_child != nullptr)
            {
                if (running_child != child)
                    running_child->interrupt(actor, blackboard);
                _cleanup_running(running_child, actor, blackboard);
            }
            child->after_run(actor, blackboard);
            _interrupt_children(actor, blackboard, i, previous_success_or_running_index);
            previous_success_or_running_index = i;
            ready_to_interrupt_all = false;
            return SUCCESS;

        case FAILURE:
            if (running_child != nullptr && running_child == child)
                _cleanup_running(running_child, actor, blackboard);
            child->after_run(actor, blackboard);
            last_execution_index = std::max(last_execution_index, child->get_index() + 1);
            break;

        case RUNNING:
            if (child != running_child)
            {
                if (running_child != nullptr)
                    running_child->interrupt(actor, blackboard);
                running_child = child;
            }
            auto action = Object::cast_to<ActionLeaf>(child);
            if (action != nullptr)
            {
                String id = String::num_int64(actor->get_instance_id());
                blackboard->set_running_action(action, id);
            }
            _interrupt_children(actor, blackboard, i, previous_success_or_running_index);
            previous_success_or_running_index = i;
            ready_to_interrupt_all = false;
            return RUNNING;
        }
    }

    // all children failed
    ready_to_interrupt_all = (processed_count == children_count);
    last_execution_index = 0;
    return FAILURE;
}

void SelectorComposite::after_run(Node *actor, Blackboard *blackboard)
{
    last_execution_index = 0;
    Composite::after_run(actor, blackboard);
}

void SelectorComposite::interrupt(Node *actor, Blackboard *blackboard)
{
    if (ready_to_interrupt_all)
    {
        // If all children failed, interrupt all children by using indices 0 and children.size()-1
        TypedArray<Node> children = get_children();
        if (children.size() > 0)
            _interrupt_children(actor, blackboard, -1, children.size() - 1);
        ready_to_interrupt_all = false;
    }
    else
    {
        // Use the normal interrupt logic for partial processing
        _interrupt_children(actor, blackboard, last_execution_index, previous_success_or_running_index);
    }
    
    if (running_child != nullptr)
    {
        running_child->interrupt(actor, blackboard);
        _cleanup_running(running_child, actor, blackboard);
    }
    
    last_execution_index = 0;
    previous_success_or_running_index = -1;
    Composite::interrupt(actor, blackboard);
}

TypedArray<StringName> SelectorComposite::get_class_name() const
{
    TypedArray<StringName> classes = Composite::get_class_name();
    classes.push_back(StringName("SelectorComposite"));
    return classes;
}

void SelectorComposite::_bind_methods()
{
}
